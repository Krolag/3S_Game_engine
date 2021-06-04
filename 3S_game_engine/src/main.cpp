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
#include <IrrKlang/irrKlang.h>

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
#include "Monster/Monster.h"
#include "Boat/Boat.h"
#include "Loader/Exporter.h"

using namespace irrklang;
#pragma region Audio
//SoundEngine
ISoundEngine* engine = createIrrKlangDevice();
// music
ISoundSource* musicSource = engine->addSoundSourceFromFile("assets/audio/music/shanty.ogg");
// waves
ISoundSource* waveSource = engine->addSoundSourceFromFile("assets/audio/sounds/background.ogg");
// menu
ISoundSource* bottleSource = engine->addSoundSourceFromFile("assets/audio/sounds/bottle.ogg");
ISoundSource* mainMenuSource = engine->addSoundSourceFromFile("assets/audio/sounds/mainMenu.ogg");
#pragma endregion

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cameraMouseInput(GLFWwindow* window, InputSystem::MouseInput* mouse);
void cameraKeyboardInput(Application::Window* _scene, InputSystem::KeyboardInput* _keyboard);
void mouseOusideWindowsPos(int key, InputSystem::KeyboardInput* keyboard, InputSystem::MouseInput* mouse);

//Switch camera position(debug)
//maxDistanceY/maxDistanceX = ScreenHeight/ScreenWidth
void cameraSwitch(int minZoom, int maxZoom, float maxDistanceX, float maxDistanceY, InputSystem::MouseInput* mouseInput, InputSystem::KeyboardInput* keyboardInput, 
    Application::Window* _scene, GameLogic::Proctor* player_1, GameLogic::Proctor* player_2,
    float& yValueUp, float& yValueDown, float& xValueLeft, float& xValueRight);

// settings
const unsigned int SCREEN_WIDTH = 1920;
const unsigned int SCREEN_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

bool isDebugModeOn = false;
bool isPaused = false;
int main()
{
#pragma region Scene init
    /* Load scene */
    Application::Window mainScene("3S GameEngine", SCREEN_WIDTH, SCREEN_HEIGHT, false); // false - window, true - fullscreen 
    glfwMakeContextCurrent(mainScene.window);
    glfwSetFramebufferSizeCallback(mainScene.window, framebuffer_size_callback);
    glfwSetInputMode(mainScene.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Load all OpenGL function pointers */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    int collisionIncrement = 1;
    int frameCounter = 0;
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
#pragma region MainMenu
    UIRender::UIElement logo("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures", "logo.png", 0.35, 0.65, 0.97, 0.76);
    UIRender::UIElement startNotPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "notPressed.png", 0.4, 0.6, 0.72, 0.53);
    UIRender::UIElement startPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "pressed.png", 0.4, 0.6, 0.72, 0.53);
    UIRender::UIElement optionsNotPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "notPressed.png", 0.4, 0.6, 0.52, 0.33);
    UIRender::UIElement optionsPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "pressed.png", 0.4, 0.6, 0.52, 0.33);
    UIRender::UIElement exitNotPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "notPressed.png", 0.4, 0.6, 0.32, 0.13);
    UIRender::UIElement exitPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "pressed.png", 0.4, 0.6, 0.32, 0.13);
#pragma endregion

#pragma region StoryScene
    UIRender::UIElement story_00("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/story", "story_00.png", 0, 1, 1, 0);
    UIRender::UIElement story_01("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/story", "story_01.png", 0, 1, 1, 0);
#pragma endregion

#pragma region Game
    /* Text init */
    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH), 0.0f, static_cast<GLfloat>(SCREEN_WIDTH));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "textProjection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    /* Create text */
    UIRender::TextRender points("assets/shaders/text.vert", "assets/shaders/text.frag", "assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);

    /* Animated mario */
    UIRender::UIElement dukatSpinning[8] = {
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/dukat", "dukat_00.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/dukat", "dukat_01.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/dukat", "dukat_02.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/dukat", "dukat_03.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/dukat", "dukat_04.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/dukat", "dukat_05.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/dukat", "dukat_06.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/dukat", "dukat_07.png", 0.01, 0.045, 0.97, 0.91),
    };

    int dukatSpinIndex = 0;
    float timeBetweenFrames = 0.10f;
#pragma endregion
#pragma endregion

#pragma region Proctors init
    /* Create object hierarchy */
    GameLogic::Hierarchy hierarchy(&mainScene);
    hierarchy.setCamera(&camera);

    /* Create models library */
    Loader::ModelLibrary modelLibrary;
    /* Create importer with given *.xml file */
     //Loader::Importer importer("assets/scenes/rotation_scale_fix/scene.xml", &model3D, 100.0f);
     Loader::Importer importer("assets/scenes/scene.xml", &model3D, 10.0f);
    
    /* Load models to hierarchy */ // UNCOMMENT TO ADD IMPORTED OBJECTS TO HIERARCHY
    int size = importer.importedProctors.size();
    for (int i = 0; i < size; ++i)
    {
        importer.meshRenderers.push_back(std::make_shared<GameLogic::MeshRenderer>(
            GameLogic::C_MESH,
            importer.importedProctors.at(i).get(),
            importer.importedModelLibrary.getModel(importer.prepareModelName(importer.importedProctors.at(i).get()->name)),
            &model3D
            ));

        std::vector<bool> tmpCompoBooleanValues = importer.componetsBooleanValues[i];
         
        /* Check which components needs to be added */
        // BoxCollider
        if (tmpCompoBooleanValues[0])
        {
            importer.boxColliders.push_back(std::make_shared<GameLogic::BoxCollider>(
                GameLogic::C_COLLIDER,
                importer.importedModelLibrary.getModel(importer.prepareModelName(importer.importedProctors.at(i).get()->name)),
                importer.importedProctors.at(i).get(),
                &collisionBoxShader,
                tmpCompoBooleanValues[1]
                ));
        }
        // Interactables
        if (tmpCompoBooleanValues[2])
        {
            importer.interactables.push_back(std::make_shared<GameLogic::Interactable>(
                GameLogic::C_INTERACTABLE,
                importer.importedProctors.at(i).get()
                ));
        }
        // Treasures
        if (tmpCompoBooleanValues[3])
        {
            importer.treasures.push_back(std::make_shared<GameLogic::Treasure>(
                GameLogic::C_TREASURE,
                importer.importedProctors.at(i).get()
                ));
        }
        // Cash
        if (tmpCompoBooleanValues[4])
        {
            importer.cash.push_back(std::make_shared<GameLogic::Cash>(
                GameLogic::C_CASH,
                importer.importedProctors.at(i).get()
                ));
        }
        hierarchy.addObject(importer.importedProctors.at(i).get());
    }

    /* Load models that probably won't be serialized */
    //modelLibrary.addModel("assets/models/players/player_one.fbx", "hero_00", true, false); // If last value is set to true, there is no animation
    modelLibrary.addModel("assets/models/player_2021_05_30_23_21.fbx", "hero_01", true, true);
    modelLibrary.addModel("assets/models/serializable/chestBody_debug.fbx", "chestBody_00", true, true);
    modelLibrary.addModel("assets/models/serializable/chestBody_debug.fbx", "chestBody_01", true, true);
    modelLibrary.addModel("assets/models/coin.fbx", "coin_00", true, true);
    modelLibrary.addModel("assets/models/coin.fbx", "coin_01", true, true);
    modelLibrary.addModel("assets/models/boat/bbot.fbx", "boat", true, true);
    //modelLibrary.addModel("assets/models/serializable/island_corner_00.fbx", "island_00", true);

    /* Configure proctors */
    /* Boat */
    GameLogic::Proctor      boat("boat", glm::vec3(720.0f, 1.8f, 802.0f), glm::quat(1.0f, 0.0f, 1.6f, 0.0f), glm::vec3(0.09f));
    GameLogic::MeshRenderer boat_mr(GameLogic::C_MESH, &boat, modelLibrary.getModel(boat.name), &model3D);
    GameLogic::Boat boat_b(GameLogic::C_MOVEMENT, &boat);
    GameLogic::Interactable boat_inter(GameLogic::C_INTERACTABLE, &boat);
    GameLogic::BoxCollider  boat_bc(GameLogic::C_COLLIDER, modelLibrary.getModel(boat.name), &boat, &collisionBoxShader, false);

    // Players
    //GameLogic::Proctor      hero_00("hero_00", glm::vec3(-5.0f, 500.0f, 0.0f), glm::quat(1.0f, glm::radians(90.0f), 0.0f, 0.0f), glm::vec3(0.03f));
    Loader::Model           hero_00_model("assets/models/player_2021_05_30_23_21.fbx", "playerOne", true, false);
    GameLogic::Proctor      hero_00("playerOne", glm::vec3(770.0f, 5.0f, 850.0f), glm::quat(1.0f, glm::radians(90.0f), 0.0f, 0.0f), glm::vec3(0.03f));
    GameLogic::MeshRenderer hero_00_mr(GameLogic::C_MESH, &hero_00, &hero_00_model, &model3D);
    GameLogic::Anima        hero_00_an(GameLogic::C_ANIMA, &hero_00);
    GameLogic::PlayerInput  hero_00_pi(GameLogic::C_MOVEMENT, &hero_00, true, &boat_b);
    GameLogic::BoxCollider  hero_00_bc(GameLogic::C_COLLIDER, &hero_00_model, &hero_00, &collisionBoxShader, false);
    hero_00_an.playAnimation(0);
    hierarchy.addObject(&hero_00);
    //GameLogic::Proctor      hero_01("hero_01", glm::vec3(5.0f, 500.0f, 0.0f), glm::quat(1.0f, glm::radians(90.0f), 0.0f, 0.0f), glm::vec3(0.03f));
    GameLogic::Proctor      hero_01("hero_01", glm::vec3(770.0f, 5.0f, 850.0f), glm::quat(1.0f, glm::radians(90.0f), 0.0f, 0.0f), glm::vec3(0.03f));
    GameLogic::MeshRenderer hero_01_mr(GameLogic::C_MESH, &hero_01, modelLibrary.getModel(hero_01.name), &model3D);
    GameLogic::PlayerInput  hero_01_pi(GameLogic::C_MOVEMENT, &hero_01, false, &boat_b);
    GameLogic::BoxCollider  hero_01_bc(GameLogic::C_COLLIDER, modelLibrary.getModel(hero_01.name), &hero_01, &collisionBoxShader, false);
    hierarchy.addObject(&hero_01);
    hierarchy.addObject(&boat);
	
  /* GameLogic::Proctor      island_00("island_00", glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
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
    WaterMesh water("assets/shaders/water.vert", "assets/shaders/water.frag", "assets/shaders/water.geom", 700, 2000);
    float waterYpos = 1.5f;
    Framebuffer reflectFramebuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    Framebuffer refractFramebuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
#pragma endregion

#pragma region Input
    /* Create InputSystem elements */
    InputSystem::MouseInput* mouseInput = new InputSystem::MouseInput(mainScene.window);
    InputSystem::KeyboardInput* keyboardInput = new InputSystem::KeyboardInput(mainScene.window);

    float xValueRight = 0.2;
    float xValueLeft = 0.2;
    float yValueUp = 0.2;
    float yValueDown = 0.2;
#pragma endregion

#pragma region TEST CODE
    std::vector<GameLogic::Proctor*> tiles;
    //tiles.push_back(&island_00);
    //tiles.push_back(&island_01);
    Monster monsterSystem(&hero_00, tiles);

    Application::Scene sceneManager;
    sceneManager.changeCurrentScene("mainMenu");
    int tmpMainMenuIndex = 0;

    /* SHADOW MAPPING */
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion

    // music
    musicSource->setDefaultVolume(0.15);
    engine->play2D(musicSource, true);

    // waves
    waveSource->setDefaultVolume(1);
    engine->play2D(waveSource, true);


    /* Render loop */
    while (!glfwWindowShouldClose(mainScene.window))
    {
        frameCounter++;
        /* Dear ImGUI new frame setup */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* SCENE LOADER TEST CODE */
        if (sceneManager.cActiveScene["mainMenu"])
        {
            if (keyboardInput->isKeyReleased(GLFW_KEY_S))
            {
                tmpMainMenuIndex++;
                if (tmpMainMenuIndex > 2)
                    tmpMainMenuIndex = 2;

                bottleSource->setDefaultVolume(1);
                engine->play2D(bottleSource, false);
            }            
            
            else if (keyboardInput->isKeyReleased(GLFW_KEY_W))
            {
                tmpMainMenuIndex--;
                if (tmpMainMenuIndex < 0)
                    tmpMainMenuIndex = 0;

                bottleSource->setDefaultVolume(1);
                engine->play2D(bottleSource, false);
            }
            hero_00_pi.setActive(false);
            hero_01_pi.setActive(false);
            boat_b.setActive(false);
            hero_00_an.playAnimation(0);

            //enable cliping
            glEnable(GL_CLIP_DISTANCE0);

            camera.setProjection(projection);
            FrustumCulling::createViewFrustumFromMatrix(&camera);

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

            //---------------------------------------------------------------------------
            hierarchy.update(true, false);
            //---------------------------------------------------------------------------

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

            //---------------------------------------------------------------------------
            hierarchy.update(true, false);
            //---------------------------------------------------------------------------

            //UNBIND REFRACT FRAMEBUFFER AND TURN OFF CLIPING
            refractFramebuffer.unbindFramebuffer();
            glDisable(GL_CLIP_DISTANCE0);

            //HERE STARTS DEFAULT RENDERING
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            model3D.use();
            model3D.setUniform("projection", projection);
            view = camera.GetViewMatrix();
            model3D.setUniform("view", view);

            cameraSwitch(35, 60, 90, 45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01, yValueUp, yValueDown, xValueLeft, xValueRight);

            dirLight.render(model3D);

            //---------------------------------------------------------------------------
            hierarchy.update(false, false); // need to be set this way, otherwise debug window won't appear
            //---------------------------------------------------------------------------

            model = glm::translate(model, glm::vec3(-700, waterYpos, -700));
            water.render(model, projection, view, reflectFramebuffer.getTexture(), refractFramebuffer.getTexture(), mainScene.deltaTime, glm::vec3(camera.Position.x + 700, camera.Position.y, camera.Position.z + 700));

            // Main menu UI
            logo.render();

            startNotPressed.render();
            optionsNotPressed.render();
            exitNotPressed.render();


            /*if (((mouseInput->getCursorPosition().x > SCREEN_WIDTH * 0.4 && mouseInput->getCursorPosition().x < SCREEN_WIDTH * 0.6) 
                && (mouseInput->getCursorPosition().y > SCREEN_HEIGHT * 0.34 && mouseInput->getCursorPosition().y < SCREEN_HEIGHT * 0.39)) || (tmpMainMenuIndex == 0))*/
            if (tmpMainMenuIndex == 0)
            {
                startPressed.render();
                if (mouseInput->isButtonPressed(0) || keyboardInput->isKeyPressed(GLFW_KEY_SPACE) || keyboardInput->isKeyPressed(GLFW_KEY_V))
                {
                 
                    engine->play2D(bottleSource, false);
                    if (isPaused == true)
                    {
                        sceneManager.changeCurrentScene("game");
                    }
                    else
                        sceneManager.changeCurrentScene("enterStory_00");
                }
            }

            /*if (((mouseInput->getCursorPosition().x > SCREEN_WIDTH * 0.4 && mouseInput->getCursorPosition().x < SCREEN_WIDTH * 0.6) 
                && (mouseInput->getCursorPosition().y > SCREEN_HEIGHT * 0.64 && mouseInput->getCursorPosition().y < SCREEN_HEIGHT * 0.69)) || (tmpMainMenuIndex == 1))*/
            if (tmpMainMenuIndex == 1)
            {
                optionsPressed.render();
                if (mouseInput->isButtonReleased(0) || keyboardInput->isKeyReleased(GLFW_KEY_SPACE) || keyboardInput->isKeyReleased(GLFW_KEY_V))
                {
                    sceneManager.changeCurrentScene("options");
                }
            }
            if (tmpMainMenuIndex == 2)
            {
                exitPressed.render();
                if (mouseInput->isButtonPressed(0) || keyboardInput->isKeyPressed(GLFW_KEY_SPACE) || keyboardInput->isKeyPressed(GLFW_KEY_V))
                {
                    glfwSetWindowShouldClose(mainScene.window, true);
                }
            }
        }
        else if (sceneManager.cActiveScene["enterStory_00"])
        {
            story_00.render();
            points.render("Press space, to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 0.0f, 0.0f));
            if (keyboardInput->isKeyPressed(GLFW_KEY_SPACE))
                sceneManager.changeCurrentScene("enterStory_01");
            
        }
        else if (sceneManager.cActiveScene["enterStory_01"])
        {
            story_01.render();
            points.render("Press space, to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 0.0f, 0.0f));
            if (keyboardInput->isKeyPressed(GLFW_KEY_SPACE))
                sceneManager.changeCurrentScene("game");
            
        }
        else if (sceneManager.cActiveScene["options"])
        {
            points.render("Options", SCREEN_WIDTH * 0.35, SCREEN_HEIGHT * 0.5, 3, glm::vec3(1.0, 0.0, 0.0));
            if (keyboardInput->isKeyReleased(GLFW_KEY_ESCAPE))
                sceneManager.changeCurrentScene("mainMenu");

        }
        else if (sceneManager.cActiveScene["game"])
        {
            /* Check if player entered pause menu */
            if (keyboardInput->isKeyPressed(GLFW_KEY_ESCAPE))
            {
                isPaused = true;
                sceneManager.changeCurrentScene("mainMenu");
                engine->play2D(mainMenuSource, false);
            }
            else if (keyboardInput->isKeyPressed(GLFW_KEY_Q))
            {
                sceneManager.changeCurrentScene("exitStory_00");
            }            
            else if (keyboardInput->isKeyPressed(GLFW_KEY_E))
            {
                sceneManager.changeCurrentScene("exitStory_01");
            }

            /* Set players variables */
            hero_00_pi.setActive(true);
            hero_01_pi.setActive(true);
            hero_00_an.playAnimation(0);

            /* Set camera variables */
            camera.setProjection(projection);
            FrustumCulling::createViewFrustumFromMatrix(&camera);

            /* Enable cliiping */
            glEnable(GL_CLIP_DISTANCE0);

#pragma region WATER - ReflectionBuffer
            /* Start rendering meshes beneath water to ReflectionBuffer */
            reflectFramebuffer.bindFramebuffer();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* Set camera underwater to get reflection */
            float distance = 2 * (camera.Position.y - waterYpos);
            camera.Position.y -= distance;
            camera.Pitch = -camera.Pitch;
            camera.updateCameraVectors();

            /* Set shader variables - projection, view, plane */
            model3D.use();
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
            view = camera.GetViewMatrix();
            model = glm::mat4(1.0f);
            model3D.setUniform("projection", projection);
            model3D.setUniform("view", view);
            // Clipping everything under water plane
            model3D.setUniform("plane", glm::vec4(0, 1, 0, -waterYpos));

            /* Render objects for ReflectionBuffer */
            dirLight.render(model3D);
            hierarchy.update(true, false);
            skybox.render();

            /* Set camera position to default */
            camera.Position.y += distance;
            camera.Pitch = -camera.Pitch;
            camera.updateCameraVectors();

            /* Close ReflectionBuffer */
            reflectFramebuffer.unbindFramebuffer();
#pragma endregion

#pragma region WATER - RefractionBuffer
            /* Staet rendering meshes under the water surface to RefractionBuffer */
            refractFramebuffer.bindFramebuffer();
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* Set shader variables - projection, view, plane */
            model3D.use();
            view = camera.GetViewMatrix();
            model3D.setUniform("projection", projection);
            model3D.setUniform("view", view);
            model3D.setUniform("plane", glm::vec4(0, -1, 0, waterYpos));

            /* Render objects for ReflectionBuffer */
            dirLight.render(model3D);
            hierarchy.update(true, false);

            /* Close RefractionBuffer */
            refractFramebuffer.unbindFramebuffer();
#pragma endregion

            /* Disable cliiping */
            glDisable(GL_CLIP_DISTANCE0);

#pragma region Default rendering
            /* Clear everything */
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            /* Set model shader variables - projection, view */
            view = camera.GetViewMatrix();
            model3D.use();
            model3D.setUniform("projection", projection);
            model3D.setUniform("view", view);

            /* TODO: @Dawid - DEBUG between game camera and debug camera */
            cameraSwitch(35, 60, 90, 45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01, yValueUp, yValueDown, xValueLeft, xValueRight);

            /* Render light and update hierarchy */
            dirLight.render(model3D);
            hierarchy.update(false, true, collisionIncrement++); // need to be set this way, otherwise debug window won't appear

            /* Render water */
            model = glm::translate(model, glm::vec3(-700, waterYpos, -700));
            water.render(model, projection, view, reflectFramebuffer.getTexture(), refractFramebuffer.getTexture(), mainScene.deltaTime, glm::vec3(camera.Position.x + 700, camera.Position.y, camera.Position.z + 700));

            /* TODO: @Dawid - DEBUG Set camera shader variables - projection, view, collision */
            collisionBoxShader.use();
            collisionBoxShader.setUniform("projection", projection);
            collisionBoxShader.setUniform("view", view);
            collisionBoxShader.setUniformBool("collision", true);

            /* Render text */
            points.render(std::to_string(Points::getInstance()->getScore()), SCREEN_WIDTH * 0.05, SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.08), 1.3, glm::vec3(1.0, 0.75, 0.0));

            /* Render dukat */
            dukatSpinning[dukatSpinIndex].render();
            timeBetweenFrames -= mainScene.deltaTime;
            if (timeBetweenFrames <= 0)
            {
                dukatSpinIndex++;
                timeBetweenFrames = 0.15f;
            }
            if (dukatSpinIndex >= 8)
                dukatSpinIndex = 0;

            /* Update monster system */
            monsterSystem.update();
            // TODO: @Ignacy - potrzebne to?
            /*if (isPaused == true)
            {
                isPaused = false;
                Sleep(2500);
            }*/
#pragma endregion
        }
        
        else if (sceneManager.cActiveScene["exitStory_00"])
        {
            points.render("You loose", SCREEN_WIDTH * 0.3, SCREEN_HEIGHT * 0.4, 3, glm::vec3(1.0, 0.0, 0.0));
            if (keyboardInput->isKeyPressed(GLFW_KEY_SPACE))
                sceneManager.changeCurrentScene("mainMenu");
        }
        /* Update InputSystem */
        keyboardInput->update();
        mouseInput->update();
    	
        /* DEBUG - Draw DearImGUI */
        ImGui::Render(); // THIS SHOULD BE THE LAST, DO NOT TOUCH IT

        /* Clear frame at the end of the loop */
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(mainScene.window);
        glfwPollEvents();
    }

	/* Export scene to xml file */
  // std::cout << "Save scene? [y/n]" << std::endl;
  // std::string check;
  // std::cin >> check;
  // if (check == "y")
  //     Loader::Exporter::exportScene(hierarchy.getProctors(), "assets/scenes/exported_scene.xml");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    hierarchy.cleanup();

    glfwTerminate();
    engine->drop();

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

void cameraKeyboardInput(Application::Window* _scene, InputSystem::KeyboardInput* _keyboard)
{
    //if (_keyboard->isKeyDown(GLFW_KEY_ESCAPE))
        //glfwSetWindowShouldClose(_scene->window, true);
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void cameraSwitch(int minZoom,int maxZoom,float maxDistanceX, float maxDistanceY, InputSystem::MouseInput* mouseInput, InputSystem::KeyboardInput* keyboardInput, Application::Window* _scene, GameLogic::Proctor* player_1, GameLogic::Proctor* player_2,
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
        float xDistance = player_1->getWorldPosition()[0] - player_2->getWorldPosition()[0];
        float yDistance = player_1->getWorldPosition()[2] - player_2->getWorldPosition()[2];
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
        camera.Position = glm::vec3((player_1->getWorldPosition()[0] + player_2->getWorldPosition()[0]) / 2.f, camPos, (player_1->getWorldPosition()[2] + player_2->getWorldPosition()[2]) / 2.f);

        mouseInput->cursorEnable();
    }
}