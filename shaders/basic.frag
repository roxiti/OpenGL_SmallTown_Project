#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;


//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;


//fog
uniform float fogDensity;
uniform vec3 cameraPos;
uniform int firstpress;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

float computeFog()
{
 //float fogDensity = 0.05f;

  //compute eye space coordinates
    vec4 fragmentPosEyeSpace = view * model * vec4(fPosition, 1.0f);

 float fragmentDistance = length(fragmentPosEyeSpace);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

float computeShadow()
{
	// perform perspective divide
vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
// Transform to [0,1] range
normalizedCoords = normalizedCoords * 0.5 + 0.5;
// Get closest depth value from light's perspective
float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
// Get depth of current fragment from light's perspective
float currentDepth = normalizedCoords.z;
//Check wheter current frag pos is in shadow
float bias= 0.005f;
float shadow= currentDepth - bias > closestDepth ? 1.0 : 0.0;
return shadow;

}

vec3 spotLight()
{

    
    vec3 position =  vec3(3.0f, 3.0f, -10.0f);
    vec3 lightColor2 = vec3 (0.2f,0.2f,0.0f);
    vec3 lightDir2 = normalize ( vec3(-70.0f, 4.0f, -2.0f) - fPosition);

	// controls how big the area that is lit up is
	float outerCone = 0.50f;
	float innerCone = 0.75f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(normalMatrix * fNormal);
	
	float diffuse = max(dot(normal, lightDir2), 0.0f);



	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(cameraPos - fPosition);
	vec3 reflectionDirection = reflect(-lightDir2, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir2);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuseTexture, fTexCoords).rgb * (diffuse * inten + ambient) + texture(specularTexture, fTexCoords).r * specular * inten) * lightColor2;
}


void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;

   // return min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
    

}

void main() 
{
    //compute fog 
    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

   vec3 finallight;
   vec3 color;

    computeDirLight();

    ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

    float shadow = computeShadow();

   // finallight = 
    
   if (firstpress == 1)
     { 
        finallight = spotLight();
     } 
  else finallight = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f); 


 
    
    //compute final vertex color
   // vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);


   

     fColor = fogColor * (1 - fogFactor) + vec4(finallight,1.0f) * fogFactor;
       
   
}
