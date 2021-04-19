#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H  
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Shader/Shader.h"
#include "Skybox/Skybox.h"
#include "Camera/Camera.h"
#include "Light/Light.h"
#include "Components/Component.h"
#include "GameLogic/Proctor.h"
#include "GameLogic/Hierarchy.h"
#include "Components/MeshRenderer.h"

/* Load 3SE packages */
#include "Loader/Loader.h"
#include "InputSystem/InputSystem.h"
#include "UIRender/UIRender.h"

#include <iostream>
#include <map>
#include <string>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cameraMouseInput(GLFWwindow* window, InputSystem::MouseInput *mouse);
void cameraKeyboardInput(GLFWwindow* window, InputSystem::KeyboardInput *keyboard);
void mouseOusideWindowsPos(int key, InputSystem::KeyboardInput* keyboard, InputSystem::MouseInput* mouse);

// Those functions will define position of given model 
void keyboardMovementWSAD(float* positionData, Proctor* _proctor, InputSystem::KeyboardInput* keyboard);
void keyboardMovementIJKL(float* positionData, Proctor* _proctor, InputSystem::KeyboardInput* keyboard);

//Switch camera position(debug)
void cameraSwitch(int cameraPositionY,InputSystem::MouseInput *mouseInput, InputSystem::KeyboardInput *keyboardInput, GLFWwindow* window, Proctor* player_1, Proctor* player_2);

// settings
const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// movement
float positionOfWsadObject[3] = { 0, 0, 0 };
float positionOfIjklObject[3] = { 0, 0, 0 };

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool isDebugModeOn = false;

int main()
{
    glm::mat4 projection;
    glm::mat4 textProjection;
    glm::mat4 view;
	glm::mat4 model;
	
#pragma region GLFW init
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Initialize GLEW to setup the OpenGL Function pointers
    //glewExperimental = GL_TRUE;
    //glewInit();

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#pragma endregion

#pragma region ImGUI init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
    ImGui::StyleColorsDark();
    int hierarchyCurrentItem = 0;

#pragma endregion

    /* Create shaders */
    Shader model3D("assets/shaders/model3D.vert", "assets/shaders/model3D.frag");
    Shader textShader("assets/shaders/text.vert", "assets/shaders/text.frag");

    /* Text init */
    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH), 0.0f, static_cast<GLfloat>(SCREEN_WIDTH));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "textProjection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    /* Create Skybox */
    Skybox skybox(&view, &projection, &camera);
	
    /* Create InputSystem elements */
    InputSystem::MouseInput* mouseInput = new InputSystem::MouseInput(window);
    InputSystem::KeyboardInput* keyboardInput = new InputSystem::KeyboardInput(window);

    /* Create object hierarchy */
    Hierarchy hierarchy;

    /* Load models */
    positionOfWsadObject[0] = -12.0f;
    positionOfWsadObject[1] = -12.0f;
    positionOfWsadObject[2] = -12.0f;
    Loader::Model troll_00_model;
    troll_00_model.loadModel("assets/models/lotr_troll/scene.gltf");
    positionOfIjklObject[0] = 12.0f;
    positionOfIjklObject[1] = -12.0f;
    positionOfIjklObject[2] = -12.0f;
    Loader::Model troll_01_model;
    troll_01_model.loadModel("assets/models/lotr_troll/scene.gltf");

    Loader::Model modelJakis;
    modelJakis.loadModel("assets/models/cube/untitled.obj");

    /* Load hierarchy */
    // TROLL 00
    Proctor troll_00("troll_00", 0, NULL);
    troll_00.setPosition(glm::vec3(-12.0f));
    troll_00.setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    troll_00.setScale(glm::vec3(0.02f));
    MeshRenderer troll_00_mr(C_MESH, &troll_00);
    troll_00_mr.setModel(&troll_00_model);
    troll_00_mr.setShader(model3D);
    troll_00.addComponent(&troll_00_mr);
    hierarchy.addObject(&troll_00);
    // TROLL 01
    Proctor troll_01("troll_01", 0, NULL);
    troll_01.setPosition(glm::vec3(12.0f, -12.0f, -12.0f));
    troll_01.setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    troll_01.setScale(glm::vec3(0.02f));
    MeshRenderer troll_01_mr(C_MESH, &troll_01);
    troll_01_mr.setModel(&troll_01_model);
    troll_01_mr.setShader(model3D);
    troll_01.addComponent(&troll_01_mr);
    hierarchy.addObject(&troll_01);
	// JAKIS MODEL
    Proctor modelJakis_00("modelJakis_00", 0, NULL);
    modelJakis_00.setPosition(glm::vec3(-10.0f));
    modelJakis_00.setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    modelJakis_00.setScale(glm::vec3(1.f));
    MeshRenderer modelJakis_mr(C_MESH, &modelJakis_00);
    modelJakis_mr.setModel(&modelJakis);
    modelJakis_mr.setShader(model3D);
    troll_00.addComponent(&modelJakis_mr);
    hierarchy.addObject(&modelJakis_00);

    /* Lights */
    DirLight dirLight = {
        glm::vec3(-0.2f, -1.0f, -0.3f), 
        glm::vec3(0.1f), 
        glm::vec3(0.4f),
        glm::vec3(0.75f)
    };

    /* Render loop */
    while (!glfwWindowShouldClose(window))
    {
        /* Clear screen */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /* Dear ImGUI new frame setup */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* Per-frame time logic */
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* Use InputSystem to move camera */
        /* Use 'P' to switch between camera modes, CTRL to look around, 'R' to get cursor position*/
        cameraSwitch(40, mouseInput, keyboardInput, window, &troll_00, &troll_01);
              
        glEnable(GL_DEPTH_TEST);
        model3D.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);//glm::mat4(1.0f);
        model3D.setUniform("projection", projection);
        view = camera.GetViewMatrix(); //glm::mat4(1.0f);
        model3D.setUniform("view", view);
        model = glm::mat4(1.0f);
        dirLight.render(model3D);

        /* Collect Input */
        keyboardMovementWSAD(positionOfWsadObject, &troll_00, keyboardInput);
        keyboardMovementIJKL(positionOfIjklObject, &troll_01, keyboardInput);

        /* Render models */
        hierarchy.update();

        /* Sky-box -- Must be rendered almost last, before hud */
        skybox.render(); 

        /* DEBUG - Draw DearImGUI */
        ImGui::Render();
        
        /* Update InputSystem */
        keyboardInput->update();
        mouseInput->update();

        /* Clear frame at the end of the loop */
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    troll_01_model.cleanup();
    troll_00_model.cleanup();
    modelJakis.cleanup();
	
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void cameraMouseInput(GLFWwindow* window, InputSystem::MouseInput *mouse)
{
    if (firstMouse)
    {
        lastX = mouse->getCursorPosition().x;
        lastY = mouse->getCursorPosition().y;
        firstMouse = false;
    }

    float xoffset = mouse->getCursorPosition().x - lastX;
    float yoffset = lastY - mouse->getCursorPosition().y; // reversed since y-coordinates go from bottom to top

    lastX = mouse->getCursorPosition().x;
    lastY = mouse->getCursorPosition().y;

    camera.ProcessMouseMovement(xoffset, yoffset);
    camera.ProcessMouseScroll(mouse->getScrollValue());
}

void cameraKeyboardInput(GLFWwindow* window, InputSystem::KeyboardInput *keyboard)
{
    if (keyboard->isKeyDown(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);
    if (keyboard->isKeyDown(GLFW_KEY_UP))
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keyboard->isKeyDown(GLFW_KEY_DOWN))
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keyboard->isKeyDown(GLFW_KEY_LEFT))
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keyboard->isKeyDown(GLFW_KEY_RIGHT))
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouseOusideWindowsPos(int key, InputSystem::KeyboardInput* keyboard, InputSystem::MouseInput* mouse)
{
    if (keyboard->isKeyDown(key)) {
        std::cout << mouse->getCursorPosition().x << "    " << mouse->getCursorPosition().y << "\n";
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void keyboardMovementWSAD(float* positionData, Proctor* _proctor, InputSystem::KeyboardInput* keyboard)
{
    if (keyboard->isKeyDown(GLFW_KEY_W))
    {
        positionData[2] -= 0.2;
        _proctor->setPosition(glm::vec3(positionData[0], positionData[1], positionData[2]));
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(180.0f), 0.0f));
    }

    if (keyboard->isKeyDown(GLFW_KEY_S))
    {
        positionData[2] += 0.2;
        _proctor->setPosition(glm::vec3(positionData[0], positionData[1], positionData[2]));
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(0.0f), 0.0f));
    }

    if (keyboard->isKeyDown(GLFW_KEY_A))
    {
        positionData[0] -= 0.2;
        _proctor->setPosition(glm::vec3(positionData[0], positionData[1], positionData[2]));
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(-90.0f), 0.0f));
    }

    if (keyboard->isKeyDown(GLFW_KEY_D))
    {
        positionData[0] += 0.2;
        _proctor->setPosition(glm::vec3(positionData[0], positionData[1], positionData[2]));
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(90.0f), 0.0f));
    }

    if (keyboard->isKeyDown(GLFW_KEY_W) && keyboard->isKeyDown(GLFW_KEY_A))
    {
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(-135.0f), 0.0f));
    }
    if (keyboard->isKeyDown(GLFW_KEY_W) && keyboard->isKeyDown(GLFW_KEY_D))
    {
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(135.0f), 0.0f));
    }
    if (keyboard->isKeyDown(GLFW_KEY_S) && keyboard->isKeyDown(GLFW_KEY_A))
    {
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(-45.0f), 0.0f));
    }
    if (keyboard->isKeyDown(GLFW_KEY_S) && keyboard->isKeyDown(GLFW_KEY_D))
    {
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(45.0f), 0.0f));
    }
}

void keyboardMovementIJKL(float* positionData, Proctor* _proctor, InputSystem::KeyboardInput* keyboard)
{
    if (keyboard->isKeyDown(GLFW_KEY_I))
    {
        positionData[2] -= 0.2;
        _proctor->setPosition(glm::vec3(positionData[0], positionData[1], positionData[2]));
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(180.0f), 0.0f));
    }
   
    if (keyboard->isKeyDown(GLFW_KEY_K))
    {
        positionData[2] += 0.2;
        _proctor->setPosition(glm::vec3(positionData[0], positionData[1], positionData[2]));
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(0.0f), 0.0f));
    }
    
    if (keyboard->isKeyDown(GLFW_KEY_J))
    {
        positionData[0] -= 0.2;
        _proctor->setPosition(glm::vec3(positionData[0], positionData[1], positionData[2]));
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(-90.0f), 0.0f));
    }
   
    if (keyboard->isKeyDown(GLFW_KEY_L)) 
    {
        positionData[0] += 0.2;
        _proctor->setPosition(glm::vec3(positionData[0], positionData[1], positionData[2]));
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(90.0f), 0.0f));
    }

    if (keyboard->isKeyDown(GLFW_KEY_I) && keyboard->isKeyDown(GLFW_KEY_J))
    {
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(-135.0f), 0.0f));
    }
    if (keyboard->isKeyDown(GLFW_KEY_I) && keyboard->isKeyDown(GLFW_KEY_L))
    {
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(135.0f), 0.0f));
    }
    if (keyboard->isKeyDown(GLFW_KEY_K) && keyboard->isKeyDown(GLFW_KEY_J))
    {
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(-45.0f), 0.0f));
    }
    if (keyboard->isKeyDown(GLFW_KEY_K) && keyboard->isKeyDown(GLFW_KEY_L))
    {
        _proctor->setRotation(glm::quat(1.0f, 0.0f, glm::radians(45.0f), 0.0f));
    }
    
}

void cameraSwitch(int cameraPositionY, InputSystem::MouseInput *mouseInput, InputSystem::KeyboardInput *keyboardInput, GLFWwindow* window, Proctor *player_1, Proctor *player_2)
{
    if (keyboardInput->isKeyPressed(GLFW_KEY_P)) {
        isDebugModeOn = !isDebugModeOn;
    }

    if (isDebugModeOn)
    {
        cameraKeyboardInput(window, keyboardInput); 
        mouseOusideWindowsPos(GLFW_KEY_R, keyboardInput, mouseInput);

        if (keyboardInput->isKeyDown(GLFW_KEY_LEFT_CONTROL))
        {
            cameraMouseInput(window, mouseInput);
            mouseInput->cursorDisable();
        }
        if (keyboardInput->isKeyReleased(GLFW_KEY_LEFT_CONTROL))
        {
            firstMouse = true; // reset last saved mosue position
        }
    }
    if (!isDebugModeOn)
    {
        camera.Yaw = -90; // set yaw to default value
        camera.ProcessMouseMovement(0, -89); // set pitch to default value
        camera.Position = glm::vec3((player_1->getPosition()[0] + player_2->getPosition()[0]) / 2.f, cameraPositionY, (player_1->getPosition()[2] + player_2->getPosition()[2]) / 2.f);
        mouseInput->cursorEnable();
    }
}
