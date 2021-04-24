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
//#include "GameLogic/Collisions/BoxCollider.h"

//#include <iostream>
#include <map>
//#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cameraMouseInput(GLFWwindow* window, InputSystem::MouseInput* mouse);
void cameraKeyboardInput(Application::Scene* _scene, InputSystem::KeyboardInput* _keyboard);
void mouseOusideWindowsPos(int key, InputSystem::KeyboardInput* keyboard, InputSystem::MouseInput* mouse);

//Switch camera position(debug)
//maxDistanceY/maxDistanceX = ScreenHeight/ScreenWidth
void cameraSwitch(int minZoom, int maxZoom, float maxDistanceX, float maxDistanceY, InputSystem::MouseInput* mouseInput, InputSystem::KeyboardInput* keyboardInput, Application::Scene* _scene, GameLogic::Proctor* player_1, GameLogic::Proctor* player_2,
    float& yValueUp, float& yValueDown, float& xValueLeft, float& xValueRight);

// Collision functions
bool checkAABBCollision(GameLogic::Proctor* _a, GameLogic::Proctor* _b);
void separateAABBCollision(GameLogic::Proctor* _a, GameLogic::Proctor* _b);

// settings
const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

bool isDebugModeOn = false;

int main()
{
    glm::mat4 projection;
    glm::mat4 textProjection;
    glm::mat4 view;
    glm::mat4 model;

    /* Load scene */
    Application::Scene mainScene("3S GameEngine", SCREEN_WIDTH, SCREEN_HEIGHT, false); // false - window, true - fullscreen 
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
    GameLogic::Hierarchy hierarchy(&mainScene);

    /* Load models */
    Loader::Model hero_00_model("assets/models/hero/untitled.fbx", true);
    Loader::Model hero_01_model("assets/models/hero/untitled.fbx", true);
    Loader::Model modelJakis_00_model("assets/models/cube/untitled.obj", false);
    Loader::Model modelJakis_01_model("assets/models/cube/untitled.obj", false);

    /* Load hierarchy */
    // hero_00 - configure proctor
    GameLogic::Proctor hero_00("hero_00", glm::vec3(2.0f, 2.5f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
    GameLogic::MeshRenderer hero_00_mr(GameLogic::C_MESH, &hero_00, &hero_00_model, &model3D);
    GameLogic::PlayerInput hero_00_pi(GameLogic::C_MOVEMENT, &hero_00, true);
    GameLogic::BoxCollider hero_00_bc(GameLogic::C_COLLIDER, &hero_00_model, &hero_00, &collisionBoxShader);
    hierarchy.addObject(&hero_00);

    // hero_01 - configure proctor
    GameLogic::Proctor hero_01("hero_01", glm::vec3(-2.0f, 2.5f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
    GameLogic::MeshRenderer hero_01_mr(GameLogic::C_MESH, &hero_01, &hero_00_model, &model3D);
    GameLogic::PlayerInput hero_01_pi(GameLogic::C_MOVEMENT, &hero_01, false);
    GameLogic::BoxCollider hero_01_bc(GameLogic::C_COLLIDER, &hero_01_model, &hero_01, &collisionBoxShader);
    hierarchy.addObject(&hero_01);
	
    // JAKIS MODEL 00
    GameLogic::Proctor modelJakis_00("modelJakis_00", glm::vec3(0.0f, -1.0f, 0.0f), glm::quat(1.0f, glm::vec3(0.0f)), glm::vec3(50.0f, 1.0f, 50.0f));
    GameLogic::MeshRenderer modelJakis_mr_00(GameLogic::C_MESH, &modelJakis_00, &modelJakis_00_model, &model3D);
    hierarchy.addObject(&modelJakis_00);
	
    // JAKIS MODEL 01
    GameLogic::Proctor modelJakis_01("modelJakis_01", glm::vec3(-10.0f));
    GameLogic::MeshRenderer modelJakis_mr_01(GameLogic::C_MESH, &modelJakis_01, &modelJakis_01_model, &model3D);
    hierarchy.addObject(&modelJakis_01);

    /* Lights */
    DirLight dirLight = {
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
        glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
        glm::vec4(0.75f, 0.75f, 0.75f, 1.0f)
    };

    /* Init Box Colliders for the models */
    //BoxCollider someModelCollider_00(collisionBoxShader, &modelJakis_00_model, modelJakis_00_model.position, modelJakis_00_model.rotation , modelJakis_00_model.scale);
    //BoxCollider someModelCollider_01(collisionBoxShader, &modelJakis_01_model, modelJakis_01_model.position, modelJakis_01_model.rotation, modelJakis_01_model.scale);
    //BoxCollider heroCollider_00(collisionBoxShader, &hero_00_model, hero_00_model.position, hero_00_model.rotation, hero_00_model.scale);
    //BoxCollider heroCollider_01(collisionBoxShader, &hero_01_model, hero_01_model.position, hero_01_model.rotation, hero_01_model.scale);
    
    float xValueRight = 0.2;
    float xValueLeft = 0.2;
    float yValueUp = 0.2;
    float yValueDown = 0.2;

    /* Load models for PGK task */
    // Load environment objects 
    Loader::Model palm_00_model("assets/models/task_models/palm_detailed_short.gltf", true);
    Loader::Model palm_01_model("assets/models/task_models/palm_long.gltf", true);
    Loader::Model palm_02_model("assets/models/task_models/palm_short.gltf", true);
    Loader::Model boat_small_model("assets/models/task_models/boat_small.gltf", true);
    Loader::Model chest_model("assets/models/task_models/chest.gltf", true);
    Loader::Model formationRock_00_model("assets/models/task_models/formation_rock.gltf", true);
    Loader::Model formationRock_01_model("assets/models/task_models/formation_rock.gltf", true);
    Loader::Model plant_00_model("assets/models/task_models/plant.gltf", true);
    Loader::Model plant_01_model("assets/models/task_models/plant.gltf", true);
    Loader::Model plant_02_model("assets/models/task_models/plant.gltf", true);
    Loader::Model tower_model("assets/models/task_models/tower.gltf", true);
    // Create proctors 
    GameLogic::Proctor palm_00          ("palm_00",          glm::vec3(-01.83f, 00.00f,  75.63f), glm::quat(1.0f, 0.0f,  00.00f, 0.0f), glm::vec3(10.0f));
    GameLogic::Proctor palm_01          ("palm_01",          glm::vec3(-46.29f, 00.00f,  75.96f), glm::quat(1.0f, 0.0f,  00.35f, 0.0f), glm::vec3(10.0f));
    GameLogic::Proctor palm_02          ("palm_02",          glm::vec3( 03.86f, 00.00f,  48.67f), glm::quat(1.0f, 0.0f,  00.72f, 0.0f), glm::vec3(10.0f));
    GameLogic::Proctor boat_small       ("boat_small",       glm::vec3( 21.48f, 00.00f,  57.05f), glm::quat(1.0f, 0.0f,  00.41f, 0.0f), glm::vec3(6.0f));
    GameLogic::Proctor chest            ("chest",            glm::vec3( 18.37f, 00.00f, -63.33f), glm::quat(1.0f, 0.0f,  02.34f, 0.0f), glm::vec3(6.0f));
    GameLogic::Proctor formationRock_00 ("formationRock_00", glm::vec3(-13.06f, 00.00f,  27.38f), glm::quat(1.0f, 0.0f,  00.25f, 0.0f), glm::vec3(6.0f));
    GameLogic::Proctor formationRock_01 ("formationRock_01", glm::vec3(-30.35f, 00.00f,  88.90f), glm::quat(1.0f, 0.0f,  00.25f, 0.0f), glm::vec3(6.0f));
    GameLogic::Proctor plant_00         ("plant_00",         glm::vec3( 06.66f, 00.00f, -19.76f), glm::quat(1.0f, 0.0f, -03.54f, 0.0f), glm::vec3(6.0f));
    GameLogic::Proctor plant_01         ("plant_01",         glm::vec3(-08.50f, 00.00f, -02.50f), glm::quat(1.0f, 0.0f, -01.28f, 0.0f), glm::vec3(6.0f));
    GameLogic::Proctor plant_02         ("plant_02",         glm::vec3( 17.73f, 00.00f,  10.50f), glm::quat(1.0f, 0.0f, -00.03f, 0.0f), glm::vec3(6.0f));
    GameLogic::Proctor tower            ("tower",            glm::vec3( 03.50f, 00.00f, -42.00f), glm::quat(1.0f, 0.0f, -03.50f, 0.0f), glm::vec3(6.0f));
    // Create MeshRenderer component
    GameLogic::MeshRenderer palm_00_mr(GameLogic::C_MESH, &palm_00, &palm_00_model, &model3D);
    GameLogic::MeshRenderer palm_01_mr(GameLogic::C_MESH, &palm_01, &palm_01_model, &model3D);
    GameLogic::MeshRenderer palm_02_mr(GameLogic::C_MESH, &palm_02, &palm_02_model, &model3D);
    GameLogic::MeshRenderer boat_small_mr(GameLogic::C_MESH, &boat_small, &boat_small_model, &model3D);
    GameLogic::MeshRenderer chest_mr(GameLogic::C_MESH, &chest, &chest_model, &model3D);
    GameLogic::MeshRenderer formationRock_00_mr(GameLogic::C_MESH, &formationRock_00, &formationRock_00_model, &model3D);
    GameLogic::MeshRenderer formationRock_01_mr(GameLogic::C_MESH, &formationRock_01, &formationRock_01_model, &model3D);
    GameLogic::MeshRenderer plant_00_mr(GameLogic::C_MESH, &plant_00, &plant_00_model, &model3D);
    GameLogic::MeshRenderer plant_01_mr(GameLogic::C_MESH, &plant_01, &plant_01_model, &model3D);
    GameLogic::MeshRenderer plant_02_mr(GameLogic::C_MESH, &plant_02, &plant_02_model, &model3D);
    GameLogic::MeshRenderer tower_mr(GameLogic::C_MESH, &tower, &tower_model, &model3D);
    // Add objects to hierarchy
    hierarchy.addObject(&palm_00);
    hierarchy.addObject(&palm_01);
    hierarchy.addObject(&palm_02);
    hierarchy.addObject(&boat_small);
    hierarchy.addObject(&chest);
    hierarchy.addObject(&formationRock_00);
    hierarchy.addObject(&formationRock_01);
    hierarchy.addObject(&plant_00);
    hierarchy.addObject(&plant_01);
    hierarchy.addObject(&plant_02);
    hierarchy.addObject(&tower);

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

        /* Use InputSystem to move camera */
        /* Use 'P' to switch between camera modes, CTRL to look around, 'R' to get cursor position*/
        cameraSwitch(35,60,90,45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01, yValueUp, yValueDown, xValueLeft, xValueRight);

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

        /* Render models */
        hierarchy.update();

    	// COLLISIONS BELOW
        /* Set up universal collisionBoxShader uniforms */
        collisionBoxShader.use();
        collisionBoxShader.setUniform("view", view);
        collisionBoxShader.setUniform("projection", projection);
    	
        /* Update colliders positions, calc collisions */
    	// box 00
        //someModelCollider_00.setScale(modelJakis_00_model.scale);
        //someModelCollider_00.setRotation(modelJakis_00_model.rotation);
        //someModelCollider_00.setPosition(modelJakis_00_model.position);

    	// box 01
        //someModelCollider_01.setScale(modelJakis_01_model.scale);
        //someModelCollider_01.setRotation(modelJakis_01_model.rotation);
        //someModelCollider_01.setPosition(modelJakis_01_model.position);

    	// hero 00
     //   heroCollider_00.setScale(hero_00_model.scale);
     //   heroCollider_00.setRotation(hero_00_model.rotation);
     //   heroCollider_00.setPosition(hero_00_model.position);

    	//// hero 01
     //   heroCollider_01.setScale(hero_01_model.scale);
     //   heroCollider_01.setRotation(hero_01_model.rotation);
     //   heroCollider_01.setPosition(hero_01_model.position);

        // DEBUG
        //std::cout << "==========================================" << std::endl;
        //std::cout << "HERO 00 VALUES AFTER UPDATE METHOD IN MAIN" << std::endl;
        //std::cout << "==========================================" << std::endl;
        //std::cout << "POSITION VECTOR: " << hero_00_model.position.x << " " << hero_00_model.position.y << " " << hero_00_model.position.z << std::endl;
        //std::cout << "ROTATION VECTOR: " << hero_00_model.rotation.x << " " << hero_00_model.rotation.y << " " << hero_00_model.rotation.z << std::endl;
        //std::cout << "SCALE VECTOR: " << hero_00_model.scale.x << " " << hero_00_model.scale.y << " " << hero_00_model.scale.z << std::endl << std::endl;

        //std::cout << "==========================================" << std::endl;
        //std::cout << "HERO 01 VALUES AFTER UPDATE METHOD IN MAIN" << std::endl;
        //std::cout << "==========================================" << std::endl;
        //std::cout << "POSITION VECTOR: " << hero_01_model.position.x << " " << hero_01_model.position.y << " " << hero_01_model.position.z << std::endl;
        //std::cout << "ROTATION VECTOR: " << hero_01_model.rotation.x << " " << hero_01_model.rotation.y << " " << hero_01_model.rotation.z << std::endl;
        //std::cout << "SCALE VECTOR: " << hero_01_model.scale.x << " " << hero_01_model.scale.y << " " << hero_01_model.scale.z << std::endl << std::endl;
    	
        /* Update uniforms and render colliders */
    	// box 00
        //collisionBoxShader.setUniform("model", 
        //    ((GameLogic::BoxCollider*) hero_00.getComponentOfType(GameLogic::C_COLLIDER))->getTranslateMatrix() *
        //    ((GameLogic::BoxCollider*)hero_00.getComponentOfType(GameLogic::C_COLLIDER))->getScaleMatrix());
        //collisionBoxShader.setUniform("radius",
        //    ((GameLogic::BoxCollider*)hero_00.getComponentOfType(GameLogic::C_COLLIDER))->getRadius() *
        //    hero_00.transform.scale);
        collisionBoxShader.setUniformBool("collision", checkAABBCollision(&hero_00, &hero_01));
        ((GameLogic::BoxCollider*)hero_00.getComponentOfType(GameLogic::C_COLLIDER))->render();
        if (checkAABBCollision(&hero_00, &hero_01)) separateAABBCollision(&hero_00, &hero_01);
        //checkAABBCollision(&hero_00, &hero_01);
    	
        // box 01
        //collisionBoxShader.setUniform("model", someModelCollider_01.getTranslateMatrix() * someModelCollider_01.getScaleMatrix());
        //collisionBoxShader.setUniform("radius", someModelCollider_01.getRadius() / someModelCollider_01.scale);
        //collisionBoxShader.setUniformBool("collision", checkAABBCollision(someModelCollider_00, someModelCollider_01));
        //someModelCollider_01.render();

        //// hero 00
        //collisionBoxShader.setUniform("model", heroCollider_00.getTranslateMatrix() * heroCollider_00.getScaleMatrix());
        //collisionBoxShader.setUniform("radius", heroCollider_00.getRadius() / heroCollider_00.scale);
        //collisionBoxShader.setUniformBool("collision", false);
        //heroCollider_00.render();
    	
        //// hero 01
        //collisionBoxShader.setUniform("model", heroCollider_01.getTranslateMatrix() * heroCollider_01.getScaleMatrix());
        //collisionBoxShader.setUniform("radius", heroCollider_01.getRadius() / heroCollider_01.scale);
        //collisionBoxShader.setUniformBool("collision", false);
        //heroCollider_01.render();

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

void cameraKeyboardInput(Application::Scene* _scene, InputSystem::KeyboardInput* _keyboard)
{
    if (_keyboard->isKeyDown(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(_scene->window, true);
    if (_keyboard->isKeyDown(GLFW_KEY_UP))
        camera.ProcessKeyboard(FORWARD, _scene->deltaTime);
    if (_keyboard->isKeyDown(GLFW_KEY_DOWN))
        camera.ProcessKeyboard(BACKWARD, _scene->deltaTime);
    if (_keyboard->isKeyDown(GLFW_KEY_LEFT))
        camera.ProcessKeyboard(LEFT, _scene->deltaTime);
    if (_keyboard->isKeyDown(GLFW_KEY_RIGHT))
        camera.ProcessKeyboard(RIGHT, _scene->deltaTime);
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

void cameraSwitch(int minZoom,int maxZoom,float maxDistanceX, float maxDistanceY, InputSystem::MouseInput* mouseInput, InputSystem::KeyboardInput* keyboardInput, Application::Scene* _scene, GameLogic::Proctor* player_1, GameLogic::Proctor* player_2,
    float& yValueUp, float& yValueDown, float& xValueLeft, float& xValueRight)
{
    if (keyboardInput->isKeyPressed(GLFW_KEY_P)) {
        isDebugModeOn = !isDebugModeOn;
    }

    if (isDebugModeOn)
    {
        cameraKeyboardInput(_scene, keyboardInput);
        mouseOusideWindowsPos(GLFW_KEY_R, keyboardInput, mouseInput);

        if (keyboardInput->isKeyDown(GLFW_KEY_RIGHT_CONTROL))
        {
            cameraMouseInput(_scene->window, mouseInput);
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

// COLLISION FUNCTIONS

bool checkAABBCollision(GameLogic::Proctor* _a, GameLogic::Proctor* _b)
{
    if (glm::abs(_a->transform.position.x - _b->transform.position.x) >
        (((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().x +
            ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().x))
    {
        return false;
    }
    if (glm::abs(_a->transform.position.y - _b->transform.position.y) >
        (((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().y +
            ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().y))
    {
        return false;
    }
    if (glm::abs(_a->transform.position.z - _b->transform.position.z) >
        (((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().z +
            ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().z))
    {
        return false;
    }
    std::cout << "COLLISION" << std::endl;
    return true;
}

void separateAABBCollision(GameLogic::Proctor* _a, GameLogic::Proctor* _b)
{
    /* Calculate separation direction */
    glm::vec3 separationDirection;
    if (_a->transform.position.x >= _b->transform.position.x) separationDirection.x = 1.0f;
    else separationDirection.x = -1.0f;
    if (_a->transform.position.y >= _b->transform.position.y) separationDirection.y = 1.0f;
    else separationDirection.y = -1.0f;
    if (_a->transform.position.z >= _b->transform.position.z) separationDirection.z = 1.0f;
    else separationDirection.z = -1.0f;
	
	/* Calculate distance between objects positions */
    glm::vec3 distanceAB;
    distanceAB.x = glm::abs(_a->transform.position.x - _b->transform.position.x);
    distanceAB.y = glm::abs(_a->transform.position.y - _b->transform.position.y);
    distanceAB.z = glm::abs(_a->transform.position.z - _b->transform.position.z);
	
    /* Calculate the sum of radians of the both objects */
    glm::vec3 radiusABSum;
    radiusABSum.x = ((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().x + ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().x;
    radiusABSum.y = ((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().y + ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().y;
    radiusABSum.z = ((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().z + ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().z;

    /* Calculate separation vector */
    glm::vec3 separationVector;
    separationVector.x = (radiusABSum.x - distanceAB.x)*separationDirection.x;
    separationVector.y = (radiusABSum.y - distanceAB.y)*separationDirection.y;
    separationVector.z = (radiusABSum.z - distanceAB.z)*separationDirection.z;

    /* Add separation vector to the objects */
    _a->transform.position.x += separationVector.x;
    _a->transform.position.y += separationVector.y;
    _a->transform.position.z += separationVector.z;

}