#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 lightRotation;


//ball object
glm::mat4 modelObj;

//bird object
glm::mat4 modelObj2;

//apple object
glm::mat4 modelObjApple;


// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

//fog uniform Location
GLint fogDensityLoc;

//spotlight uniform Location
GLint spotlightLoc;

int width = 1020;
int height = 600;

// camera
gps::Camera myCamera(
    glm::vec3(20.0f, 10.0f, -50.0f),
    glm::vec3(0.0f, 10.0f, 30.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.2f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
GLfloat angle = 0.0f;


//light Angle 
GLfloat lightAngle;
glm::mat4 lightRot;
glm::mat4 modelLight;

//light dir and color for other light sources
glm::vec3 lightDir2;
glm::vec3 lightColor2;

GLint lightDirLoc2;
GLint lightColorLoc2;

//ball
gps::Model3D ball;
GLfloat angle_ball =10.0f;

//bird
gps::Model3D bird;

//apple
gps::Model3D apple;





glm::vec3 cameraPosition;
GLint cameraPositionLoc;


glm::float32 fog_density = 0.00f;


// shaders
gps::Shader myBasicShader;

// shaders
gps::Shader lightShader;
//shadow
gps::Shader depthMapShader;


//Shadow
GLuint shadowMapFBO;
GLuint depthMapTexture;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;


float val = 0.0f;
int firstpress = 0;
bool animate=0;
int front = 1, back =0;


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	
    WindowDimensions win;
    
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);

    win.width = width;
    win.height = height;


    myWindow.setWindowDimensions(win);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}
//float sensitivity = 0.15f;
bool mouse_click_once = true;
float prevposX = 400;
float prevposY = 300;
float pitch =0.0f; 
float yaw = -90.0f;
float sensitivity = 0.5f;
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

   


    
       if (mouse_click_once)
        {
           
            mouse_click_once = false;
            prevposX = xpos;
            prevposY = ypos;
        }

     



         yaw += (xpos - prevposX) * sensitivity;
        pitch += (prevposY - ypos) * sensitivity;

        prevposX = xpos;
        prevposY = ypos;

       

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

 
        myCamera.rotate(pitch, yaw);
      view = myCamera.getViewMatrix();
              myBasicShader.useShaderProgram();
          glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

          normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}
bool animation = true;
float val2 = 0, val3 = 0.1;
float apple_anim = 0.0f;
bool apple_animation = false;

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    if (pressedKeys[GLFW_KEY_Y]) {
        myCamera.move(gps::MOVE_UP, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_T]) {
        myCamera.move(gps::MOVE_DOWN, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_F]) {
        if (val >= 0)
            val += 1.0f;
        // update model matrix for teapot
        modelObj = glm::rotate(glm::mat4(1.0f), glm::radians(val), glm::vec3(0, 0, 1));
   
    }

    if (pressedKeys[GLFW_KEY_G]) {

        fog_density += 0.0001f;

        myBasicShader.useShaderProgram();
        glUniform1f(fogDensityLoc, fog_density);
      
    }
    if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
       cameraSpeed = 0.7f;
    }
    else if (!pressedKeys[GLFW_KEY_LEFT_SHIFT])
    {
        cameraSpeed = 0.1f;
    }

    if (pressedKeys[GLFW_KEY_J])
    {

        myBasicShader.useShaderProgram();
        firstpress = 1;
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "firstpress"), firstpress);
    }
    if (pressedKeys[GLFW_KEY_H])
    {

        myBasicShader.useShaderProgram();
        firstpress = 0;
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "firstpress"), firstpress);
    }

    if (pressedKeys[GLFW_KEY_O]) {


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    }

    if (pressedKeys[GLFW_KEY_N])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    if (pressedKeys[GLFW_KEY_P]) {

        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[GLFW_KEY_K])
    {

        GLint MaxPatchVertices = 5;
        glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
        printf("Max supported patch vertices %d\n", MaxPatchVertices);
        glPatchParameteri(GL_PATCH_VERTICES, 3);

    }
    if (pressedKeys[GLFW_KEY_B])
    {
        animation = !animation;

        
    }
    if (pressedKeys[GLFW_KEY_V ])
    {
        apple_animation = true;


    }
}

void initOpenGLWindow() {
    myWindow.Create(width, height, "Small Town");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    teapot.LoadModel("models/scene/scene.obj");
    bird.LoadModel("models/bird/bird.obj");
    apple.LoadModel("models/apple/apple.obj");
   

}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");

    depthMapShader.loadShader(
        "shaders/depthMapShader.vert",
        "shaders/depthMapShader.frag");
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 2000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 50.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");

    lightRot = glm::rotate(glm::mat4(1.0f),glm::radians(lightAngle),glm::vec3(0.0f,1.0f,0.0f));
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    //set fog density
    
    fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    // send light color to shader
    
    //spotlightLoc = glGetUniformLocation(myBasicShader.shaderProgram, "firstpress");

    cameraPosition = myCamera.getcamPos();
    cameraPositionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "cameraPos");
    glUniform3fv(cameraPositionLoc,1,glm::value_ptr(cameraPosition));

   
}




void renderApple(gps::Shader shader)
{
    glm::mat4 start(1.0f);
    shader.useShaderProgram();


    modelObjApple = glm::rotate(start, glm::radians(angle), glm::vec3(0, 1, 0));
    modelObjApple = glm::translate(modelObjApple, glm::vec3(143.0f, 35.0f, 26.0f));
    modelObjApple = glm::scale(modelObjApple, glm::vec3(0.6f, 0.6f, 0.6f));
   // modelObjApple = glm::scale(start, glm::vec3(3.0f, 3.0f, 3.0f));

    
    if (apple_animation )  
        if (apple_anim < 64)
    {
        modelObjApple = glm::translate(modelObjApple, glm::vec3(0.0f, -apple_anim, 0.0f));
        apple_anim += 0.5f;
    } 
      else  modelObjApple = glm::translate(modelObjApple, glm::vec3(0.0f, -apple_anim, 0.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelObjApple));


    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverseTranspose(view * modelObjApple))));



    apple.Draw(shader);

}

void renderBirdMovement(gps::Shader shader ) {


    glm::mat4 start(1.0f);
    shader.useShaderProgram();


    modelObj2 = glm::rotate(start, glm::radians(angle), glm::vec3(0, 1, 0));
    modelObj2 = glm::rotate(modelObj2, glm::radians(270.0f), glm::vec3(0, 1, 0));
    modelObj2 = glm::translate(modelObj2, glm::vec3(-100.0f, -4.0f, -100.0f));
    modelObj2 = glm::scale(modelObj2, glm::vec3(3.0f, 3.0f, 3.0f));


    if (val2 < 1.4)
    {
        modelObj2 = glm::translate(modelObj2, glm::vec3(0.0f, val2, 0.0f));
      
        val2 += 0.1f;

        val3 = 1.4f;
    }
    else if (val3 > 0)
    {

        modelObj2 = glm::translate(modelObj2, glm::vec3(0.0f, val3, 0.0f));
      
        val3 -= 0.1f;

    }
    else val2 = 0.0f;


    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelObj2));

   
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverseTranspose(view * modelObj2))));

   

    bird.Draw(shader);

}





void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();



    //send teapot model matrix data to shader
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
 
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
}
void renderWithShadow(gps::Model3D* object, int  type)
{
    depthMapShader.useShaderProgram();
    glm::mat4 start(1.0f);
    if (type == 1)  // apple
    {
        
        modelObjApple = glm::rotate(start, glm::radians(angle), glm::vec3(0, 1, 0));
        modelObjApple = glm::translate(modelObjApple, glm::vec3(143.0f, 35.0f, 26.0f));
        modelObjApple = glm::scale(modelObjApple, glm::vec3(0.6f, 0.6f, 0.6f));

          glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelObjApple));

    }
    else if (type  == 2) // bird
    {
        modelObj2 = glm::rotate(start, glm::radians(angle), glm::vec3(0, 1, 0));
        modelObj2 = glm::rotate(modelObj2, glm::radians(270.0f), glm::vec3(0, 1, 0));
        modelObj2 = glm::translate(modelObj2, glm::vec3(-100.0f, -4.0f, -100.0f));
        modelObj2 = glm::scale(modelObj2, glm::vec3(3.0f, 3.0f, 3.0f));

        glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelObj2));
    }


    if ( type == 0)
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    (*object).Draw(depthMapShader);

}
glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 150.0f;
    glm::mat4 lightProjection = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;

}
void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the scene


    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

   
    renderWithShadow(&teapot,0);
    renderWithShadow(&bird,2);
    renderWithShadow(&apple,1);

    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    

        // final scene rendering pass 

        glViewport(0, 0, (float)myWindow.getWindowDimensions().width, (float)myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

     //   drawObjects(myBasicShader, false);
    




	
	renderTeapot(myBasicShader);
    renderBirdMovement(myBasicShader);
    renderApple(myBasicShader);
 
  
}

void initFBO() {
    //Create the FBO, the depth texture and attach the depth texture to the FBO - taken from laboratory 
    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
        0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);



}


void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}
int forward =0 ;
int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    initFBO();
    setWindowCallbacks();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();

        if (animation)
        {

            if (forward < 100)
            {
                myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
                view = myCamera.getViewMatrix();
                myBasicShader.useShaderProgram();
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                // compute normal matrix for teapot
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
                forward++;

            }
            else{
                yaw += 0.3;
                myCamera.rotate(pitch,yaw);

                view = myCamera.getViewMatrix();
                myBasicShader.useShaderProgram();
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                // compute normal matrix for teapot
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

                
          

            }
           
       
         }

   

	    renderScene();
      
		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
