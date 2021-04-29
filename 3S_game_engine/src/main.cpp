#include <windows.h>
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

/* Load 3SE packages */
#include "Application/Application.h"
#include "Loader/Loader.h"
#include "InputSystem/InputSystem.h"
#include "GameLogic/GameLogic.h"
#include "UIRender/UIRender.h"

#include "Shader/Shader.h"
#include "Skybox/Skybox.h"
#include "Camera/Camera.h"
#include "Light/Light.h"
#include "Points/Points.h"
#include "Camera/FrustumCulling.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cameraMouseInput(GLFWwindow* window, InputSystem::MouseInput* mouse);
void cameraKeyboardInput(Application::Scene* _scene, InputSystem::KeyboardInput* _keyboard);
void mouseOusideWindowsPos(int key, InputSystem::KeyboardInput* keyboard, InputSystem::MouseInput* mouse);

//Switch camera position(debug)
//maxDistanceY/maxDistanceX = ScreenHeight/ScreenWidth
void cameraSwitch(int minZoom, int maxZoom, float maxDistanceX, float maxDistanceY, InputSystem::MouseInput* mouseInput, InputSystem::KeyboardInput* keyboardInput, 
    Application::Scene* _scene, GameLogic::Proctor* player_1, GameLogic::Proctor* player_2,
    float& yValueUp, float& yValueDown, float& xValueLeft, float& xValueRight);

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

    /* Create text */
    UIRender::TextRender points("assets/shaders/text.vert", "assets/shaders/text.frag", "assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);
    
    /* Create points */
    Points score(0);

    /* Create InputSystem elements */
    InputSystem::MouseInput* mouseInput = new InputSystem::MouseInput(mainScene.window);
    InputSystem::KeyboardInput* keyboardInput = new InputSystem::KeyboardInput(mainScene.window);

    /* Create object hierarchy */
    GameLogic::Hierarchy hierarchy(&mainScene);

    /* Load models */
    clock_t begin_time = clock(); // Calculate time for loading models
    // Create model library
    Loader::ModelLibrary modelLibrary;
    // PLAYERS
    modelLibrary.addModel("assets/models/hero/pirat_po_obrocie_na_plecki.fbx",  "hero_00",  true);
    modelLibrary.addModel("assets/models/hero/pirat_po_obrocie_na_plecki.fbx",  "hero_01",  true);
    // ENVIR
    modelLibrary.addModel("assets/models/cube/untitled.obj",                    "ground",   true);
    modelLibrary.addModel("assets/models/task_models/palm_detailed_short.gltf", "palm_00",  true);
    modelLibrary.addModel("assets/models/task_models/palm_detailed_short.gltf", "palm_01",  true);
    modelLibrary.addModel("assets/models/task_models/palm_long.gltf",           "palm_02",  true);
    modelLibrary.addModel("assets/models/task_models/palm_short.gltf",          "palm_03",  true);
    modelLibrary.addModel("assets/models/task_models/boat_small.gltf",          "boat",     true);
    modelLibrary.addModel("assets/models/task_models/chest.gltf",               "chest",    true);
    modelLibrary.addModel("assets/models/task_models/formation_rock.gltf",      "rocks_00", true);
    modelLibrary.addModel("assets/models/task_models/formation_rock.gltf",      "rocks_01", true);
    modelLibrary.addModel("assets/models/task_models/plant.gltf",               "plant_00", true);
    modelLibrary.addModel("assets/models/task_models/plant.gltf",               "plant_01", true);
    modelLibrary.addModel("assets/models/task_models/plant.gltf",               "plant_02", true);
    modelLibrary.addModel("assets/models/task_models/tower.gltf",               "tower",    true);
    
    std::cout << "models loaded in:   " << float(clock() - begin_time) / CLOCKS_PER_SEC << "\t seconds" << std::endl;

    /* Configure proctors */
    begin_time = clock(); // Calculate time for creating proctors 
    GameLogic::Proctor      hero_00("hero_00", glm::vec3(2.0f, 2.5f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
    GameLogic::MeshRenderer hero_00_mr(GameLogic::C_MESH, &hero_00, modelLibrary.getModel("hero_00"), &model3D);
    GameLogic::PlayerInput  hero_00_pi(GameLogic::C_MOVEMENT, &hero_00, true);
    GameLogic::BoxCollider  hero_00_bc(GameLogic::C_COLLIDER, modelLibrary.getModel("hero_00"), &hero_00, &collisionBoxShader, false);
    hierarchy.addObject(&hero_00);
    GameLogic::Proctor      hero_01("hero_01", glm::vec3(-2.0f, 2.5f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
    GameLogic::MeshRenderer hero_01_mr(GameLogic::C_MESH, &hero_01, modelLibrary.getModel(hero_01.name), &model3D);
    GameLogic::PlayerInput  hero_01_pi(GameLogic::C_MOVEMENT, &hero_01, false);
    GameLogic::BoxCollider  hero_01_bc(GameLogic::C_COLLIDER, modelLibrary.getModel(hero_01.name), &hero_01, &collisionBoxShader);
    hierarchy.addObject(&hero_01);
    GameLogic::Proctor      ground("ground", glm::vec3(0.0f, -1.0f, 0.0f), glm::quat(1.0f, glm::vec3(0.0f)), glm::vec3(50.0f, 1.0f, 50.0f));
    GameLogic::MeshRenderer ground_mr(GameLogic::C_MESH, &ground, modelLibrary.getModel(ground.name), &model3D);
    hierarchy.addObject(&ground);
    GameLogic::Proctor      palm_00("palm_00", glm::vec3(-01.83f, 00.00f, 75.63f), glm::quat(1.0f, 0.0f, 00.00f, 0.0f), glm::vec3(10.0f));
    GameLogic::MeshRenderer palm_00_mr(GameLogic::C_MESH, &palm_00, modelLibrary.getModel(palm_00.name), &model3D);
    hierarchy.addObject(&palm_00);
    GameLogic::Proctor      palm_01("palm_01", glm::vec3(-6.29f, 00.00f, 75.96f), glm::quat(1.0f, 0.0f, 00.35f, 0.0f), glm::vec3(10.0f));
    GameLogic::MeshRenderer palm_01_mr(GameLogic::C_MESH, &palm_01, modelLibrary.getModel(palm_01.name), &model3D);
    hierarchy.addObject(&palm_01);
    GameLogic::Proctor      palm_02("palm_02", glm::vec3( 03.86f, 00.00f, 48.67f), glm::quat(1.0f, 0.0f, 00.72f, 0.0f), glm::vec3(10.0f));
    GameLogic::MeshRenderer palm_02_mr(GameLogic::C_MESH, &palm_02, modelLibrary.getModel(palm_02.name), &model3D);
    hierarchy.addObject(&palm_02);
    GameLogic::Proctor      palm_03("palm_03", glm::vec3(-03.86f, 00.00f, 48.67f), glm::quat(1.0f, 0.0f, 00.72f, 0.0f), glm::vec3(10.0f));
    GameLogic::MeshRenderer palm_03_mr(GameLogic::C_MESH, &palm_03, modelLibrary.getModel(palm_03.name), &model3D);
    hierarchy.addObject(&palm_03);
    GameLogic::Proctor      boat("boat", glm::vec3(21.48f, 00.00f, 57.05f), glm::quat(1.0f, 0.0f, 00.41f, 0.0f), glm::vec3(6.0f));
    GameLogic::MeshRenderer boat_mr(GameLogic::C_MESH, &boat, modelLibrary.getModel(boat.name), &model3D);
    hierarchy.addObject(&boat);
    GameLogic::Proctor      chest("chest", glm::vec3(18.37f, 00.00f, -63.33f), glm::quat(1.0f, 0.0f, 02.34f, 0.0f), glm::vec3(6.0f));
    GameLogic::MeshRenderer chest_mr(GameLogic::C_MESH, &chest, modelLibrary.getModel(chest.name), &model3D);
    GameLogic::Interactable chest_ible(GameLogic::C_INTERACTABLE, &chest); 
    hierarchy.addObject(&chest);
    GameLogic::Proctor      rocks_00("rocks_00", glm::vec3(-13.06f, 00.00f, 27.38f), glm::quat(1.0f, 0.0f, 00.25f, 0.0f), glm::vec3(6.0f));
    GameLogic::MeshRenderer rocks_00_mr(GameLogic::C_MESH, &rocks_00, modelLibrary.getModel(rocks_00.name), &model3D);
    hierarchy.addObject(&rocks_00);
    GameLogic::Proctor      rocks_01("rocks_01", glm::vec3(-30.35f, 00.00f, 88.90f), glm::quat(1.0f, 0.0f, 00.25f, 0.0f), glm::vec3(6.0f));
    GameLogic::MeshRenderer rocks_01_mr(GameLogic::C_MESH, &rocks_01, modelLibrary.getModel(rocks_01.name), &model3D);
    hierarchy.addObject(&rocks_01);
    GameLogic::Proctor      plant_00("plant_00", glm::vec3(06.66f, 00.00f, -19.76f), glm::quat(1.0f, 0.0f, -03.54f, 0.0f), glm::vec3(6.0f));
    GameLogic::MeshRenderer plant_00_mr(GameLogic::C_MESH, &plant_00, modelLibrary.getModel(plant_00.name), &model3D);
    hierarchy.addObject(&plant_00);
    GameLogic::Proctor      plant_01("plant_01", glm::vec3(-08.50f, 00.00f, -02.50f), glm::quat(1.0f, 0.0f, -01.28f, 0.0f), glm::vec3(6.0f));
    GameLogic::MeshRenderer plant_01_mr(GameLogic::C_MESH, &plant_01, modelLibrary.getModel(plant_01.name), &model3D);
    hierarchy.addObject(&plant_01);
    GameLogic::Proctor      plant_02("plant_02", glm::vec3(17.73f, 00.00f, 10.50f), glm::quat(1.0f, 0.0f, -00.03f, 0.0f), glm::vec3(6.0f));
    GameLogic::MeshRenderer plant_02_mr(GameLogic::C_MESH, &plant_02, modelLibrary.getModel(plant_02.name), &model3D);
    hierarchy.addObject(&plant_02);
    GameLogic::Proctor      tower("tower", glm::vec3(03.50f, 00.00f, -42.00f), glm::quat(1.0f, 0.0f, -03.50f, 0.0f), glm::vec3(6.0f));
    GameLogic::MeshRenderer tower_mr(GameLogic::C_MESH, &tower, modelLibrary.getModel(tower.name), &model3D);
    GameLogic::BoxCollider  tower_bc(GameLogic::C_COLLIDER, modelLibrary.getModel(tower.name), &tower, &collisionBoxShader);
    hierarchy.addObject(&tower);
    std::cout << "proctors created in:    " << float(clock() - begin_time) / CLOCKS_PER_SEC << "\t seconds" << std::endl;
    hierarchy.setCamera(&camera);

    /* Lights */
    DirLight dirLight = {
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
        glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
        glm::vec4(0.75f, 0.75f, 0.75f, 1.0f)
    };
    
     /* Animated mario */
    UIRender::UIElement marioWalking[4] = {
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_00.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_01.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_00.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_03.png", 0.01, 0.045, 0.97, 0.91),
    };

    int marioWalkingIndex = 0;
    float timeBetweenFrames = 0.15f;

    float xValueRight = 0.2;
    float xValueLeft = 0.2;
    float yValueUp = 0.2;
    float yValueDown = 0.2;

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
        
        camera.setProjection(projection);   
        FrustumCulling::createViewFrustumFromMatrix(&camera);



        //camera.createViewFrustumFromMatrix(&camera);


    	/* Render lights */
        dirLight.render(model3D);

        /* Render models */
        hierarchy.update();

    	// COLLISIONS BELOW
        /* Set up universal collisionBoxShader uniforms */
        collisionBoxShader.use();
        collisionBoxShader.setUniform("view", view);
        collisionBoxShader.setUniform("projection", projection);

        /* Sky-box -- Must be rendered almost last, before hud */
        skybox.render();

        /* DEBUG - Draw DearImGUI */
        ImGui::Render();

        /* Render text */
        points.render(std::to_string(score.getScore()), 60, 660, 1, glm::vec3(1.0, 0.75, 0.0));

        /* Render cute Mario as placeholder to coin */
        marioWalking[marioWalkingIndex].render();
        timeBetweenFrames -= mainScene.deltaTime;
        if (timeBetweenFrames <= 0)
        {
            marioWalkingIndex++;
            timeBetweenFrames = 0.15f;
        }
        if (marioWalkingIndex >= 4)
            marioWalkingIndex = 0;
        
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