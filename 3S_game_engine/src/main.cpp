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

static bool isDebugModeOn = false;
bool isDebugCameraOn = false;
bool isPaused = false;
int main()
{
    int tmpMainMenuIndex = 0;
    int tmpOptionsMenuIndex = 0;

#pragma region Scene init
    /* Load scene */
    Application::Window mainScene("TREASURE HEIST FINAL BUILD", SCREEN_WIDTH, SCREEN_HEIGHT, false); // false - window, true - fullscreen 
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
    Shader depthShader("assets/shaders/depthShader.vert", "assets/shaders/depthShader.frag");
    Shader debugShader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

#pragma endregion

#pragma region UI init
#pragma region MainMenu
    /* Buttons can be scaled with buttonScaleFactor */
    float buttonScaleFactor = 0.9f;
    /* Map that will contain name of each ui element and vector of top, bottom, left and right !calculated! positions
     * Each texture coord is calculated using this equation on width (same for height): ((texture_width / screen_width) * scale) 
     * I'm using map for optimization purpose, so each elemnt's texcoords can be calculated only once
     */
    std::map<std::string, glm::vec2> uiPositions;
    uiPositions["npressed"] = { ((586 * 0.5f) / SCREEN_WIDTH) * buttonScaleFactor, ((147 * 0.5f) / SCREEN_HEIGHT) * buttonScaleFactor };
    uiPositions["pressed"]  = { ((601 * 0.5f) / SCREEN_WIDTH) * buttonScaleFactor, ((149 * 0.5f) / SCREEN_HEIGHT) * buttonScaleFactor };
    uiPositions["logo"]     = { ((512 * 0.5f) / SCREEN_WIDTH) * 1.5f, ((251 * 0.5f) / SCREEN_HEIGHT) * 1.5f };
    float buttonsPos[4] = { 0.45f, 0.45f - 0.14f, 0.45f - (2 * 0.14f), 0.45f - (3 * 0.14f)};

    /* Load all main menu's ui elements with coorect positions */
    UIRender::UIElement logo("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures", "logo.png", 
        0.5f - uiPositions["logo"].x, 0.5f + uiPositions["logo"].x, 0.75f + uiPositions["logo"].y, 0.75f - uiPositions["logo"].y);
    UIRender::UIElement startButtonNPrs     ("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "start_button_nprsd.png",
        0.5f - uiPositions["npressed"].x, 0.5f + uiPositions["npressed"].x, buttonsPos[0] + uiPositions["npressed"].y, buttonsPos[0] - uiPositions["npressed"].y);
    UIRender::UIElement startButtonPrsd     ("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "start_button_prsd.png", 
        0.5f - uiPositions["pressed"].x,  0.5f + uiPositions["pressed"].x,  buttonsPos[0] + uiPositions["pressed"].y,  buttonsPos[0] - uiPositions["pressed"].y);
    UIRender::UIElement optionsNotPressed   ("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "options_button_nprsd.png", 
        0.5f - uiPositions["npressed"].x, 0.5f + uiPositions["npressed"].x, buttonsPos[1] + uiPositions["npressed"].y, buttonsPos[1] - uiPositions["npressed"].y);
    UIRender::UIElement optionsPressed      ("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "options_button_prsd.png", 
        0.5f - uiPositions["pressed"].x,  0.5f + uiPositions["pressed"].x,  buttonsPos[1] + uiPositions["pressed"].y,  buttonsPos[1] - uiPositions["pressed"].y);
    UIRender::UIElement exitNotPressed      ("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "exit_button_nprsd.png", 
        0.5f - uiPositions["npressed"].x, 0.5f + uiPositions["npressed"].x, buttonsPos[2] + uiPositions["npressed"].y, buttonsPos[2] - uiPositions["npressed"].y);
    UIRender::UIElement exitPressed         ("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "exit_button_prsd.png", 
        0.5f - uiPositions["pressed"].x,  0.5f + uiPositions["pressed"].x,  buttonsPos[2] + uiPositions["pressed"].y,  buttonsPos[2] - uiPositions["pressed"].y );
#pragma endregion
#pragma region Options
    UIRender::UIElement backNotPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "back_button_nprsd.png",
       0.15f - uiPositions["npressed"].x, 0.15f + uiPositions["npressed"].x, buttonsPos[2] + uiPositions["npressed"].y, buttonsPos[2] - uiPositions["npressed"].y);//0.01, 0.2, 0.32, 0.13);
    UIRender::UIElement backPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "back_button_prsd.png",
       0.15f - uiPositions["pressed"].x, 0.15f + uiPositions["pressed"].x, buttonsPos[2] + uiPositions["pressed"].y, buttonsPos[2] - uiPositions["pressed"].y);//0.01, 0.2, 0.32, 0.13);
    UIRender::UIElement creditsNotPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "credits_button_nprsd.png",
       0.15f - uiPositions["npressed"].x, 0.15f + uiPositions["npressed"].x, buttonsPos[1] + uiPositions["npressed"].y, buttonsPos[1] - uiPositions["npressed"].y);//0.01, 0.2, 0.32, 0.13);
    UIRender::UIElement creditsPressed("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/button", "credits_button_prsd.png",
       0.15f - uiPositions["pressed"].x, 0.15f + uiPositions["pressed"].x, buttonsPos[1] + uiPositions["pressed"].y, buttonsPos[1] - uiPositions["pressed"].y);//0.01, 0.2, 0.32, 0.13);
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
    // Options text
    UIRender::TextRender audio("assets/shaders/text.vert", "assets/shaders/text.frag", "assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);
    UIRender::TextRender music("assets/shaders/text.vert", "assets/shaders/text.frag", "assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);
    UIRender::TextRender gamma("assets/shaders/text.vert", "assets/shaders/text.frag", "assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);
    UIRender::TextRender shadows("assets/shaders/text.vert", "assets/shaders/text.frag", "assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);
    //UIRender::TextRender credits("assets/shaders/text.vert", "assets/shaders/text.frag", "assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);

    /* Animated dukat */
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

    /* Load models for not-serializable proctors */
    modelLibrary.addModel("assets/models/players/player_red_no_anim.fbx", "playerTwo", true, true);
 
    /* Boat */
    Loader::Model           boat_m("assets/models/boat/bbot.fbx", "boat", true, true);
    GameLogic::Proctor      boat("boat", glm::vec3(704.0f, 2.0f, 802.0f), glm::quat(1.0f, 0.0f, 1.6f, 0.0f), glm::vec3(0.09f));
    GameLogic::MeshRenderer boat_mr(GameLogic::C_MESH, &boat, &boat_m, &model3D);
    GameLogic::Boat         boat_b(GameLogic::C_MOVEMENT, &boat);
    GameLogic::Interactable boat_inter(GameLogic::C_INTERACTABLE, &boat);
    GameLogic::BoxCollider  boat_bc(GameLogic::C_COLLIDER, &boat_m, &boat, &collisionBoxShader, false);
    hierarchy.addObject(&boat);
	
    /* Player One */
    Loader::Model           hero_00_m("assets/models/players/c_r.fbx", "playerOne", true, false);
    GameLogic::Proctor      hero_00("playerOne", glm::vec3(770.0f, 62.0f, 850.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.1f));
    GameLogic::MeshRenderer hero_00_mr(GameLogic::C_MESH, &hero_00, &hero_00_m/*modelLibrary.getModel(hero_00.name)*/, &model3D);
    GameLogic::Anima        hero_00_an(GameLogic::C_ANIMA, &hero_00);
    hero_00_an.playAnimation(0);
    GameLogic::PlayerInput  hero_00_pi(GameLogic::C_MOVEMENT, &hero_00, true, &boat_b);
    //hero_00_pi.setFootstepSound(footstepSource);
    GameLogic::BoxCollider  hero_00_bc(GameLogic::C_COLLIDER, &hero_00_m, &hero_00, &collisionBoxShader, false);
    hierarchy.addObject(&hero_00);
    /* Player Two */
    GameLogic::Proctor      hero_01("playerTwo", glm::vec3(770.0f, 62.0f, 850.0f), glm::quat(1.0f, glm::radians(0.0f), 0.0f, 0.0f), glm::vec3(0.024f));
    GameLogic::MeshRenderer hero_01_mr(GameLogic::C_MESH, &hero_01, modelLibrary.getModel(hero_01.name), &model3D);
    //GameLogic::Anima        hero_01_an(GameLogic::C_ANIMA, &hero_01);
    GameLogic::PlayerInput  hero_01_pi(GameLogic::C_MOVEMENT, &hero_01, false, &boat_b);
    //hero_01_pi.setFootstepSound(footstepSource);
    GameLogic::BoxCollider  hero_01_bc(GameLogic::C_COLLIDER, modelLibrary.getModel(hero_01.name), &hero_01, &collisionBoxShader, false);
    hierarchy.addObject(&hero_01);
    ///* Enemies */
    Loader::Model           enemy_00_m("assets/models/serializable/locals_00.fbx", "enemy_00", true, true);
    GameLogic::Proctor      enemy_00("enemy_00", glm::vec3(770.f, 5.0f, 810.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.05f));
    GameLogic::MeshRenderer enemy_00_mr(GameLogic::C_MESH, &enemy_00, &enemy_00_m, &model3D);
    GameLogic::Enemy        enemy_00_e(&enemy_00, &hero_00, &hero_01);
    GameLogic::BoxCollider  enemy_00_bc(GameLogic::C_COLLIDER, &enemy_00_m, &enemy_00, &collisionBoxShader, false);
    hierarchy.addObject(&enemy_00);
    GameLogic::Proctor      monster("monster", glm::vec3(0.0f, -1000.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.04f));
    GameLogic::MeshRenderer monster_mr(GameLogic::C_MESH, &monster, modelLibrary.getModel(hero_01.name), &model3D);
    hierarchy.addObject(&monster);

    /* Create importer with given *.xml file */
    Loader::Importer importer("assets/scenes/exported_scene.xml", &model3D, true, 10.0f);
    
    /* Load models to hierarchy */
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
	
    GameLogic::Proctor safe_zone_1("safe_zone_1", glm::vec3(766.0f, 0.0f, 810.0f));
    GameLogic::Proctor safe_zone_2("safe_zone_2", glm::vec3(-350.0f, 0.0f, 350.0f));
    GameLogic::Proctor safe_zone_3("safe_zone_3", glm::vec3(-400.0f, 0.0f, -350.0f));
    GameLogic::Proctor safe_zone_4("safe_zone_4", glm::vec3(700.0f, 0.0f, 0.0f));
    GameLogic::Proctor safe_zone_5("safe_zone_5", glm::vec3(334.0f, 0.0f, -810.0f));
#pragma endregion

#pragma region Environment
    /* Create Skybox */
    Skybox skybox(&view, &projection, &camera);

    /* Lights */
    DirLight dirLight(
        // 170.8f  /  -134.9f  /  212.0f 
        glm::vec3(0.0f, -26.0f, -0.2f),
        glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
        glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
        glm::vec4(0.75f, 0.75f, 0.75f, 1.0f),
        BoundingRegion(glm::vec3(-50.0f, -50.0f, 0.5f), glm::vec3(50.0f, 50.0f, 80.0f)),
        8000
    );

    /* Water and water's frame buffer */
    WaterMesh water("assets/shaders/water.vert", "assets/shaders/water.frag", "assets/shaders/water.geom", 900, 3000);
    float waterYpos = 1.5f;
    Framebuffer reflectFramebuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    reflectFramebuffer.generate();
    reflectFramebuffer.bind();
    Texture reflectBufferTex("reflectBufferTex");
    reflectBufferTex.bind();
    reflectBufferTex.allocate(GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, GL_UNSIGNED_BYTE);
    reflectBufferTex.setParams(true, GL_LINEAR, GL_LINEAR);
    reflectFramebuffer.attachTexture(GL_COLOR_ATTACHMENT0, reflectBufferTex);
    reflectFramebuffer.allocateAndAttachRBO(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8);
    
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
    std::vector<GameLogic::Proctor*> zones;
    zones.push_back(&safe_zone_1);
    zones.push_back(&safe_zone_2);
    zones.push_back(&safe_zone_3);
    zones.push_back(&safe_zone_4);
    zones.push_back(&safe_zone_5);
    Monster monsterSystem(&boat, zones);

    Application::Scene sceneManager;
    sceneManager.changeCurrentScene("mainMenu");
    bool isSoundsPaused = false;

    int animID = 0;

    Texture debug;
    bool isShadowMapVisible = false;
    UIRender::UIElement shadowMapUI("assets/shaders/ui.vert", "assets/shaders/ui.frag", debug, 0.05, 0.05 + (400.0f / SCREEN_WIDTH), 0.95 - (400.0f / SCREEN_HEIGHT), 0.95);

    float gammaCorrection = 1.2f;
#pragma endregion
#pragma region Audio

    float audioValues = 1.00f;
    float musicValues = 0.15f;

    //SoundEngine
    ISoundEngine* engine = createIrrKlangDevice();
    // music
    //ISoundEngine* musicEngine = createIrrKlangDevice();
    ISound* musicSource = engine->play2D("assets/audio/music/shanty.ogg", true, false, true);
    musicSource->setVolume(0.15);
    // waves
    //ISoundEngine* waveEngine = createIrrKlangDevice();
    ISound* waveSource = engine->play2D("assets/audio/sounds/background.ogg", true, false, true);
    // menu
    ISoundSource* bottleSource = engine->addSoundSourceFromFile("assets/audio/sounds/bottle.ogg");
    ISoundSource* mainMenuSource = engine->addSoundSourceFromFile("assets/audio/sounds/mainMenu.ogg");
    // monster
    ISoundSource* heartBeatSource = engine->addSoundSourceFromFile("assets/audio/sounds/beat.ogg");

    // music
    musicSource->setVolume(musicValues);
    // waves
    waveSource->setVolume(audioValues);
    // bottleSource
    bottleSource->setDefaultVolume(audioValues);
    // mainMenu
    mainMenuSource->setDefaultVolume(audioValues);
#pragma endregion

    bool restartFlag = false;
    int cashSize = importer.cash.size();

    /* Render loop */
    while (!glfwWindowShouldClose(mainScene.window))
    {
        /* Set gamma value */
        // TODO: @Ignacy - czy gamma musi byc ustawiana za kazdym razem? Moze tylko w opcjach?
        model3D.use();
        model3D.setUniformFloat("gamma", gammaCorrection);
        /* At first, update scene time */
        mainScene.update();

        frameCounter++;

        // music volume
        musicSource->setVolume(musicValues);
        // waves volume
        waveSource->setVolume(audioValues);
        // bottleSource volume
        bottleSource->setDefaultVolume(audioValues);
        // mainMenu volume
        mainMenuSource->setDefaultVolume(audioValues);

        /* Dear ImGUI new frame setup */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* Enable/disable debug mode */
        if (keyboardInput->isKeyPressed(GLFW_KEY_TAB))
            isDebugModeOn = !isDebugModeOn;

        /* SCENE LOADER */
        if (sceneManager.cActiveScene["mainMenu"])
        {
            if (keyboardInput->isKeyReleased(GLFW_KEY_S))
            {
                tmpMainMenuIndex++;
                if (tmpMainMenuIndex > 2)
                    tmpMainMenuIndex = 2;

                engine->play2D(bottleSource, false);
            }            
            else if (keyboardInput->isKeyReleased(GLFW_KEY_W))
            {
                tmpMainMenuIndex--;
                if (tmpMainMenuIndex < 0)
                    tmpMainMenuIndex = 0;

                engine->play2D(bottleSource, false);
            }

            hero_00_pi.setActive(false);
            hero_01_pi.setActive(false);
            boat_b.setActive(false);

            /* Set camera variables */
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
            camera.setProjection(projection);
            //camera.Position = { 680.0f, 37.0f, 719.0f };
            //camera.Pitch = -17.5f;
            //camera.Yaw = 53.4f;
            //FrustumCulling::createViewFrustumFromMatrix(&camera);

#pragma region SHADOWS - ShadowsBuffer
            /* Activate directional light's FBO */
            dirLight.shadowFBO.activate();

            depthShader.use();
            view = camera.GetViewMatrix();
            glm::mat4 lightView = glm::lookAt(-2.0f * dirLight.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            /* Note that those values are constant */
            glm::mat4 proj = glm::ortho(680.0f, 860.0f, -899.0f, -719.0f, dirLight.br.min.z, dirLight.br.max.z);
            dirLight.lightSpaceMatrix = proj * lightView;
            depthShader.setUniform("lightSpaceMatrix", dirLight.lightSpaceMatrix);
            dirLight.render(depthShader, 31);
            hierarchy.renderWithShader(&depthShader);
            shadowMapUI.setTexture(dirLight.shadowFBO.textures[0]);
            dirLight.shadowFBO.unbind();
#pragma endregion

            /* Enable cliiping */
            glEnable(GL_CLIP_DISTANCE0);

#pragma region WATER - ReflectionBuffer
            /* Start rendering meshes beneath water to ReflectionBuffer */
            reflectFramebuffer.activate();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

            /* Set camera underwater to get reflection */
            float distance = 2 * (camera.Position.y - waterYpos);
            camera.Position.y -= distance;
            camera.Pitch = -camera.Pitch;
            camera.updateCameraVectors();

            /* Set shader variables - projection, view, plane */
            model3D.use();
            view = camera.GetViewMatrix();
            model = glm::mat4(1.0f);
            model3D.setUniform("projection", projection);
            model3D.setUniform("view", view);
            model3D.setUniform("plane", glm::vec4(0, 1, 0, -waterYpos)); // Clipping everything under water plane

            /* Render objects for ReflectionBuffer */
            dirLight.render(model3D, 31);
            hierarchy.renderWithShader(&model3D);
            skybox.render();

            /* Set camera position to default */
            camera.Position.y += distance;
            camera.Pitch = -camera.Pitch;
            camera.updateCameraVectors();

            /* Close ReflectionBuffer */
            reflectFramebuffer.unbind();
#pragma endregion

            /* Disable cliiping */
            glDisable(GL_CLIP_DISTANCE0);

#pragma region Default rendering
            /* Clear everything */
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CULL_FACE);

            /* Set model shader variables - projection, view */
            view = camera.GetViewMatrix();
            model3D.use();
            model3D.setUniform("projection", projection);
            model3D.setUniform("view", view);
            model3D.setUniform("viewPos", camera.Position);

            /* TODO: @Dawid - DEBUG between game camera and debug camera */
            cameraSwitch(35, 60, 90, 45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01, yValueUp, yValueDown, xValueLeft, xValueRight);

            /* Render light and update hierarchy */
            dirLight.render(model3D, 31);
            //hierarchy.update(false, isDebugModeOn, collisionIncrement++);
            hierarchy.renderWithShader(&model3D);

            /* Render water */
            model = glm::translate(model, glm::vec3(-1100, waterYpos, -1100));
            water.render(model, projection, view, reflectBufferTex.id, mainScene.deltaTime, glm::vec3(camera.Position.x + 1100, camera.Position.y, camera.Position.z + 1100));
#pragma endregion

#pragma region UI Elements
            /* Render title */
            logo.render();

            /* Start button */
            if (tmpMainMenuIndex == 0)
            {
                startButtonPrsd.render();
                if (keyboardInput->isKeyPressed(GLFW_KEY_V) || keyboardInput->isKeyPressed(GLFW_KEY_PERIOD))
                {
                    engine->play2D(bottleSource, false);
                    if (isPaused == true)
                        sceneManager.changeCurrentScene("game");
                    else
                        sceneManager.changeCurrentScene("enterStory_00");
                }
            }
            else
                startButtonNPrs.render();

            /* Options button */
            if (tmpMainMenuIndex == 1)
            {
                optionsPressed.render();
                if (keyboardInput->isKeyPressed(GLFW_KEY_V) || keyboardInput->isKeyPressed(GLFW_KEY_PERIOD))
                    sceneManager.changeCurrentScene("options");
            }
            else
                optionsNotPressed.render();

            /* Exit button */
            if (tmpMainMenuIndex == 2)
            {
                exitPressed.render();
                if (keyboardInput->isKeyPressed(GLFW_KEY_V) || keyboardInput->isKeyPressed(GLFW_KEY_PERIOD))
                    glfwSetWindowShouldClose(mainScene.window, true);
            }
            else
                exitNotPressed.render();
#pragma endregion

#pragma region Debug Mode
            if (isDebugModeOn)
            {
                ImGui::Begin("Camera and dir light");
                {
                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                    float variables[4];
                    /* CAMERA */
                    ImGui::Text("---- CAMERA ----");
                    // Position
                    variables[0] = camera.Position.x; variables[1] = camera.Position.y; variables[2] = camera.Position.z;
                    ImGui::DragFloat3("campos", variables);
                    camera.Position.x = variables[0]; camera.Position.y = variables[1]; camera.Position.z = variables[2];
                    // Pitch
                    variables[0] = camera.Pitch;
                    ImGui::DragFloat("pitch", variables);
                    camera.Pitch = variables[0];
                    // Yaw
                    variables[0] = camera.Yaw;
                    ImGui::DragFloat("yaw", variables);
                    camera.Yaw = variables[0];
                    ImGui::Text("---- LIGHT -----");
                    // Shadow map
                    ImGui::Checkbox("show shadow map", &isShadowMapVisible);
                    /* Dir light */
                    // Direction
                    variables[0] = dirLight.direction.x; variables[1] = dirLight.direction.y; variables[2] = dirLight.direction.z;
                    ImGui::DragFloat3("direction", variables);
                    dirLight.direction.x = variables[0]; dirLight.direction.y = variables[1]; dirLight.direction.z = variables[2];
                    // Ambient
                    variables[0] = dirLight.ambient.x; variables[1] = dirLight.ambient.y; variables[2] = dirLight.ambient.z; variables[3] = dirLight.ambient.w;
                    ImGui::DragFloat4("ambient", variables);
                    dirLight.ambient.x = variables[0]; dirLight.ambient.y = variables[1]; dirLight.ambient.z = variables[2]; dirLight.ambient.w = variables[3];
                    // Diffuse
                    variables[0] = dirLight.diffuse.x; variables[1] = dirLight.diffuse.y; variables[2] = dirLight.diffuse.z; variables[3] = dirLight.diffuse.w;
                    ImGui::DragFloat4("diffuse", variables);
                    dirLight.diffuse.x = variables[0]; dirLight.diffuse.y = variables[1]; dirLight.diffuse.z = variables[2]; dirLight.diffuse.w = variables[3];
                    // Specular
                    variables[0] = dirLight.specular.x; variables[1] = dirLight.specular.y; variables[2] = dirLight.specular.z; variables[3] = dirLight.specular.w;
                    ImGui::DragFloat4("specular", variables);
                    dirLight.specular.x = variables[0]; dirLight.specular.y = variables[1]; dirLight.specular.z = variables[2]; dirLight.specular.w = variables[3];
                    // Bounding region min
                    variables[0] = dirLight.br.min.x; variables[1] = dirLight.br.min.y; variables[2] = dirLight.br.min.z;
                    ImGui::DragFloat3("br.min", variables);
                    dirLight.br.min.x = variables[0]; dirLight.br.min.y = variables[1]; dirLight.br.min.z = variables[2];
                    // Bounding region max
                    variables[0] = dirLight.br.max.x; variables[1] = dirLight.br.max.y; variables[2] = dirLight.br.max.z;
                    ImGui::DragFloat3("br.max", variables);
                    dirLight.br.max.x = variables[0]; dirLight.br.max.y = variables[1]; dirLight.br.max.z = variables[2];
                }
                ImGui::End();

                if (keyboardInput->isKeyPressed(GLFW_KEY_Q))
                    sceneManager.changeCurrentScene("exitStory_00");
                else if (keyboardInput->isKeyPressed(GLFW_KEY_E))
                    sceneManager.changeCurrentScene("exitStory_01");

                collisionBoxShader.use();
                collisionBoxShader.setUniform("projection", projection);
                collisionBoxShader.setUniform("view", view);
                collisionBoxShader.setUniformBool("collision", true);

                if (isShadowMapVisible)
                    shadowMapUI.render();
            }
#pragma endregion
        }
        else if (sceneManager.cActiveScene["enterStory_00"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            story_00.render();
            points.render("Press space, to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 0.0f, 0.0f));
            if (keyboardInput->isKeyPressed(GLFW_KEY_SPACE))
                sceneManager.changeCurrentScene("enterStory_01");
            
        }
        else if (sceneManager.cActiveScene["enterStory_01"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            story_01.render();
            points.render("Press space, to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 0.0f, 0.0f));
            if (keyboardInput->isKeyPressed(GLFW_KEY_SPACE))
                sceneManager.changeCurrentScene("game");
            
        }
        else if (sceneManager.cActiveScene["options"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //backNotPressed.render();

            if (tmpOptionsMenuIndex != 0)
            {
                audio.render("Audio: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.8, 1.5, glm::vec3(1.0, 0.0, 0.0));
                audio.render(std::to_string(audioValues), SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 0.8, 1.5, glm::vec3(1.0, 0.0, 0.0));
            }
            //audio.render("Audio: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.8, 1.5, glm::vec3(1.0, 0.0, 0.0));
            if (tmpOptionsMenuIndex != 1)
            {
                music.render("Music: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.7, 1.5, glm::vec3(1.0, 0.0, 0.0));
                music.render(std::to_string(musicValues), SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 0.7, 1.5, glm::vec3(1.0, 0.0, 0.0));
            }
            if (tmpOptionsMenuIndex != 2)
                gamma.render("Gamma: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.6, 1.5, glm::vec3(1.0, 0.0, 0.0));

            if (tmpOptionsMenuIndex != 3)
                shadows.render("Shadows resolution: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.5, 1.5, glm::vec3(1.0, 0.0, 0.0));

            if (tmpOptionsMenuIndex != 4);
                //creditsNotPressed.render();
                //credits.render("Credits", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.4, 1.5, glm::vec3(1.0, 0.0, 0.0));

            if (keyboardInput->isKeyReleased(GLFW_KEY_S))
            {
                tmpOptionsMenuIndex++;
                if (tmpOptionsMenuIndex > 5)
                    tmpOptionsMenuIndex = 5;

                engine->play2D(bottleSource, false);
            }

            else if (keyboardInput->isKeyReleased(GLFW_KEY_W))
            {
                tmpOptionsMenuIndex--;
                if (tmpOptionsMenuIndex < 0)
                    tmpOptionsMenuIndex = 0;

                engine->play2D(bottleSource, false);
            }

            if (tmpOptionsMenuIndex == 0)
            {
                audio.render("Audio: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.8, 1.5, glm::vec3(0.0, 1.0, 0.0));
                audio.render(std::to_string(audioValues), SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 0.8, 1.5, glm::vec3(0.0, 1.0, 0.0));
                if (keyboardInput->isKeyDown(GLFW_KEY_A))
                {
                    if (audioValues >= 0.02)
                        audioValues -= 0.02f;
                }
                else if (keyboardInput->isKeyDown(GLFW_KEY_D))
                {
                    if (audioValues <= 0.98)
                        audioValues += 0.02f;
                }

                for (int i = 0; i < cashSize; i++)
                {
                    importer.cash.at(i).get()->getCoinSource()->setDefaultVolume(audioValues);
                }
                for (int i = 0; i < 4; i++)
                {
                    hero_00_pi.getPlayerSounds().at(i)->setDefaultVolume(audioValues);
                    hero_01_pi.getPlayerSounds().at(i)->setDefaultVolume(audioValues);
                }
            }

            if (tmpOptionsMenuIndex == 1)
            {
                music.render("Music: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.7, 1.5, glm::vec3(0.0, 1.0, 0.0));
                music.render(std::to_string(musicValues), SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 0.7, 1.5, glm::vec3(0.0, 1.0, 0.0));
                if (keyboardInput->isKeyDown(GLFW_KEY_A))
                {
                    if (musicValues >= 0.01)
                        musicValues -= 0.01f;
                }
                else if (keyboardInput->isKeyDown(GLFW_KEY_D))
                {
                    if (musicValues <= 0.99)
                        musicValues += 0.01f;
                }

            }
            if (tmpOptionsMenuIndex == 2)
            {
                gamma.render("Gamma: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.6, 1.5, glm::vec3(0.0, 1.0, 0.0));
            }

            if (tmpOptionsMenuIndex == 3)
            {
                shadows.render("Shadows resolution: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.5, 1.5, glm::vec3(0.0, 1.0, 0.0));
            }

            if (tmpOptionsMenuIndex == 4)
            {
                //credits.render("Credits", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.4, 1.5, glm::vec3(0.0, 1.0, 0.0));
                creditsPressed.render();
                if (keyboardInput->isKeyReleased(GLFW_KEY_V))
                    sceneManager.changeCurrentScene("credits");
            }
            else
                creditsNotPressed.render();

            if (tmpOptionsMenuIndex == 5)
            {
                backPressed.render();
                if (keyboardInput->isKeyReleased(GLFW_KEY_V))
                {
                    sceneManager.changeCurrentScene("mainMenu");
                    engine->play2D(mainMenuSource, false);
                }
            }
            else
                backNotPressed.render();


        }
        else if (sceneManager.cActiveScene["credits"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            points.render("Game made by Slightly Superior", SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.8, 2.5, glm::vec3(1.0, 0.0, 0.0));
            points.render("TGiSK:", SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.7, 2, glm::vec3(1.0, 0.0, 0.0));
            points.render("Dawid Paweloszek", SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.65, 1, glm::vec3(1.0, 0.0, 0.0));
            points.render("Weronika Dobies", SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.60, 1, glm::vec3(1.0, 0.0, 0.0));
            points.render("Kuba Podkomorka", SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.55, 1, glm::vec3(1.0, 0.0, 0.0));
            points.render("Ignacy Olesinski", SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.50, 1, glm::vec3(1.0, 0.0, 0.0));
            points.render("Grafika:", SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.4, 2, glm::vec3(1.0, 0.0, 0.0));
            points.render("Kinga Kudelska", SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.35, 1, glm::vec3(1.0, 0.0, 0.0));
            points.render("Mateusz Sudra", SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.30, 1, glm::vec3(1.0, 0.0, 0.0));
            points.render("Press escape, to go back to options menu", 0.05, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 0.0f, 0.0f));

            if (keyboardInput->isKeyReleased(GLFW_KEY_ESCAPE))
            {
                sceneManager.changeCurrentScene("options");
                engine->play2D(mainMenuSource, false);
            }
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

            /* Set players variables */
            hero_00_pi.setActive(true);
            hero_01_pi.setActive(true);

            /* Set camera variables */
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
            camera.setProjection(projection);
            FrustumCulling::createViewFrustumFromMatrix(&camera);

#pragma region SHADOWS - ShadowsBuffer
            /* Activate directional light's FBO */
            dirLight.shadowFBO.activate();

            depthShader.use();
            view = camera.GetViewMatrix();
            glm::mat4 lightView = glm::lookAt(-2.0f * dirLight.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 proj = glm::ortho(
                dirLight.br.min.x + camera.Position.x,
                dirLight.br.max.x + camera.Position.x,
                dirLight.br.min.y - camera.Position.z,
                dirLight.br.max.y - camera.Position.z,
                dirLight.br.min.z, 
                dirLight.br.max.z);
            dirLight.lightSpaceMatrix = proj * lightView;
            depthShader.setUniform("lightSpaceMatrix", dirLight.lightSpaceMatrix);
            dirLight.render(depthShader, 31);
            hierarchy.renderWithShader(&depthShader);
            shadowMapUI.setTexture(dirLight.shadowFBO.textures[0]);
            dirLight.shadowFBO.unbind();
#pragma endregion

            /* Enable cliiping */
            glEnable(GL_CLIP_DISTANCE0);

#pragma region WATER - ReflectionBuffer
            /* Start rendering meshes beneath water to ReflectionBuffer */
            reflectFramebuffer.activate();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

            /* Set camera underwater to get reflection */
            float distance = 2 * (camera.Position.y - waterYpos);
            camera.Position.y -= distance;
            camera.Pitch = -camera.Pitch;
            camera.updateCameraVectors();

            /* Set shader variables - projection, view, plane */
            model3D.use();
            view = camera.GetViewMatrix();
            model = glm::mat4(1.0f);
            model3D.setUniform("projection", projection);
            model3D.setUniform("view", view);
            model3D.setUniform("plane", glm::vec4(0, 1, 0, -waterYpos)); // Clipping everything under water plane

            /* Render objects for ReflectionBuffer */
            dirLight.render(model3D, 31);
            hierarchy.renderWithShader(&model3D);
            skybox.render();

            /* Set camera position to default */
            camera.Position.y += distance;
            camera.Pitch = -camera.Pitch;
            camera.updateCameraVectors();

            /* Close ReflectionBuffer */
            reflectFramebuffer.unbind();
#pragma endregion

            /* Disable cliiping */
            glDisable(GL_CLIP_DISTANCE0);

#pragma region Default rendering
            /* Clear everything */
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CULL_FACE);
            
            /* Set model shader variables - projection, view */
            view = camera.GetViewMatrix();
            model3D.use();
            model3D.setUniform("projection", projection);
            model3D.setUniform("view", view);
            model3D.setUniform("viewPos", camera.Position);
                
            /* TODO: @Dawid - DEBUG between game camera and debug camera */
            cameraSwitch(35, 60, 90, 45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01, yValueUp, yValueDown, xValueLeft, xValueRight);

            /* Render light and update hierarchy */
            dirLight.render(model3D, 31);
            hierarchy.update(false, isDebugModeOn, collisionIncrement++); // need to be set this way, otherwise debug window won't appear

            /* Render water */
            model = glm::translate(model, glm::vec3(-1100, waterYpos, -1100));
            water.render(model, projection, view, reflectBufferTex.id, mainScene.deltaTime, glm::vec3(camera.Position.x + 1100, camera.Position.y, camera.Position.z + 1100));

            /* Render text */
            points.render(std::to_string(Points::getInstance()->getScore()), SCREEN_WIDTH * 0.05, SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.08), 1.3, glm::vec3(1.0, 0.75, 0.0));

            /* Check if player picked up clue */
            if (hero_00_pi.isCluePickedUp)
                points.render(hero_00_pi.clueText, SCREEN_WIDTH * 0.25, SCREEN_HEIGHT * 0.5, 2, glm::vec3(1.0, 0.0, 0.0));
            else if (hero_01_pi.isCluePickedUp)
                points.render(hero_01_pi.clueText, SCREEN_WIDTH * 0.25, SCREEN_HEIGHT * 0.5, 2, glm::vec3(1.0, 0.0, 0.0));

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
            monsterSystem.update(engine, heartBeatSource,bottleSource,&monster);
            if (monsterSystem.isGameOver) 
            {
                monsterSystem.isGameOver = false;
                sceneManager.changeCurrentScene("exitStory_00"); //TODO reset main scene
            }
#pragma endregion

#pragma region Debug Mode
            if (isDebugModeOn)
            {
                ImGui::Begin("Camera and dir light");
                {
                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                    float variables[4];
                    /* CAMERA */
                    ImGui::Text("---- CAMERA ----");
                    // Position
                    variables[0] = camera.Position.x; variables[1] = camera.Position.y; variables[2] = camera.Position.z;
                    ImGui::DragFloat3("campos", variables);
                    camera.Position.x = variables[0]; camera.Position.y = variables[1]; camera.Position.z = variables[2];
                    // Pitch
                    variables[0] = camera.Pitch;
                    ImGui::DragFloat("pitch", variables);
                    camera.Pitch = variables[0];
                    // Yaw
                    variables[0] = camera.Yaw;
                    ImGui::DragFloat("yaw", variables);
                    camera.Yaw = variables[0];
                    ImGui::Text("---- LIGHT -----");
                    // Shadow map
                    ImGui::Checkbox("show shadow map", &isShadowMapVisible);
                    /* Dir light */
                    // Direction
                    variables[0] = dirLight.direction.x; variables[1] = dirLight.direction.y; variables[2] = dirLight.direction.z;
                    ImGui::DragFloat3("direction", variables);
                    dirLight.direction.x = variables[0]; dirLight.direction.y = variables[1]; dirLight.direction.z = variables[2];
                    // Ambient
                    variables[0] = dirLight.ambient.x; variables[1] = dirLight.ambient.y; variables[2] = dirLight.ambient.z; variables[3] = dirLight.ambient.w;
                    ImGui::DragFloat4("ambient", variables);
                    dirLight.ambient.x = variables[0]; dirLight.ambient.y = variables[1]; dirLight.ambient.z = variables[2]; dirLight.ambient.w = variables[3];
                    // Diffuse
                    variables[0] = dirLight.diffuse.x; variables[1] = dirLight.diffuse.y; variables[2] = dirLight.diffuse.z; variables[3] = dirLight.diffuse.w;
                    ImGui::DragFloat4("diffuse", variables);
                    dirLight.diffuse.x = variables[0]; dirLight.diffuse.y = variables[1]; dirLight.diffuse.z = variables[2]; dirLight.diffuse.w = variables[3];
                    // Specular
                    variables[0] = dirLight.specular.x; variables[1] = dirLight.specular.y; variables[2] = dirLight.specular.z; variables[3] = dirLight.specular.w;
                    ImGui::DragFloat4("specular", variables);
                    dirLight.specular.x = variables[0]; dirLight.specular.y = variables[1]; dirLight.specular.z = variables[2]; dirLight.specular.w = variables[3];
                    // Bounding region min
                    variables[0] = dirLight.br.min.x; variables[1] = dirLight.br.min.y; variables[2] = dirLight.br.min.z;
                    ImGui::DragFloat3("br.min", variables);
                    dirLight.br.min.x = variables[0]; dirLight.br.min.y = variables[1]; dirLight.br.min.z = variables[2];
                    // Bounding region max
                    variables[0] = dirLight.br.max.x; variables[1] = dirLight.br.max.y; variables[2] = dirLight.br.max.z;
                    ImGui::DragFloat3("br.max", variables);
                    dirLight.br.max.x = variables[0]; dirLight.br.max.y = variables[1]; dirLight.br.max.z = variables[2];
                    // Gamma correction
                    ImGui::SliderFloat("gamma", &gammaCorrection, 0.0f, 3.0f);
                }
                ImGui::End();

                if (keyboardInput->isKeyPressed(GLFW_KEY_Q))
                    sceneManager.changeCurrentScene("exitStory_00");
                else if (keyboardInput->isKeyPressed(GLFW_KEY_E))
                    sceneManager.changeCurrentScene("exitStory_01");

                collisionBoxShader.use();
                collisionBoxShader.setUniform("projection", projection);
                collisionBoxShader.setUniform("view", view);
                collisionBoxShader.setUniformBool("collision", true);

                if (isShadowMapVisible)
                    shadowMapUI.render();
            }
#pragma endregion
        }
        else if (sceneManager.cActiveScene["exitStory_00"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            points.render("You loose", SCREEN_WIDTH * 0.35, SCREEN_HEIGHT * 0.5, 3, glm::vec3(1.0, 0.0, 0.0));
            points.render("Press space, to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 0.0f, 0.0f));
            if (keyboardInput->isKeyReleased(GLFW_KEY_SPACE))
            { 
                restartFlag = true;
                sceneManager.changeCurrentScene("mainMenu");
            }
        }
        else if (sceneManager.cActiveScene["exitStory_01"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            points.render("You win, ARRRRRR", SCREEN_WIDTH * 0.35, SCREEN_HEIGHT * 0.5, 3, glm::vec3(1.0, 0.0, 0.0));
            points.render("Press space, to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 0.0f, 0.0f));
            if (keyboardInput->isKeyPressed(GLFW_KEY_SPACE))
            {
                restartFlag = true;
                sceneManager.changeCurrentScene("mainMenu");
            }
        }

        if (restartFlag)
        {
            restartFlag = false;
            Points::getInstance()->setScore(0);
            unsigned int size = hierarchy.getProctors().size();
            for (int i = 0; i < size; i++)
            {
                hierarchy.getProctors()[i]->activate();
            }
            boat.activate();
            hero_00.activate();
            hero_01.activate();
            monster.activate();
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
    //Loader::Exporter::exportScene(hierarchy.getProctors(), "assets/scenes/exported_scene.xml");

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
    glViewport(0, 0, width, height);
}

void cameraSwitch(int minZoom,int maxZoom,float maxDistanceX, float maxDistanceY, InputSystem::MouseInput* mouseInput, InputSystem::KeyboardInput* keyboardInput, Application::Window* _scene, GameLogic::Proctor* player_1, GameLogic::Proctor* player_2,
    float& yValueUp, float& yValueDown, float& xValueLeft, float& xValueRight)
{
    if (keyboardInput->isKeyPressed(GLFW_KEY_P)) {
        isDebugCameraOn = !isDebugCameraOn;
    }

    if (isDebugCameraOn)
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
    if (!isDebugCameraOn)
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