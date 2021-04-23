/* Load 3SE packages */
#include "Application/Application.h"
#include "Loader/Loader.h"
#include "InputSystem/InputSystem.h"
#include "GameLogic/GameLogic.h"
#include "UIRender/UIRender.h"
 
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H 
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Shader/Shader.h"
#include "Skybox/Skybox.h"
#include "Camera/Camera.h"
#include "Light/Light.h"
#include "GameLogic/Collisions/BoxCollider.h"

//#include <iostream>
#include <map>
//#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cameraMouseInput(GLFWwindow* window, InputSystem::MouseInput* mouse);
void cameraKeyboardInput(GLFWwindow* window, InputSystem::KeyboardInput* keyboard);
void mouseOusideWindowsPos(int key, InputSystem::KeyboardInput* keyboard, InputSystem::MouseInput* mouse);

// Those functions will define position of given model 
void keyboardMovementWSAD(float* positionData, GameLogic::Proctor* _proctor, InputSystem::KeyboardInput* keyboard, float yValueUp, float yValueDown, float xValueLeft, float xValueRight, float &speed);
void keyboardMovementIJKL(float* positionData, GameLogic::Proctor* _proctor, InputSystem::KeyboardInput* keyboard, float yValueUp, float yValueDown, float xValueLeft, float xValueRight, float &speed);

//Switch camera position(debug)
//maxDistanceY/maxDistanceX = ScreenHeight/ScreenWidth
void cameraSwitch(int minZoom, int maxZoom, float maxDistanceX, float maxDistanceY, InputSystem::MouseInput* mouseInput, InputSystem::KeyboardInput* keyboardInput, GLFWwindow* window, GameLogic::Proctor* player_1, GameLogic::Proctor* player_2,
    float& yValueUp, float& yValueDown, float& xValueLeft, float& xValueRight);

// Collision functions
bool checkAABBCollision(BoxCollider a, BoxCollider b);

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
const float acceleration = 0.005f;

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

    /* Load scene */
    Application::Scene mainScene("3S GameEngine", SCREEN_WIDTH, SCREEN_HEIGHT, false);
    glfwMakeContextCurrent(mainScene.window);
    glfwSetFramebufferSizeCallback(mainScene.window, framebuffer_size_callback);
    glfwSetInputMode(mainScene.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Load all OpenGL function pointers */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

#pragma region ImGUI init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(mainScene.window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
    ImGui::StyleColorsDark();
    int hierarchyCurrentItem = 0;

#pragma endregion

    /* Create shaders */
    Shader model3D("assets/shaders/model3D.vert", "assets/shaders/model3D.frag");
    Shader textShader("assets/shaders/text.vert", "assets/shaders/text.frag");
    Shader collisionBoxShader("assets/shaders/boxCollider.vert", "assets/shaders/boxCollider.frag", "assets/shaders/boxCollider.geom");
	
    /* Text init */
    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH), 0.0f, static_cast<GLfloat>(SCREEN_WIDTH));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "textProjection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    /* Create Skybox */
    Skybox skybox(&view, &projection, &camera);

    /* Create InputSystem elements */
    InputSystem::MouseInput* mouseInput = new InputSystem::MouseInput(mainScene.window);
    InputSystem::KeyboardInput* keyboardInput = new InputSystem::KeyboardInput(mainScene.window);

    /* Create object hierarchy */
    GameLogic::Hierarchy hierarchy;

    /* Load models */
    Loader::Model troll_01_model;
    troll_01_model.loadModel("assets/models/lotr_troll/scene.gltf");

    Loader::Model hero_00_model;
    hero_00_model.loadModel("assets/models/hero/hero_noanim.fbx");

    Loader::Model modelJakis_00_model;
    modelJakis_00_model.loadModel("assets/models/cube/untitled.obj");

    Loader::Model modelJakis_01_model;
    modelJakis_01_model.loadModel("assets/models/cube/untitled.obj");

    /* Load hierarchy */
    // hero_00 - configure proctor
    GameLogic::Proctor hero_00("hero_00", 0, NULL);
    hero_00.setPosition(glm::vec3(0.0f));
    hero_00.setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    hero_00.setScale(glm::vec3(0.3f));
    // hero_00 - add mesh renderer component
    GameLogic::MeshRenderer hero_00_mr(GameLogic::C_MESH, &hero_00);
    hero_00_mr.setModel(&hero_00_model);
    hero_00_mr.setShader(model3D);
    hero_00.addComponent(&hero_00_mr);
    // hero_00 - add movement component
    GameLogic::PlayerInput hero_00_pi(GameLogic::C_MOVEMENT, &hero_00, true);
    hero_00.addComponent(&hero_00_pi);
    // hero_00 - add object to hierarchy
    hierarchy.addObject(&hero_00);
	
    // troll_01 - configure proctor
    GameLogic::Proctor troll_01("troll_01", 0, NULL);
    troll_01.setPosition(glm::vec3(0.0f));
    troll_01.setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    troll_01.setScale(glm::vec3(0.02f));
    // troll_01 - add mesh renderer component
    GameLogic::MeshRenderer troll_01_mr(GameLogic::C_MESH, &troll_01);
    troll_01_mr.setModel(&troll_01_model);
    troll_01_mr.setShader(model3D);
    troll_01.addComponent(&troll_01_mr);
    // troll_01 - add movement component
    GameLogic::PlayerInput troll_01_pi(GameLogic::C_MOVEMENT, &troll_01, false);
    troll_01.addComponent(&troll_01_pi);
    // TROLL 01 - add object to hierarchy
    hierarchy.addObject(&troll_01);
	
    // JAKIS MODEL 00
    GameLogic::Proctor modelJakis_00("modelJakis_00", 0, NULL);
    modelJakis_00.setPosition(glm::vec3(-4.0f));
    modelJakis_00.setRotation(glm::quat(0.0f, 0.0f, 0.0f, 0.0f));
    modelJakis_00.setScale(glm::vec3(1.f));
    GameLogic::MeshRenderer modelJakis_mr_00(GameLogic::C_MESH, &modelJakis_00);
    modelJakis_mr_00.setModel(&modelJakis_00_model);
    modelJakis_mr_00.setShader(model3D);
    modelJakis_00.addComponent(&modelJakis_mr_00);
    hierarchy.addObject(&modelJakis_00);
	
    // JAKIS MODEL 01
    GameLogic::Proctor modelJakis_01("modelJakis_01", 0, NULL);
    modelJakis_01.setPosition(glm::vec3(-10.0f));
    modelJakis_01.setRotation(glm::quat(0.0f, 0.0f, 0.0f, 0.0f));
    modelJakis_01.setScale(glm::vec3(1.f));
    GameLogic::MeshRenderer modelJakis_mr_01(GameLogic::C_MESH, &modelJakis_01);
    modelJakis_mr_01.setModel(&modelJakis_01_model);
    modelJakis_mr_01.setShader(model3D);
    modelJakis_01.addComponent(&modelJakis_mr_01);
    hierarchy.addObject(&modelJakis_01);

    /* Lights */
    DirLight dirLight = {
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec3(0.9f),
        glm::vec3(0.6f),
        glm::vec3(0.75f)
    };

    /* Init Box Colliders for the models */
    BoxCollider someModelCollider_00(collisionBoxShader, &modelJakis_00_model, modelJakis_00_model.position, modelJakis_00_model.rotation , modelJakis_00_model.scale);
    BoxCollider someModelCollider_01(collisionBoxShader, &modelJakis_01_model, modelJakis_01_model.position, modelJakis_01_model.rotation, modelJakis_01_model.scale);
    
    float xValueRight = 0.2;
    float xValueLeft = 0.2;
    float yValueUp = 0.2;
    float yValueDown = 0.2;
    float WSADSpeed = 0.0;
    float IJKLSpeed = 0.0;

    /* Render loop */
    while (!glfwWindowShouldClose(mainScene.window))
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
        cameraSwitch(35,60,90,45, mouseInput, keyboardInput, mainScene.window, &hero_00, &troll_01, yValueUp, yValueDown, xValueLeft, xValueRight);

        glEnable(GL_DEPTH_TEST);
    	
    	/* Set up model 3D shader uniforms*/
        model3D.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);//glm::mat4(1.0f);
        model3D.setUniform("projection", projection);
        view = camera.GetViewMatrix(); //glm::mat4(1.0f);
        model3D.setUniform("view", view);
        model = glm::mat4(1.0f);

    	/* Render lights */
        dirLight.render(model3D);

        /* Collect input */
        keyboardMovementWSAD(positionOfWsadObject, &hero_00, keyboardInput, yValueUp, yValueDown, xValueLeft, xValueRight, WSADSpeed);
        keyboardMovementIJKL(positionOfIjklObject, &troll_01, keyboardInput, yValueUp, yValueDown, xValueLeft, xValueRight, IJKLSpeed);

        /* Render models */
        hierarchy.update();

        /* Set up universal collisionBoxShader uniforms */
        collisionBoxShader.use();
        collisionBoxShader.setUniform("view", view);
        collisionBoxShader.setUniform("projection", projection);
    	
        /* Update colliders positions, calc collisions */
    	// box 00
        someModelCollider_00.setScale(modelJakis_00_model.scale);
        someModelCollider_00.setRotation(modelJakis_00_model.rotation);
        someModelCollider_00.setPosition(modelJakis_00_model.position);

    	// box 01
        someModelCollider_01.setScale(modelJakis_01_model.scale);
        someModelCollider_01.setRotation(modelJakis_01_model.rotation);
        someModelCollider_01.setPosition(modelJakis_01_model.position);

        /* Update uniforms and render colliders */
    	// box 00
        collisionBoxShader.setUniform("model", someModelCollider_00.getTranslateMatrix() * someModelCollider_00.getScaleMatrix());
        collisionBoxShader.setUniform("radius", someModelCollider_00.getRadius() / someModelCollider_00.scale);
        collisionBoxShader.setUniformBool("collision", checkAABBCollision(someModelCollider_00, someModelCollider_01));
        someModelCollider_00.render();
    	
        // box 01
        collisionBoxShader.setUniform("model", someModelCollider_01.getTranslateMatrix() * someModelCollider_01.getScaleMatrix());
        collisionBoxShader.setUniform("radius", someModelCollider_01.getRadius() / someModelCollider_01.scale);
        collisionBoxShader.setUniformBool("collision", checkAABBCollision(someModelCollider_00, someModelCollider_01));
        someModelCollider_01.render();

        /* Sky-box -- Must be rendered almost last, before hud */
        skybox.render();

        /* DEBUG - Draw DearImGUI */
        ImGui::Render();

        /* Update InputSystem */
        keyboardInput->update();
        mouseInput->update();

        /* Clear frame at the end of the loop */
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(mainScene.window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    hierarchy.cleanup();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void cameraMouseInput(GLFWwindow* window, InputSystem::MouseInput* mouse)
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

void cameraKeyboardInput(GLFWwindow* window, InputSystem::KeyboardInput* keyboard)
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

void keyboardMovementWSAD(float* positionData, GameLogic::Proctor* _proctor, InputSystem::KeyboardInput* keyboard, float yValueUp, float yValueDown, float xValueLeft, float xValueRight, float &speed)
{
    if (keyboard->isKeyDown(GLFW_KEY_W))
    {
        speed += acceleration;
        if (speed >= yValueUp)
        {
            speed = yValueUp;
        }
    }

    if (keyboard->isKeyDown(GLFW_KEY_S))
    {
        speed += acceleration;
        if (speed >= yValueDown)
        {
            speed = yValueDown;
        }
    }

    if (keyboard->isKeyDown(GLFW_KEY_A))
    {
        speed += acceleration;
        if (speed >= xValueRight)
        {
            speed = xValueRight;
        }
    }

    if (keyboard->isKeyDown(GLFW_KEY_D))
    {
        speed += acceleration;
        if (speed >= xValueLeft)
        {
            speed = xValueLeft;
        }
    }
}

void keyboardMovementIJKL(float* positionData, GameLogic::Proctor* _proctor, InputSystem::KeyboardInput* keyboard, float yValueUp, float yValueDown, float xValueLeft, float xValueRight, float &speed)
{
    if (keyboard->isKeyDown(GLFW_KEY_I))
    {
        speed += acceleration;
        if (speed >= yValueDown)
        {
            speed = yValueDown;
        }
    }

    if (keyboard->isKeyDown(GLFW_KEY_K))
    {
        speed += acceleration;
        if (speed >= yValueUp)
        {
            speed = yValueUp;
        }
    }

    if (keyboard->isKeyDown(GLFW_KEY_J))
    {
        speed += acceleration;
        if (speed >= xValueLeft)
        {
            speed = xValueLeft;
        }
    }

    if (keyboard->isKeyDown(GLFW_KEY_L))
    {
        speed += acceleration;
        if (speed >= xValueRight)
        {
            speed = xValueRight;
        }
    }
}

void cameraSwitch(int minZoom,int maxZoom,float maxDistanceX, float maxDistanceY, InputSystem::MouseInput* mouseInput, InputSystem::KeyboardInput* keyboardInput, GLFWwindow* window, GameLogic::Proctor* player_1, GameLogic::Proctor* player_2,
    float& yValueUp, float& yValueDown, float& xValueLeft, float& xValueRight)
{
    if (keyboardInput->isKeyPressed(GLFW_KEY_P)) {
        isDebugModeOn = !isDebugModeOn;
    }

    if (isDebugModeOn)
    {
        cameraKeyboardInput(window, keyboardInput);
        mouseOusideWindowsPos(GLFW_KEY_R, keyboardInput, mouseInput);

        if (keyboardInput->isKeyDown(GLFW_KEY_RIGHT_CONTROL))
        {
            cameraMouseInput(window, mouseInput);
            mouseInput->cursorDisable();
        }
        if (keyboardInput->isKeyReleased(GLFW_KEY_RIGHT_CONTROL))
        {
            firstMouse = true; // reset last saved mosue position
            mouseInput->cursorEnable();
        }

    }
    if (!isDebugModeOn)
    {       
        float xDistance = player_1->getPosition()[0] - player_2->getPosition()[0];
        float yDistance = player_1->getPosition()[2] - player_2->getPosition()[2];
        float distance = sqrt(xDistance * xDistance + yDistance * yDistance);
        
        if (xDistance < -maxDistanceX) xValueRight = 0;
        else xValueRight = 0.2;

        if (xDistance > maxDistanceX) xValueLeft = 0;
        else xValueLeft = 0.2;

        if (yDistance < -maxDistanceY) yValueUp = 0;
        else yValueUp = 0.2;

        if (yDistance > maxDistanceY) yValueDown = 0;
        else yValueDown = 0.2;

        float camPos = minZoom + distance/maxDistanceX*(maxZoom-minZoom); //clamp
        camera.Yaw = -90; // set yaw to default value
        camera.ProcessMouseMovement(0, -89); // set pitch to default value
        camera.Position = glm::vec3((player_1->getPosition()[0] + player_2->getPosition()[0]) / 2.f, camPos, (player_1->getPosition()[2] + player_2->getPosition()[2]) / 2.f);

        mouseInput->cursorEnable();
    }
}

bool checkAABBCollision(BoxCollider a, BoxCollider b)
{
    if (glm::abs(a.position.x - b.position.x) > (a.getRadius().x + b.getRadius().x)) return false;
    if (glm::abs(a.position.y - b.position.y) > (a.getRadius().y + b.getRadius().y)) return false;
    if (glm::abs(a.position.z - b.position.z) > (a.getRadius().z + b.getRadius().z)) return false;
    return true;
}