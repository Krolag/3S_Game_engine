#include <windows.h>
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
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
#include "Loader/Importer.h"
#include "Water/WaterMesh.h"
#include "Framebuffer/Framebuffer.h"
#include <Monster/Monster.h>

#include "Loader/Exporter.h"

#include "AnimationSystem/System/Animation.h"
#include "AnimationSystem/System/Animator.h"

//#include "AnimationSystem/AnimationSystem.h"

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
#pragma region Scene init
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
#pragma endregion

#pragma region ImGUI init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(mainScene.window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
    ImGui::StyleColorsDark();
    int hierarchyCurrentItem = 0;
#pragma endregion

#pragma region Shaders and shader variables
    glm::mat4 projection;
    glm::mat4 textProjection;
    glm::mat4 view;
    glm::mat4 model;

    /* Create shaders */
    Shader model3D("assets/shaders/model3D.vert", "assets/shaders/model3D.frag");
    Shader textShader("assets/shaders/text.vert", "assets/shaders/text.frag");
    Shader collisionBoxShader("assets/shaders/boxCollider2.vert", "assets/shaders/boxCollider2.frag");
#pragma endregion

#pragma region UI init
    /* Text init */
    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH), 0.0f, static_cast<GLfloat>(SCREEN_WIDTH));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "textProjection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    /* Create text */
    UIRender::TextRender points("assets/shaders/text.vert", "assets/shaders/text.frag", "assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);

    /* Animated mario */
    // TODO: @Ignacy zmien to na monete gdy bedzie gotowa
    UIRender::UIElement marioWalking[4] = {
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_00.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_01.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_00.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_03.png", 0.01, 0.045, 0.97, 0.91),
    };

    int marioWalkingIndex = 0;
    float timeBetweenFrames = 0.15f;
#pragma endregion

#pragma region Proctors init
    /* Create object hierarchy */
    GameLogic::Hierarchy hierarchy(&mainScene);
    hierarchy.setCamera(&camera);

    /* Create models library */
    Loader::ModelLibrary modelLibrary;

    /* Create importer with given *.xml file */
    Loader::Importer importer("assets/scenes/scene.xml", &model3D, 10.0f);

    /* Load models to hierarchy */ // UNCOMMENT TO ADD IMPORTED OBJECTS TO HIERARCHY
	for (int i = 0; i < importer.importedProctors.size(); ++i)
	{
	   importer.meshRenderers.push_back(std::make_shared<GameLogic::MeshRenderer>(GameLogic::C_MESH, importer.importedProctors.at(i).get(), importer.importedModelLibrary.getModel(importer.prepareModelName(importer.importedProctors.at(i).get()->name)), &model3D));
	   hierarchy.addObject(importer.importedProctors.at(i).get());
	}


    /* Load models that probably won't be serialized */
    modelLibrary.addModel("assets/models/hero/player_concept.fbx", "hero_00", true);
    modelLibrary.addModel("assets/models/hero/player_concept.fbx", "hero_01", true);
    modelLibrary.addModel("assets/models/serializable/chestBody.fbx", "chestBody_00", true);
    modelLibrary.addModel("assets/models/serializable/chestBody.fbx", "chestBody_01", true);
    modelLibrary.addModel("assets/models/coin.fbx", "coin_00", true);
    modelLibrary.addModel("assets/models/coin.fbx", "coin_01", true);
    //modelLibrary.addModel("assets/models/serializable/island_corner_00.fbx", "island_00", true);

    /* Configure proctors */
    // Players1
    GameLogic::Proctor      hero_00("hero_00", glm::vec3(2.0f, 2.5f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
    GameLogic::MeshRenderer hero_00_mr(GameLogic::C_MESH, &hero_00, modelLibrary.getModel("hero_00"), &model3D);
    GameLogic::PlayerInput  hero_00_pi(GameLogic::C_MOVEMENT, &hero_00, true);
    GameLogic::BoxCollider  hero_00_bc(GameLogic::C_COLLIDER, modelLibrary.getModel("hero_00"), &hero_00, &collisionBoxShader, false);
    hierarchy.addObject(&hero_00);
    GameLogic::Proctor      hero_01("hero_01", glm::vec3(-2.0f, 2.5f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
    GameLogic::MeshRenderer hero_01_mr(GameLogic::C_MESH, &hero_01, modelLibrary.getModel(hero_01.name), &model3D);
    GameLogic::PlayerInput  hero_01_pi(GameLogic::C_MOVEMENT, &hero_01, false);
    GameLogic::BoxCollider  hero_01_bc(GameLogic::C_COLLIDER, modelLibrary.getModel(hero_01.name), &hero_01, &collisionBoxShader, false);
    hierarchy.addObject(&hero_01);

    // Chests and coins
    GameLogic::Proctor      chestBody_00("chestBody_00", glm::vec3(0.00f, 2.97f, -05.00f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.03f));
    GameLogic::MeshRenderer chestBody_00_mr(GameLogic::C_MESH, &chestBody_00, modelLibrary.getModel(chestBody_00.name), &model3D);
    GameLogic::Interactable chestBody_00_ible(GameLogic::C_INTERACTABLE, &chestBody_00);
    GameLogic::Treasure     chestBody_00_tre(GameLogic::C_TREASURE, &chestBody_00);
    //GameLogic::BoxCollider  chestBody_00_bc(GameLogic::C_COLLIDER, modelLibrary.getModel(chestBody_00.name), &chestBody_00, &collisionBoxShader);
    hierarchy.addObject(&chestBody_00);
    GameLogic::Proctor      chestBody2("chestBody_01", glm::vec3(0.00f, 2.97f, 010.00f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.03f));
    GameLogic::MeshRenderer chestBody2_mr(GameLogic::C_MESH, &chestBody2, modelLibrary.getModel(chestBody2.name), &model3D);
    GameLogic::Interactable chestBody2_ible(GameLogic::C_INTERACTABLE, &chestBody2);
    GameLogic::Treasure     chestBody2_tre(GameLogic::C_TREASURE, &chestBody2);
    //GameLogic::BoxCollider  chestBody2_bc(GameLogic::C_COLLIDER, modelLibrary.getModel(chestBody2.name), &chestBody2, &collisionBoxShader);
    hierarchy.addObject(&chestBody2);
    GameLogic::Proctor      coin_00("coin_00", glm::vec3(0.00f, 4.97f, -05.00f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(.003f));
    GameLogic::MeshRenderer coin_00_mr(GameLogic::C_MESH, &coin_00, modelLibrary.getModel(coin_00.name), &model3D);
    GameLogic::Cash         coin_00_csh(GameLogic::C_CASH, &coin_00, &hero_00, &hero_01);
    hierarchy.addObject(&coin_00);
    GameLogic::Proctor      coin_01("coin_01", glm::vec3(0.00f, 2.97f, 010.00f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(.003f));
    GameLogic::MeshRenderer coin_01_mr(GameLogic::C_MESH, &coin_01, modelLibrary.getModel(coin_00.name), &model3D);
    GameLogic::Cash         coin_01_csh(GameLogic::C_CASH, &coin_01, &hero_00, &hero_01);
    hierarchy.addObject(&coin_01);
  /*  GameLogic::Proctor      island_00("island_00", glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
    GameLogic::MeshRenderer island_00_mr(GameLogic::C_MESH, &island_00, modelLibrary.getModel("island_00"), &model3D);
    hierarchy.addObject(&island_00);
    GameLogic::Proctor      island_01("island_01", glm::vec3(20.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, -1.57f, 0.0f), glm::vec3(0.05f));
    GameLogic::MeshRenderer island_01_mr(GameLogic::C_MESH, &island_01, modelLibrary.getModel("island_00"), &model3D);
    hierarchy.addObject(&island_01);*/
#pragma endregion

#pragma region Environment
    /* Create Skybox */
    Skybox skybox(&view, &projection, &camera);

    /* Lights */
    DirLight dirLight = {
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
        glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
        glm::vec4(0.75f, 0.75f, 0.75f, 1.0f)
    };

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    WaterMesh water("assets/shaders/water.vert", "assets/shaders/water.frag", "assets/shaders/water.geom", 1000, 2000);
    int waterYpos = 0;
    Framebuffer reflectFramebuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    Framebuffer refractFramebuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
#pragma endregion

    /* Create InputSystem elements */
    InputSystem::MouseInput* mouseInput = new InputSystem::MouseInput(mainScene.window);
    InputSystem::KeyboardInput* keyboardInput = new InputSystem::KeyboardInput(mainScene.window);

    float xValueRight = 0.2;
    float xValueLeft = 0.2;
    float yValueUp = 0.2;
    float yValueDown = 0.2;

    std::vector<GameLogic::Proctor*> tiles;
    //tiles.push_back(&island_00);
    //tiles.push_back(&island_01);
    Monster monsterSystem(&hero_00, tiles);

    //Loader::Model player_one("assets/models/vampire.dae", "red_run", true);
    GameLogic::Proctor test_anim("test_anim", glm::vec3(0.0f), glm::quat(1.0f, glm::radians(-90.0f), 0.0f, 0.0f), glm::vec3(0.01f));
    Loader::Model player_one("assets/models/uploads_files_600310_skeleton_animated.FBX", "red_run", true);
    Animation movement(player_one.path.c_str(), &player_one);
    Animator animator(&movement);
    animator.playAnimation(&movement);
    GameLogic::MeshRenderer test_anim_mr(GameLogic::C_MESH, &test_anim, &player_one, &model3D);
    //Animation movement(test_anim_mr.model->path, test_anim_mr.model);
    //animator.playAnimation(&movement);
    hierarchy.addObject(&test_anim);
   
    /* Render loop */
    while (!glfwWindowShouldClose(mainScene.window))
    {
        /* Dear ImGUI new frame setup */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        camera.setProjection(projection);
        FrustumCulling::createViewFrustumFromMatrix(&camera);

        //enable cliping
        glEnable(GL_CLIP_DISTANCE0);

        //HERE STARTS RENDERING MODELS BENEATH WATER TO BUFFER (REFLECTFRAMEBUFER)
        reflectFramebuffer.bindFramebuffer();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set camera underwater - to get reflection
        float distance = 2 * (camera.Position.y - waterYpos);
        camera.Position.y -= distance;
        camera.Pitch = -camera.Pitch;
        camera.updateCameraVectors();
    	
        model3D.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
        model3D.setUniform("projection", projection);
        view = camera.GetViewMatrix();
        model3D.setUniform("view", view);
        model3D.setUniform("plane", glm::vec4(0, 1, 0, -waterYpos)); // cliping everything under water plane
        model = glm::mat4(1.0f);

        dirLight.render(model3D);
        hierarchy.update(true, false);
        skybox.render(); // render skybox only for reflectionbuffer

        //set camera position to default
        camera.Position.y += distance;
        camera.Pitch = -camera.Pitch;
        camera.updateCameraVectors();

        //close reflectframebuffer
        reflectFramebuffer.unbindFramebuffer();

        //HERE STARTS RENDERING MODELS UNDER THE WATER SURFACE (REFRACTION)
        refractFramebuffer.bindFramebuffer();
      
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model3D.use();
        model3D.setUniform("projection", projection);
        view = camera.GetViewMatrix();
        model3D.setUniform("view", view);
        model3D.setUniform("plane", glm::vec4(0, -1, 0, waterYpos));

        dirLight.render(model3D);   	   	
        hierarchy.update(true, false);

        hero_00_bc.checkCollisionOBB(&hero_01_bc);
    	
        //UNBIND REFRACT FRAMEBUFFER AND TURN OFF CLIPING
        refractFramebuffer.unbindFramebuffer();
        glDisable(GL_CLIP_DISTANCE0); 

        //---------------------------------------------------------------------------------------------------------
        //HERE STARTS DEFAULT RENDERING
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        model3D.use();
        model3D.setUniform("projection", projection);
        view = camera.GetViewMatrix();
        model3D.setUniform("view", view);

        auto transforms = animator.getFinalBoneMatrices();
        model3D.use();
        for (int i = 0; i < transforms.size(); ++i)
        {
            model3D.setUniform("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }

        //for (int i = 0; i < transforms.size(); ++i)
        //{
        //    model3D.setUniform("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        //    if (i <= 30)
        //    {
        //        std::cout << "BONE NR: '\s'  " << i << std::endl;
        //        std::cout << glm::to_string(transforms[i]).c_str() << std::endl;
        //    }
        //}

        cameraSwitch(35, 60, 90, 45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01, yValueUp, yValueDown, xValueLeft, xValueRight);

        dirLight.render(model3D);

        collisionBoxShader.use();
        collisionBoxShader.setUniform("projection", projection);
        collisionBoxShader.setUniform("view", view);
        collisionBoxShader.setUniformBool("collision", true);

        animator.updateAniamtion(mainScene.deltaTime);
        hierarchy.update(false, true); // need to be set this way, otherwise debug window won't appear

        model = glm::translate(model, glm::vec3(0, waterYpos, 0));
        water.render(model, projection, view, reflectFramebuffer.getTexture(), refractFramebuffer.getTexture(), mainScene.deltaTime, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));

        /* Render text */
        points.render(std::to_string(Points::getInstance()->getScore()), 60, 660, 1, glm::vec3(1.0, 0.75, 0.0));
        
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

        monsterSystem.update();
        
        /* Update InputSystem */
        keyboardInput->update();
        mouseInput->update();

        /* DEBUG - Draw DearImGUI */
        // THIS SHOULD BE THE LAST, DO NOT TOUCH IT
        ImGui::Render();

        /* Clear frame at the end of the loop */
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(mainScene.window);
        glfwPollEvents();
    }

	/* Export scene to xml file */
    //std::cout << "Save scene? [y/n]" << std::endl;
    //std::string check;
    //std::cin >> check;
    //if (check == "y")
    //    Loader::Exporter::exportScene(hierarchy.getProctors());

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    hierarchy.cleanup();

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