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
#include <iostream>
#include <iomanip>
#include <fstream>

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
void cameraSwitch(int minZoom, int maxZoom, float maxDistanceX, float maxDistanceY, InputSystem::MouseInput* mouseInput, InputSystem::KeyboardInput* keyboardInput, 
    Application::Window* _scene, GameLogic::Proctor* player_1, GameLogic::Proctor* player_2,
    GameLogic::Boat* boat_b, GameLogic::Proctor* boat, GameLogic::PlayerInput* input_00, GameLogic::PlayerInput* input_01);
int normalizedAudioValue(float _value);
int normalizedGammaValue(float _value);

// settings
const unsigned int SCREEN_WIDTH = 1920;
const unsigned int SCREEN_HEIGHT = 1080;

// camera
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

static bool isDebugModeOn = false;
bool isDebugCameraOn = false;
bool isPaused = false;

/* CREATE GLOBAL VALUES USE IN WHOLE PROJECT */
Application::Window mainScene("TREASURE HEIST FINAL BUILD", SCREEN_WIDTH, SCREEN_HEIGHT, true); // false - window, true - fullscreen 
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main()
{
    int tmpMainMenuIndex = 0;
    int tmpOptionsMenuIndex = 0;
    int tmpCreditsIndex = 1;
    int collisionIncrement = 1;
    int frameCounter = 0;
    int random = 0;
    bool restartFlag = false;

#pragma region Scene init
    /* Load scene */
    glfwMakeContextCurrent(mainScene.window);
    glfwSetFramebufferSizeCallback(mainScene.window, framebuffer_size_callback);
    glfwSetInputMode(mainScene.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0); // Uncoment this line to remove 60FPS cap

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
    Shader model3D              ("./assets/shaders/model3D.vert",       "./assets/shaders/model3D.frag");
    Shader textShader           ("./assets/shaders/text.vert",          "./assets/shaders/text.frag");
    Shader collisionBoxShader   ("./assets/shaders/boxCollider2.vert",  "./assets/shaders/boxCollider2.frag");
    Shader depthShader          ("./assets/shaders/depthShader.vert",   "./assets/shaders/depthShader.frag");
#pragma endregion

#pragma region UI init
#pragma region MainMenu
    /* Buttons can be scaled with buttonScaleFactor */
    float menuButtonScaleFactor = 0.9f;
    float optionsButtonScaleFactor = 0.7f;
    /* Map that will contain name of each ui element and vector of top, bottom, left and right !calculated! positions
     * Each texture coord is calculated using this equation on width (same for height): ((texture_width / screen_width) * scale)
     * I'm using map for optimization purpose, so each elemnt's texcoords can be calculated only once
     */
    std::map<std::string, glm::vec2> uiPositions;
    uiPositions["npressed"] = { ((586 * 0.5f) / SCREEN_WIDTH), ((147 * 0.5f) / SCREEN_HEIGHT) };
    uiPositions["pressed"] = { ((601 * 0.5f) / SCREEN_WIDTH), ((149 * 0.5f) / SCREEN_HEIGHT) };
    uiPositions["logo"] = { ((512 * 0.5f) / SCREEN_WIDTH) * 1.5f, ((251 * 0.5f) / SCREEN_HEIGHT) * 1.5f };
    float buttonsPos[4] = { 0.45f, 0.45f - 0.14f, 0.45f - (2 * 0.14f), 0.45f - (3 * 0.14f) };

    /* Load all main menu's ui elements with coorect positions */
    UIRender::UIElement logo("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures", "logo.png",
        0.5f - uiPositions["logo"].x, 0.5f + uiPositions["logo"].x, 0.75f + uiPositions["logo"].y, 0.75f - uiPositions["logo"].y);
    UIRender::UIElement startButtonNPrs("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "start_button_nprsd.png",
        0.5f - uiPositions["npressed"].x * menuButtonScaleFactor, 0.5f + uiPositions["npressed"].x * menuButtonScaleFactor, buttonsPos[0] + uiPositions["npressed"].y * menuButtonScaleFactor, buttonsPos[0] - uiPositions["npressed"].y * menuButtonScaleFactor);
    UIRender::UIElement startButtonPrsd("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "start_button_prsd.png",
        0.5f - uiPositions["pressed"].x * menuButtonScaleFactor, 0.5f + uiPositions["pressed"].x * menuButtonScaleFactor, buttonsPos[0] + uiPositions["pressed"].y * menuButtonScaleFactor, buttonsPos[0] - uiPositions["pressed"].y * menuButtonScaleFactor);
    UIRender::UIElement optionsNotPressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "options_button_nprsd.png",
        0.5f - uiPositions["npressed"].x * menuButtonScaleFactor, 0.5f + uiPositions["npressed"].x * menuButtonScaleFactor, buttonsPos[1] + uiPositions["npressed"].y * menuButtonScaleFactor, buttonsPos[1] - uiPositions["npressed"].y * menuButtonScaleFactor);
    UIRender::UIElement optionsPressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "options_button_prsd.png",
        0.5f - uiPositions["pressed"].x * menuButtonScaleFactor, 0.5f + uiPositions["pressed"].x * menuButtonScaleFactor, buttonsPos[1] + uiPositions["pressed"].y * menuButtonScaleFactor, buttonsPos[1] - uiPositions["pressed"].y * menuButtonScaleFactor);
    UIRender::UIElement exitNotPressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "exit_button_nprsd.png",
        0.5f - uiPositions["npressed"].x * menuButtonScaleFactor, 0.5f + uiPositions["npressed"].x * menuButtonScaleFactor, buttonsPos[2] + uiPositions["npressed"].y * menuButtonScaleFactor, buttonsPos[2] - uiPositions["npressed"].y * menuButtonScaleFactor);
    UIRender::UIElement exitPressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "exit_button_prsd.png",
        0.5f - uiPositions["pressed"].x * menuButtonScaleFactor, 0.5f + uiPositions["pressed"].x * menuButtonScaleFactor, buttonsPos[2] + uiPositions["pressed"].y * menuButtonScaleFactor, buttonsPos[2] - uiPositions["pressed"].y * menuButtonScaleFactor);
    UIRender::UIElement resumeNotPressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "resume_button_nprsd.png",
        0.5f - uiPositions["npressed"].x * menuButtonScaleFactor, 0.5f + uiPositions["npressed"].x * menuButtonScaleFactor, buttonsPos[0] + uiPositions["npressed"].y * menuButtonScaleFactor, buttonsPos[0] - uiPositions["npressed"].y * menuButtonScaleFactor);
    UIRender::UIElement resumePressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "resume_button_prsd.png",
        0.5f - uiPositions["pressed"].x * menuButtonScaleFactor, 0.5f + uiPositions["pressed"].x * menuButtonScaleFactor, buttonsPos[0] + uiPositions["pressed"].y * menuButtonScaleFactor, buttonsPos[0] - uiPositions["pressed"].y * menuButtonScaleFactor);
#pragma endregion
#pragma region Options
    UIRender::UIElement backNotPressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "back_button_nprsd.png",
        0.15f - uiPositions["npressed"].x * optionsButtonScaleFactor, 0.15f + uiPositions["npressed"].x * optionsButtonScaleFactor, buttonsPos[2] + uiPositions["npressed"].y * optionsButtonScaleFactor, buttonsPos[2] - uiPositions["npressed"].y * optionsButtonScaleFactor);//0.01, 0.2, 0.32, 0.13);
    UIRender::UIElement backPressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "back_button_prsd.png",
        0.15f - uiPositions["pressed"].x * optionsButtonScaleFactor, 0.15f + uiPositions["pressed"].x * optionsButtonScaleFactor, buttonsPos[2] + uiPositions["pressed"].y * optionsButtonScaleFactor, buttonsPos[2] - uiPositions["pressed"].y * optionsButtonScaleFactor);//0.01, 0.2, 0.32, 0.13);
    UIRender::UIElement creditsNotPressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "credits_button_nprsd.png",
        0.15f - uiPositions["npressed"].x * optionsButtonScaleFactor, 0.15f + uiPositions["npressed"].x * optionsButtonScaleFactor, buttonsPos[1] + uiPositions["npressed"].y * optionsButtonScaleFactor, buttonsPos[1] - uiPositions["npressed"].y * optionsButtonScaleFactor);//0.01, 0.2, 0.32, 0.13);
    UIRender::UIElement creditsPressed("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/button", "credits_button_prsd.png",
        0.15f - uiPositions["pressed"].x * optionsButtonScaleFactor, 0.15f + uiPositions["pressed"].x * optionsButtonScaleFactor, buttonsPos[1] + uiPositions["pressed"].y * optionsButtonScaleFactor, buttonsPos[1] - uiPositions["pressed"].y * optionsButtonScaleFactor);//0.01, 0.2, 0.32, 0.13);

    UIRender::UIElement optionsBackground("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures", "background.png",
        0.65f - 0.4f, 0.65f + 0.3f, 0.96, 0.35);

    UIRender::TextRender audio("./assets/shaders/text.vert", "./assets/shaders/text.frag", "./assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);
    UIRender::TextRender music("./assets/shaders/text.vert", "./assets/shaders/text.frag", "./assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);
    UIRender::TextRender gamma("./assets/shaders/text.vert", "./assets/shaders/text.frag", "./assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);
    UIRender::TextRender shadows("./assets/shaders/text.vert", "./assets/shaders/text.frag", "./assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);
    UIRender::TextRender instructions("./assets/shaders/text.vert", "./assets/shaders/text.frag", "./assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);

    // Sliders
    UIRender::UIElement audioSlider("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures", "slider.png",
        0.75f - 0.365f, 0.75f + 0.015f, 0.83 + 0.01 - 0.1, 0.83 - 0.01 - 0.1);
    UIRender::UIElement musicSlider("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures", "slider.png",
        0.75f - 0.365f, 0.75f + 0.015f, 0.73 + 0.01 - 0.1, 0.73 - 0.01 - 0.1);
    UIRender::UIElement gammaSlider("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures", "slider.png",
        0.75f - 0.365f, 0.75f + 0.015f, 0.63 + 0.01 - 0.1, 0.63 - 0.01 - 0.1);
#pragma endregion
#pragma region Credits
    UIRender::UIElement credits("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures", "credits.png",
        0.5f - 0.2f, 0.5f + 0.45f, 0.94, 0.19);
#pragma endregion
#pragma region StoryScene
    UIRender::UIElement story_00("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/story", "story_00.png", 0, 1, 1, 0);
    UIRender::UIElement story_01("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/story", "story_01.png", 0, 1, 1, 0);
#pragma endregion
#pragma region endScenes
    UIRender::UIElement winScene("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/story", "winScene.png", 0, 1, 1, 0);
    UIRender::UIElement lostScene("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/story", "lostScene.png", 0, 1, 1, 0);
#pragma endregion
#pragma region Clues
    UIRender::UIElement clues[4] = {
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/clues", "clue_00.png", 0.5 - 0.2, 0.5 + 0.2, 0.5 + 0.2, 0.5 - 0.2),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/clues", "clue_01.png", 0.5 - 0.2, 0.5 + 0.2, 0.5 + 0.2, 0.5 - 0.2),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/clues", "clue_02.png", 0.5 - 0.2, 0.5 + 0.2, 0.5 + 0.2, 0.5 - 0.2),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/clues", "boozeClue.png", 0.5 - 0.2, 0.5 + 0.2, 0.5 + 0.2, 0.5 - 0.2)
        //UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/clues", "clue_04.png", 0.5 - 0.1, 0.5 + 0.1, 0.5 + 0.2, 0.5 - 0.2)
    };
#pragma endregion
#pragma region Game
    /* Text init */
    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH), 0.0f, static_cast<GLfloat>(SCREEN_WIDTH));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "textProjection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    /* Create text */
    UIRender::TextRender points("./assets/shaders/text.vert", "./assets/shaders/text.frag", "./assets/fonts/medieval.ttf", SCREEN_WIDTH, SCREEN_HEIGHT);

    /* Animated dukat */
    UIRender::UIElement dukatSpinning[8] = {
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/dukat", "dukat_00.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/dukat", "dukat_01.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/dukat", "dukat_02.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/dukat", "dukat_03.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/dukat", "dukat_04.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/dukat", "dukat_05.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/dukat", "dukat_06.png", 0.01, 0.045, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/dukat", "dukat_07.png", 0.01, 0.045, 0.97, 0.91),
    };

    /* Arrows for chest opening */
    UIRender::UIElement arrows[4] = { 
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/keys", "key_0.png", 0.05, 0.089, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/keys", "key_1.png", 0.05, 0.089, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/keys", "key_2.png", 0.05, 0.089, 0.97, 0.91),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/keys", "key_3.png", 0.05, 0.089, 0.97, 0.91)
    };

    /* Health */
    float healthPosX = ((320 * 0.6f) / 1920) * 0.3f;
    float healthPosY = ((320 * 0.6f) / 1080) * 0.3f;
    UIRender::UIElement health[6] = {
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/health", "empty.png",
        0.4 - healthPosX, 0.4 + healthPosX, 0.9 + healthPosY, 0.9 - healthPosY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/health", "empty.png",
        0.5 - healthPosX, 0.5 + healthPosX, 0.9 + healthPosY, 0.9 - healthPosY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/health", "empty.png",
        0.6 - healthPosX, 0.6 + healthPosX, 0.9 + healthPosY, 0.9 - healthPosY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/health", "full.png",
        0.4 - healthPosX, 0.4 + healthPosX, 0.9 + healthPosY, 0.9 - healthPosY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/health", "full.png",
        0.5 - healthPosX, 0.5 + healthPosX, 0.9 + healthPosY, 0.9 - healthPosY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/health", "full.png",
        0.6 - healthPosX, 0.6 + healthPosX, 0.9 + healthPosY, 0.9 - healthPosY)
    };

    /* Players control */
    float playerControlsSizeX = ((120 * 0.5f) / 1920) * 0.55f;
    float playerControlsSizeY = ((120 * 0.5f) / 1080) * 0.55f;
    std::vector<glm::vec2> playerOneControlsPos = {
        {0.05f, 0.15f},     // A
        {0.08f, 0.15f},     // S
        {0.11f, 0.15f},     // D
        {0.08f, 0.21f},     // W
        {0.05f, 0.09f},    // V
        {0.08f, 0.09f}     // B
    };
    std::vector<glm::vec2> playerTwoControlsPos = {
        {1-0.05f, 0.15f},     // L
        {1-0.08f, 0.15f},     // K
        {1-0.11f, 0.15f},     // J
        {1-0.08f, 0.21f},     // I
        {1-0.05f, 0.09f},    // /
        {1-0.08f, 0.09f}     // .
    };

    UIRender::UIElement playerOneControls[6] = {
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "a.png",
        playerOneControlsPos[0].x - playerControlsSizeX, playerOneControlsPos[0].x + playerControlsSizeX, 
        playerOneControlsPos[0].y + playerControlsSizeY, playerOneControlsPos[0].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "s.png",
        playerOneControlsPos[1].x - playerControlsSizeX, playerOneControlsPos[1].x + playerControlsSizeX,
        playerOneControlsPos[1].y + playerControlsSizeY, playerOneControlsPos[1].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "d.png",
        playerOneControlsPos[2].x - playerControlsSizeX, playerOneControlsPos[2].x + playerControlsSizeX,
        playerOneControlsPos[2].y + playerControlsSizeY, playerOneControlsPos[2].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "w.png",
        playerOneControlsPos[3].x - playerControlsSizeX, playerOneControlsPos[3].x + playerControlsSizeX,
        playerOneControlsPos[3].y + playerControlsSizeY, playerOneControlsPos[3].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "v.png",
        playerOneControlsPos[4].x - playerControlsSizeX, playerOneControlsPos[4].x + playerControlsSizeX,
        playerOneControlsPos[4].y + playerControlsSizeY, playerOneControlsPos[4].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "b.png",
        playerOneControlsPos[5].x - playerControlsSizeX, playerOneControlsPos[5].x + playerControlsSizeX,
        playerOneControlsPos[5].y + playerControlsSizeY, playerOneControlsPos[5].y - playerControlsSizeY)
    };
    UIRender::UIElement playerTwoControls[6] = {
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "l.png",
        playerTwoControlsPos[0].x - playerControlsSizeX, playerTwoControlsPos[0].x + playerControlsSizeX,
        playerTwoControlsPos[0].y + playerControlsSizeY, playerTwoControlsPos[0].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "k.png",
        playerTwoControlsPos[1].x - playerControlsSizeX, playerTwoControlsPos[1].x + playerControlsSizeX,
        playerTwoControlsPos[1].y + playerControlsSizeY, playerTwoControlsPos[1].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "j.png",
        playerTwoControlsPos[2].x - playerControlsSizeX, playerTwoControlsPos[2].x + playerControlsSizeX,
        playerTwoControlsPos[2].y + playerControlsSizeY, playerTwoControlsPos[2].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "i.png",
        playerTwoControlsPos[3].x - playerControlsSizeX, playerTwoControlsPos[3].x + playerControlsSizeX,
        playerTwoControlsPos[3].y + playerControlsSizeY, playerTwoControlsPos[3].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "period.png",
        playerTwoControlsPos[5].x - playerControlsSizeX, playerTwoControlsPos[5].x + playerControlsSizeX,
        playerTwoControlsPos[5].y + playerControlsSizeY, playerTwoControlsPos[5].y - playerControlsSizeY),
        UIRender::UIElement("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures/controls", "slash.png",
        playerTwoControlsPos[4].x - playerControlsSizeX, playerTwoControlsPos[4].x + playerControlsSizeX,
        playerTwoControlsPos[4].y + playerControlsSizeY, playerTwoControlsPos[4].y - playerControlsSizeY)

    };

    int dukatSpinIndex = 0;
    float timeBetweenFrames = 0.10f;
#pragma endregion
#pragma endregion

#pragma region Proctors init
    GameLogic::Hierarchy hierarchy(&mainScene);
    hierarchy.setCamera(&camera);
    for (int i = 0; i < 6; i++)
        hierarchy.health[i] = &health[i];

    /* Create models library */
    Loader::ModelLibrary modelLibrary;

    /* Load models for not-serializable proctors */
    /* Boat */
    Loader::Model           boat_m("./assets/models/boat/bbot.fbx", "boat", true, true);
    GameLogic::Proctor      boat("boat", glm::vec3(709.0f, 2.2f, -164.5f), glm::quat(1.0f, 0.0f, 3.14f, 0.0f), glm::vec3(0.09f));
    GameLogic::MeshRenderer boat_mr(GameLogic::C_MESH, &boat, &boat_m, &model3D);
    GameLogic::Boat         boat_b(GameLogic::C_MOVEMENT, &boat);
    GameLogic::Interactable boat_inter(GameLogic::C_INTERACTABLE, &boat);
    GameLogic::BoxCollider  boat_bc(GameLogic::C_COLLIDER, &boat_m, &boat, &collisionBoxShader, false);
    hierarchy.addObject(&boat);
    /* Player One */
    Loader::Model           hero_00_m("./assets/models/players/blue1.fbx", "playerOne", true, true);
    GameLogic::Proctor      hero_00("playerOne", glm::vec3(621.332f, 6.5f, 25.568f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.012f));
    GameLogic::MeshRenderer hero_00_mr(GameLogic::C_MESH, &hero_00, &hero_00_m, &model3D);
    //GameLogic::Anima        hero_00_an(GameLogic::C_ANIMA, &hero_00, &hero_00_m, &model3D);
    GameLogic::PlayerInput  hero_00_pi(GameLogic::C_MOVEMENT, &hero_00, true, &boat_b);
    for (int i = 0; i < 6; i++)
        hero_00_pi.controls[i] = &playerOneControls[i];
    GameLogic::BoxCollider  hero_00_bc(GameLogic::C_COLLIDER, &hero_00_m, &hero_00, &collisionBoxShader, false);
    hierarchy.addObject(&hero_00);
    /* Player Two */
    Loader::Model           hero_01_m("./assets/models/players/red.fbx", "playerTwo", true, true);
    GameLogic::Proctor      hero_01("playerTwo", glm::vec3(625.963f, 6.5f, 25.425f), glm::quat(1.0f, glm::radians(0.0f), 0.0f, 0.0f), glm::vec3(0.012f));
    GameLogic::MeshRenderer hero_01_mr(GameLogic::C_MESH, &hero_01, &hero_01_m, &model3D);
    //GameLogic::Anima        hero_01_an(GameLogic::C_ANIMA, &hero_01);
    GameLogic::PlayerInput  hero_01_pi(GameLogic::C_MOVEMENT, &hero_01, false, &boat_b);
    for (int i = 0; i < 6; i++)
        hero_01_pi.controls[i] = &playerTwoControls[i];
    //hero_01_pi.setFootstepSound(footstepSource);
    GameLogic::BoxCollider  hero_01_bc(GameLogic::C_COLLIDER, &hero_01_m, &hero_01, &collisionBoxShader, false);
    hierarchy.addObject(&hero_01);
    // Kraken
    Loader::Model           monster_m("./assets/models/kraken/kraken.fbx", "enemy_00", true, true);
    GameLogic::Proctor      monster("monster", glm::vec3(703.0f, -200.0f, -130.0f), glm::quat(1.0f, 0.0f, 0.0f, glm::radians(180.0f)), glm::vec3(0.04f));
    GameLogic::MeshRenderer monster_mr(GameLogic::C_MESH, &monster, &monster_m, &model3D);
    hierarchy.addObject(&monster);

    hero_00_pi.setActive(false);
    hero_01_pi.setActive(false);
    boat_b.setActive(false);

    hero_00.setInitialTransform();
    hero_01.setInitialTransform();

    /* Create importer with given *.xml file */
    Loader::Importer importer("./assets/scenes/scene.xml", &model3D, false, 10.0f);
	
    /* Load models to hierarchy */
    int size = importer.importedProctors.size();
    int enemiesIterator = 0;
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
   		// Enemy
   		if(tmpCompoBooleanValues[5])
   		{
           importer.enemies.push_back(std::make_shared<GameLogic::Enemy>(
               importer.importedProctors.at(i).get(),
               &hero_00,
               &hero_01,
               importer.enemiesIslands[enemiesIterator++],
               importer.prepareModelName(importer.importedProctors.at(i).get()->name)
           ));
   		}
       hierarchy.addObject(importer.importedProctors.at(i).get());
   }
	
	/* Clear vector which is now unused */
	importer.enemiesIslands.clear();
	
    int cashSize = importer.cash.size();
#pragma endregion

#pragma region Environment
    /* Create Skybox */
    Skybox skybox(&view, &projection, &camera);

    /* Lights */
    DirLight dirLight(
        // 170.8f  /  -134.9f  /  212.0f 
        glm::vec3(0.0f, -26.0f, -0.2f),
        glm::vec4(0.45f, 0.45f, 0.45f, 1.0f),
        glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
        glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
        BoundingRegion(glm::vec3(-50.0f, -50.0f, 0.5f), glm::vec3(50.0f, 50.0f, 80.0f)),
        8000
    );

    /* Water and water's frame buffer */
    WaterMesh water("./assets/shaders/water.vert", "./assets/shaders/water.frag", "./assets/shaders/water.geom", 900, 3000);
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
#pragma endregion

#pragma region Monster System
    GameLogic::Proctor safe_zone_1("safe_zone_1", glm::vec3(703.0f, 0.0f, -130.0f));
    GameLogic::Proctor safe_zone_2("safe_zone_2", glm::vec3(701.0f, 0.0f, -380.0f));
    GameLogic::Proctor safe_zone_3("safe_zone_3", glm::vec3(430.0f, 0.0f, -927.0f));
    GameLogic::Proctor safe_zone_4("safe_zone_4", glm::vec3(-50.0f, 0.0f, -500.0f));

    std::vector<GameLogic::Proctor*> zones;
    zones.push_back(&safe_zone_1);
    zones.push_back(&safe_zone_2);
    zones.push_back(&safe_zone_3);
    zones.push_back(&safe_zone_4);
    Monster monsterSystem(&boat, zones,&water, &boat_b);
#pragma endregion

#pragma region Scene Manager
    Application::Scene sceneManager;
    sceneManager.changeCurrentScene("mainMenu");
    bool isResumeFlag = false;
#pragma endregion

#pragma region Audio
    /* Create sound engine */
    ISoundEngine* engine = createIrrKlangDevice();
    ISound* musicSource = engine->play2D("./assets/audio/music/shanty.ogg", true, false, true);
    ISound* waveSource = engine->play2D("./assets/audio/sounds/background.ogg", true, false, true);
    ISoundSource* bottleSource = engine->addSoundSourceFromFile("./assets/audio/sounds/bottle.ogg");
    ISoundSource* mainMenuSource = engine->addSoundSourceFromFile("./assets/audio/sounds/mainMenu.ogg");
    ISoundSource* heartBeatSource = engine->addSoundSourceFromFile("./assets/audio/sounds/beat.ogg");
#pragma endregion

#pragma region Load Options
    /* Create option variables */
    float gammaCorrection = 1.2f;
    bool isMusicPlaying = false;
    float audioValues = 1.00f;
    float musicValues = 0.15f;
    /* Normalization of audio values */
    int newGammaValue = normalizedGammaValue(gammaCorrection);
    int newAudioValue = normalizedAudioValue(audioValues);
    int newMusicValue = normalizedAudioValue(musicValues);

    int i = -1;
    std::string txtLine;
    std::ifstream optionsFile;
    optionsFile.open("./assets/optionsSave.txt");
    if (optionsFile.is_open())
    {
        while (std::getline(optionsFile, txtLine))
        {
            i++;
            if (i == 0)
                audioValues = std::stof(txtLine);
            else if (i == 1)
                musicValues = std::stof(txtLine);
            else if (i == 2)
                gammaCorrection = std::stof(txtLine);
            else if (i == 3)
                isMusicPlaying = std::stoi(txtLine);
        }
    }
    else
        std::cout << "Unable to open the file" << std::endl;
    optionsFile.close();

    /* Set loaded values */
    model3D.use();
    model3D.setUniformFloat("gamma", gammaCorrection);
    mainMenuSource->setDefaultVolume(audioValues);
    bottleSource->setDefaultVolume(audioValues);
    musicSource->setIsPaused(!isMusicPlaying);
    musicSource->setVolume(musicValues);
    waveSource->setVolume(audioValues);
#pragma endregion

#pragma region HealthSystem
#pragma endregion

    /* Render loop */
    while (!glfwWindowShouldClose(mainScene.window))
    {
        /* At first, update scene time */
        mainScene.update();
        frameCounter++;

        /* Dear ImGUI new frame setup */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* Enable/disable debug mode */
       /* if (keyboardInput->isKeyPressed(GLFW_KEY_TAB))
            isDebugModeOn = !isDebugModeOn;
        if (keyboardInput->isKeyPressed(GLFW_KEY_SPACE))
        {
            hierarchy.takeDamage();
        }*/

        /* SCENE LOADER */
        if (sceneManager.cActiveScene["mainMenu"])
        {
            if (keyboardInput->isKeyPressed(GLFW_KEY_S) || keyboardInput->isKeyPressed(GLFW_KEY_K))
            {
                tmpMainMenuIndex++;
                if (tmpMainMenuIndex > 2)
                    tmpMainMenuIndex = 0;
                engine->play2D(bottleSource, false);
            }            
            else if (keyboardInput->isKeyPressed(GLFW_KEY_W) || keyboardInput->isKeyPressed(GLFW_KEY_I))
            {
                tmpMainMenuIndex--;
                if (tmpMainMenuIndex < 0)
                    tmpMainMenuIndex = 2;
                engine->play2D(bottleSource, false);
            }

            /* Set camera variables */
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
            camera.setProjection(projection);
            camera.activeProctorsRadius = 710.0f;
            camera.Position = { 648.5f, 25.5f, -340.0f };
            camera.Pitch = -9.3f;
            camera.Yaw = -44.2f;

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
            dirLight.shadowFBO.unbind();
#pragma endregion

#pragma region WATER - ReflectionBuffer
            /* Enable cliiping */
            glEnable(GL_CLIP_DISTANCE0);
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

            /* Disable cliiping */
            glDisable(GL_CLIP_DISTANCE0);
#pragma endregion

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

            cameraSwitch(35, 60, 90, 45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01,&boat_b,&boat, &hero_00_pi, &hero_01_pi);

            /* Render light and update hierarchy */
            dirLight.render(model3D, 31);
            hierarchy.renderWithShader(&model3D);

            /* Render water */
            model = glm::translate(model, glm::vec3(-1100, waterYpos, -1100));
            water.render(model, projection, view, reflectBufferTex.id, mainScene.deltaTime, glm::vec3(camera.Position.x + 1100, camera.Position.y, camera.Position.z + 1100));

            skybox.render();
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

            for (int i = 0; i < 5; i++)
            {
                if (i == 0 || i == 2) continue;
                hero_00_pi.controls[i]->render();
                hero_01_pi.controls[i]->render();
            }
#pragma endregion
        }
        else if (sceneManager.cActiveScene["enterStory_00"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            story_00.render();
            points.render("Press V / ., to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 1.0f, 1.0f));
            if (keyboardInput->isKeyPressed(GLFW_KEY_V) || keyboardInput->isKeyPressed(GLFW_KEY_PERIOD))
                sceneManager.changeCurrentScene("enterStory_01");
        }
        else if (sceneManager.cActiveScene["enterStory_01"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            story_01.render();
            points.render("Press V / ., to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 1.0f, 1.0f));
            if (keyboardInput->isKeyPressed(GLFW_KEY_V) || keyboardInput->isKeyPressed(GLFW_KEY_PERIOD))
                sceneManager.changeCurrentScene("game");
            
        }
        else if (sceneManager.cActiveScene["options"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mainMenuSource->setDefaultVolume(audioValues);
            bottleSource->setDefaultVolume(audioValues);
            musicSource->setIsPaused(!isMusicPlaying);
            musicSource->setVolume(musicValues);
            waveSource->setVolume(audioValues);

            hero_00_pi.setActive(false);
            hero_01_pi.setActive(false);
            boat_b.setActive(false);

            /* Set camera variables */
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
            camera.setProjection(projection);
            if (!isResumeFlag)
            {
                camera.activeProctorsRadius = 710.0f;
                camera.Position = { 648.5f, 25.5f, -340.0f };
                camera.Pitch = -9.3f;
                camera.Yaw = -44.2f;
            }

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
            dirLight.shadowFBO.unbind();
#pragma endregion

#pragma region WATER - ReflectionBuffer
            /* Enable cliiping */
            glEnable(GL_CLIP_DISTANCE0);

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

            /* Disable cliiping */
            glDisable(GL_CLIP_DISTANCE0);
#pragma endregion

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
            cameraSwitch(35, 60, 90, 45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01, &boat_b, &boat, &hero_00_pi, &hero_01_pi);

            /* Render light and update hierarchy */
            dirLight.render(model3D, 31);
            hierarchy.renderWithShader(&model3D);

            /* Render water */
            model = glm::translate(model, glm::vec3(-1100, waterYpos, -1100));
            water.render(model, projection, view, reflectBufferTex.id, mainScene.deltaTime, glm::vec3(camera.Position.x + 1100, camera.Position.y, camera.Position.z + 1100));

            /* Render skybox */
            skybox.render();
#pragma endregion

            optionsBackground.render();

            audioSlider.render();
            musicSlider.render();
            gammaSlider.render();

            if (tmpOptionsMenuIndex != 0)
            {
                audio.render("Audio: ", SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.75, 1.5, glm::vec3(1.0, 0.0, 0.0));
                audio.render(std::to_string(newAudioValue), SCREEN_WIDTH * 0.65, SCREEN_HEIGHT * 0.75, 1.5, glm::vec3(1.0, 0.0, 0.0));
            }
            //audio.render("Audio: ", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.8, 1.5, glm::vec3(1.0, 0.0, 0.0));
            if (tmpOptionsMenuIndex != 1)
            {
                music.render("Music: ", SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.65, 1.5, glm::vec3(1.0, 0.0, 0.0));
                music.render(std::to_string(newMusicValue), SCREEN_WIDTH * 0.65, SCREEN_HEIGHT * 0.65, 1.5, glm::vec3(1.0, 0.0, 0.0));
            }
            if (tmpOptionsMenuIndex != 2)
            {
                gamma.render("Gamma: ", SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.55, 1.5, glm::vec3(1.0, 0.0, 0.0));
                gamma.render(std::to_string(newGammaValue), SCREEN_WIDTH * 0.65, SCREEN_HEIGHT * 0.55, 1.5, glm::vec3(1.0, 0.0, 0.0));
            }

            if (tmpOptionsMenuIndex != 3)
            {
                shadows.render("Music turned: ", SCREEN_WIDTH * 0.45, SCREEN_HEIGHT * 0.45, 1.5, glm::vec3(1.0, 0.0, 0.0));
                if (!isMusicPlaying)
                    shadows.render("OFF", SCREEN_WIDTH * 0.7, SCREEN_HEIGHT * 0.45, 1.5, glm::vec3(1.0, 0.0, 0.0));
                else
                    shadows.render("ON", SCREEN_WIDTH * 0.7, SCREEN_HEIGHT * 0.45, 1.5, glm::vec3(1.0, 0.0, 0.0));
            }

            if (tmpOptionsMenuIndex != 4);
            //creditsNotPressed.render();
            //credits.render("Credits", SCREEN_WIDTH * 0.01, SCREEN_HEIGHT * 0.4, 1.5, glm::vec3(1.0, 0.0, 0.0));

            if (keyboardInput->isKeyReleased(GLFW_KEY_S))
            {
                tmpOptionsMenuIndex++;
                if (tmpOptionsMenuIndex > 5)
                    tmpOptionsMenuIndex = 0;

                engine->play2D(bottleSource, false);
            }

            else if (keyboardInput->isKeyReleased(GLFW_KEY_W))
            {
                tmpOptionsMenuIndex--;
                if (tmpOptionsMenuIndex < 0)
                    tmpOptionsMenuIndex = 5;

                engine->play2D(bottleSource, false);
            }

            if (tmpOptionsMenuIndex == 0)
            {
                audio.render("Audio: ", SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.75, 1.5, glm::vec3(0.0, 1.0, 0.0));
                audio.render(std::to_string(newAudioValue), SCREEN_WIDTH * 0.65, SCREEN_HEIGHT * 0.75, 1.5, glm::vec3(0.0, 1.0, 0.0));
                if (keyboardInput->isKeyDown(GLFW_KEY_A))
                {
                    if (audioValues > 0)
                        audioValues -= 0.01f;
                }
                else if (keyboardInput->isKeyDown(GLFW_KEY_D))
                {
                    if (audioValues < 1)
                        audioValues += 0.01f;
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

            UIRender::UIElement audioPointer("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures", "notExists.png",
                (0.75f - 0.015f) + ((newAudioValue * 0.01f) * 0.35f) - 0.35f, (0.75f + 0.015f) + ((newAudioValue * 0.01f) * 0.35f) - 0.35f, 0.83 + 0.015 - 0.1, 0.83 - 0.015 - 0.1);
            audioPointer.render();

            if (tmpOptionsMenuIndex == 1)
            {
                music.render("Music: ", SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.65, 1.5, glm::vec3(0.0, 1.0, 0.0));
                music.render(std::to_string(newMusicValue), SCREEN_WIDTH * 0.65, SCREEN_HEIGHT * 0.65, 1.5, glm::vec3(0.0, 1.0, 0.0));
                if (keyboardInput->isKeyDown(GLFW_KEY_A))
                {
                    if (musicValues > 0)
                        musicValues -= 0.01f;
                }
                else if (keyboardInput->isKeyDown(GLFW_KEY_D))
                {
                    if (musicValues < 1)
                        musicValues += 0.01f;
                }
            }

            newMusicValue = normalizedAudioValue(musicValues);
            newAudioValue = normalizedAudioValue(audioValues);

            UIRender::UIElement musicPointer("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures", "notExists.png",
                (0.75f - 0.015f) + ((newMusicValue * 0.01f) * 0.35f) - 0.35f, (0.75f + 0.015f) + ((newMusicValue * 0.01f) * 0.35f) - 0.35f, 0.73 + 0.015 - 0.1, 0.73 - 0.015 - 0.1);
            musicPointer.render();

            if (tmpOptionsMenuIndex == 2)
            {
                gamma.render("Gamma: ", SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.55, 1.5, glm::vec3(0.0, 1.0, 0.0));
                gamma.render(std::to_string(newGammaValue), SCREEN_WIDTH * 0.65, SCREEN_HEIGHT * 0.55, 1.5, glm::vec3(0.0, 1.0, 0.0));
                if (keyboardInput->isKeyDown(GLFW_KEY_A))
                {
                    if (gammaCorrection > 0.6)
                        gammaCorrection -= 0.050f;
                }
                else if (keyboardInput->isKeyDown(GLFW_KEY_D))
                {
                    if (gammaCorrection < 2.5)
                        gammaCorrection += 0.050f;
                }
            }

            newGammaValue = normalizedGammaValue(gammaCorrection);
            model3D.use();
            model3D.setUniformFloat("gamma", gammaCorrection);

            UIRender::UIElement gammaPointer("./assets/shaders/ui.vert", "./assets/shaders/ui.frag", "./assets/textures", "noExists.png",
                0.2975 + (newGammaValue * 0.0175), 0.3275 + (newGammaValue * 0.0175), 0.63 + 0.015 - 0.1, 0.63 - 0.015 - 0.1);
            gammaPointer.render();

            if (tmpOptionsMenuIndex == 3)
            {
                shadows.render("Music turned: ", SCREEN_WIDTH * 0.45, SCREEN_HEIGHT * 0.45, 1.5, glm::vec3(0.0, 1.0, 0.0));
                if (!isMusicPlaying)
                    shadows.render("OFF", SCREEN_WIDTH * 0.7, SCREEN_HEIGHT * 0.45, 1.5, glm::vec3(0.0, 1.0, 0.0));
                else
                    shadows.render("ON", SCREEN_WIDTH * 0.7, SCREEN_HEIGHT * 0.45, 1.5, glm::vec3(0.0, 1.0, 0.0));

                if (keyboardInput->isKeyReleased(GLFW_KEY_A) || keyboardInput->isKeyReleased(GLFW_KEY_D))
                {
                    if (isMusicPlaying)
                        isMusicPlaying = false;
                    else
                        isMusicPlaying = true;
                }
            }

            if (isMusicPlaying)
                musicSource->setIsPaused(false);
            else
                musicSource->setIsPaused(true);

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
                    tmpOptionsMenuIndex = 0;
                    if (!isResumeFlag)
                        sceneManager.changeCurrentScene("mainMenu");
                    else
                        sceneManager.changeCurrentScene("resume");
                    engine->play2D(mainMenuSource, false);
                }
            }
            else
                backNotPressed.render();


        }
        else if (sceneManager.cActiveScene["credits"])
        {
            /* Set camera variables */
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
            camera.setProjection(projection);
            if (!isResumeFlag)
            {
                camera.activeProctorsRadius = 710.0f;
                camera.Position = { 648.5f, 25.5f, -340.0f };
                camera.Pitch = -9.3f;
                camera.Yaw = -44.2f;
            }

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
            dirLight.shadowFBO.unbind();
#pragma endregion

#pragma region WATER - ReflectionBuffer
            /* Enable cliiping */
            glEnable(GL_CLIP_DISTANCE0);
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

            /* Disable cliiping */
            glDisable(GL_CLIP_DISTANCE0);
#pragma endregion

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

            cameraSwitch(35, 60, 90, 45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01, &boat_b, &boat, &hero_00_pi, &hero_01_pi);

            /* Render light and update hierarchy */
            dirLight.render(model3D, 31);
            hierarchy.renderWithShader(&model3D);

            /* Render water */
            model = glm::translate(model, glm::vec3(-1100, waterYpos, -1300));
            water.render(model, projection, view, reflectBufferTex.id, mainScene.deltaTime, glm::vec3(camera.Position.x + 1100, camera.Position.y, camera.Position.z + 1100));

          
#pragma endregion

            credits.render();

            if (keyboardInput->isKeyReleased(GLFW_KEY_W))
                tmpCreditsIndex = 1;

            if (keyboardInput->isKeyReleased(GLFW_KEY_S))
                tmpCreditsIndex = 0;

            if (tmpCreditsIndex == 0)
            {
                backPressed.render();
                if (keyboardInput->isKeyReleased(GLFW_KEY_V) || keyboardInput->isKeyPressed(GLFW_KEY_PERIOD))
                {
                    tmpCreditsIndex = 1;
                    sceneManager.changeCurrentScene("options");
                    engine->play2D(mainMenuSource, false);
                }
            }
            else
                backNotPressed.render();
            }
        else if (sceneManager.cActiveScene["game"])
        {
            /* Check if player entered pause menu */
            if (keyboardInput->isKeyPressed(GLFW_KEY_ESCAPE))
            {
                isPaused = true;
                sceneManager.changeCurrentScene("resume");
                isResumeFlag = true;
                engine->play2D(mainMenuSource, false);
            }

            /* Set players variables */
            hero_00_pi.setActive(true);
            hero_01_pi.setActive(true);

            /* Set camera variables */
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
            camera.activeProctorsRadius = 150.0f;
            camera.setProjection(projection);

#pragma region SHADOWS - ShadowsBuffer
            /* Activate directional light's FBO */
            dirLight.shadowFBO.activate();

            depthShader.use();
            view = camera.GetViewMatrix();
            glm::mat4 lightView = glm::lookAt(-2.0f * dirLight.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            float offset = 0;
            if (boat_b.isPlayerOneInBoat() && boat_b.isPlayerTwoInBoat()) {
                offset = 50.0f;
            }
            glm::mat4 proj = glm::ortho(
                dirLight.br.min.x + camera.Position.x - offset,
                dirLight.br.max.x + camera.Position.x + offset,
                dirLight.br.min.y - camera.Position.z - offset,
                dirLight.br.max.y - camera.Position.z + offset,
                dirLight.br.min.z, 
                dirLight.br.max.z);
            dirLight.lightSpaceMatrix = proj * lightView;
            depthShader.setUniform("lightSpaceMatrix", dirLight.lightSpaceMatrix);
            dirLight.render(depthShader, 31);
            hierarchy.renderWithShader(&depthShader);
            dirLight.shadowFBO.unbind();
#pragma endregion

#pragma region WATER - ReflectionBuffer
            /* Enable clipping */
            glEnable(GL_CLIP_DISTANCE0);

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

            /* Disable cliiping */
            glDisable(GL_CLIP_DISTANCE0);
#pragma endregion

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
            cameraSwitch(40, 60, 90, 40, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01,&boat_b, &boat,&hero_00_pi, &hero_01_pi);

            /* Render light and update hierarchy */
            dirLight.render(model3D, 31);
            hierarchy.update(false, isDebugModeOn, collisionIncrement++); // need to be set this way, otherwise debug window won't appear

            /* Render water */
            model = glm::translate(model, glm::vec3(-1100, waterYpos, -1100));
            water.render(model, projection, view, reflectBufferTex.id, mainScene.deltaTime, glm::vec3(camera.Position.x + 1100, camera.Position.y, camera.Position.z + 1100));

            /* Update monster system */
            monsterSystem.update(engine, heartBeatSource, bottleSource, waveSource, &monster, isMusicPlaying);
            if (monsterSystem.isGameOver)
            {
                monsterSystem.isGameOver = false;
                water.waterColor = glm::vec3(0.4f, 0.6f, 0.9f);
                sceneManager.changeCurrentScene("exitStory_00"); //TODO reset main scene
            }
#pragma endregion

#pragma region UI Rendering
            /* Render text */
            points.render(std::to_string(Points::getInstance()->getScore()), SCREEN_WIDTH * 0.05, SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.08), 1.3, glm::vec3(1.0, 0.75, 0.0));

            /* Check if player picked up clue */
            if (hero_00_pi.isCluePickedUp)
            {
                if (hero_00_pi.clue == 0)
                    clues[0].render();
                if (hero_00_pi.clue == 1)
                    clues[1].render();
                if (hero_00_pi.clue == 2)
                    clues[2].render();
                if (hero_00_pi.clue == 3)
                    clues[3].render();
            }
          
            else if (hero_01_pi.isCluePickedUp)
            {
                if (hero_01_pi.clue == 0)
                    clues[0].render();
                if (hero_01_pi.clue == 1)
                    clues[1].render();
                if (hero_01_pi.clue == 2)
                    clues[2].render();
                if (hero_01_pi.clue == 3)
                    clues[3].render();
            }
                //points.render(hero_01_pi.clueText, SCREEN_WIDTH * 0.25, SCREEN_HEIGHT * 0.5, 2, glm::vec3(1.0, 0.0, 0.0));

            ///* TEST chest interaction*/
            //if (hero_00_pi.isPlayerOneUsingChest) 
            //{                
            //    if (hero_00_pi.timepassed == 0) {
            //        random = rand() % 3;
            //        //std::cout << random << "\n";
            //        hero_00_pi.buttonToPress = random;
            //    }
            //    arrows[random].render();
            //}   

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

            /* Render health */
            int size = 6;
            if (hierarchy.playerHealth == 2)    size = 5;
            if (hierarchy.playerHealth == 1)    size = 4;
            if (hierarchy.playerHealth <= 0)
            {
                sceneManager.changeCurrentScene("exitStory_00");
                hierarchy.playerHealth = 3;
            }
            for (int i = 0; i < size; i++)
            {
                hierarchy.health[i]->render();
            }

            /* Render controls */
            // Player One

            if (!hero_00_pi.isFirstClicked)
            {
                for (int i = 0; i < 4; i++)
                    hero_00_pi.controls[i]->render();

                instructions.render("MOVEMENT", 35, 89,1.0, glm::vec3(1.0, 1.0, 1.0));
            }

            if (hero_00_pi.stayCloseToInteractable)
            {
                hero_00_pi.controls[4]->render();
                instructions.render("USE", 133, 77, 1.0, glm::vec3(1.0, 1.0, 1.0));
            }
            if (hero_00_pi.isPlayerOneInBoat)
            {
                for (int i = 1; i < 6; i++)
                {
                    if (i == 2) continue;
                    hero_00_pi.controls[i]->render();
                }  
                instructions.render("DIRECTION", 15, 182, 0.4, glm::vec3(1.0, 1.0, 1.0));
                instructions.render("ROW", 2, 90, 0.4, glm::vec3(1.0, 1.0, 1.0));
                instructions.render("LEAVE", 195, 90, 0.4, glm::vec3(1.0, 1.0, 1.0));
            }

            // Player Two
            if (!hero_01_pi.isFirstClicked)
            {
                for (int i = 0; i < 4; i++)
                    hero_01_pi.controls[i]->render();
                instructions.render("MOVEMENT", 1635, 89, 1.0, glm::vec3(1.0, 1.0, 1.0));
            }
            if (hero_01_pi.stayCloseToInteractable)
            {
                hero_01_pi.controls[4]->render();
                instructions.render("USE", 1639, 77, 1.0, glm::vec3(1.0, 1.0, 1.0));
            }
            if (hero_01_pi.playerInAttackRadius) {
                hero_01_pi.controls[5]->render();
            }
            if (hero_01_pi.isPlayerTwoInBoat)
            {
                for (int i = 1; i < 6; i++)
                {
                    if (i == 2) continue;
                    hero_01_pi.controls[i]->render();
                    instructions.render("DIRECTION", 1800, 182, 0.4, glm::vec3(1.0, 1.0, 1.0));
                    instructions.render("ROW", 1678, 90, 0.4, glm::vec3(1.0, 1.0, 1.0));
                    instructions.render("LEAVE", 1856, 90, 0.4, glm::vec3(1.0, 1.0, 1.0));
                }                
            }


            if (hero_00_pi.isFinalChestOpen == true || hero_01_pi.isFinalChestOpen == true)
            {
                sceneManager.changeCurrentScene("exitStory_01");
                hero_00_pi.isFinalChestOpen = false;
                hero_01_pi.isFinalChestOpen = false;
            }
#pragma endregion
        }
        else if (sceneManager.cActiveScene["resume"])
        {
            if (keyboardInput->isKeyPressed(GLFW_KEY_S) || keyboardInput->isKeyPressed(GLFW_KEY_K))
            {
                tmpMainMenuIndex++;
                if (tmpMainMenuIndex > 2)
                    tmpMainMenuIndex = 0;
                engine->play2D(bottleSource, false);
            }
            else if (keyboardInput->isKeyPressed(GLFW_KEY_W) || keyboardInput->isKeyPressed(GLFW_KEY_I))
            {
                tmpMainMenuIndex--;
                if (tmpMainMenuIndex < 0)
                    tmpMainMenuIndex = 2;
                engine->play2D(bottleSource, false);
            }

            /* Set camera variables */
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);
            camera.setProjection(projection);

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
            dirLight.shadowFBO.unbind();
#pragma endregion

#pragma region WATER - ReflectionBuffer
            /* Enable cliiping */
            glEnable(GL_CLIP_DISTANCE0);
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

            /* Disable cliiping */
            glDisable(GL_CLIP_DISTANCE0);
#pragma endregion

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

            cameraSwitch(35, 60, 90, 45, mouseInput, keyboardInput, &mainScene, &hero_00, &hero_01, &boat_b, &boat, &hero_00_pi, &hero_01_pi);

            /* Render light and update hierarchy */
            dirLight.render(model3D, 31);
            hierarchy.renderWithShader(&model3D);

            /* Render water */
            model = glm::translate(model, glm::vec3(-1100, waterYpos, -1300));
            water.render(model, projection, view, reflectBufferTex.id, mainScene.deltaTime, glm::vec3(camera.Position.x + 1100, camera.Position.y, camera.Position.z + 1100));

            skybox.render();
#pragma endregion

#pragma region UI Elements
            /* Render title */
            logo.render();

            /* Resume button */
            if (tmpMainMenuIndex == 0)
            {
                resumePressed.render();
                if (keyboardInput->isKeyPressed(GLFW_KEY_V) || keyboardInput->isKeyPressed(GLFW_KEY_PERIOD))
                {
                    engine->play2D(bottleSource, false);
                    if (isPaused == true)
                    {
                        sceneManager.changeCurrentScene("game");
                        isResumeFlag = false;
                    }
                    else
                        sceneManager.changeCurrentScene("enterStory_00");
                }
            }
            else
                resumeNotPressed.render();

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
                    sceneManager.changeCurrentScene("mainMenu");
            }
            else
                exitNotPressed.render();

            for (int i = 0; i < 5; i++)
            {
                if (i == 0 || i == 2) continue;
                hero_00_pi.controls[i]->render();
                hero_01_pi.controls[i]->render();
            }
#pragma endregion
        }
        else if (sceneManager.cActiveScene["exitStory_00"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            lostScene.render();
            points.render("GOLD", SCREEN_WIDTH * 0.66, SCREEN_HEIGHT * 0.715, 3, glm::vec3(1.0, 0.75, 0.0));
            points.render(std::to_string(Points::getInstance()->getScore()), SCREEN_WIDTH * 0.63, SCREEN_HEIGHT * 0.57, 3, glm::vec3(1.0, 0.75, 0.0));
            points.render("Press V / ., to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 1.0f, 1.0f));
            if (keyboardInput->isKeyPressed(GLFW_KEY_V) || keyboardInput->isKeyPressed(GLFW_KEY_PERIOD))
            {
                restartFlag = true;
                sceneManager.changeCurrentScene("mainMenu");
            }
        }
        else if (sceneManager.cActiveScene["exitStory_01"])
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            winScene.render();
            points.render("GOLD", SCREEN_WIDTH * 0.66, SCREEN_HEIGHT * 0.715, 3, glm::vec3(1.0, 0.75, 0.0));
            points.render(std::to_string(Points::getInstance()->getScore()), SCREEN_WIDTH * 0.63, SCREEN_HEIGHT * 0.57, 3, glm::vec3(1.0, 0.75, 0.0));
            points.render("Press V / ., to continue...", 0, SCREEN_HEIGHT * 0.08, 1, glm::vec3(1.0f, 1.0f, 1.0f));
            if (keyboardInput->isKeyPressed(GLFW_KEY_V) || keyboardInput->isKeyPressed(GLFW_KEY_PERIOD))
            {
                sceneManager.changeCurrentScene("mainMenu");
                restartFlag = true;
            }
        }

        if (restartFlag)
        {
            restartFlag = false;
            if (hero_00_pi.isPlayerOneInBoat)
                boat_b.detachPlayerOne(&hero_00);
            else if (hero_00_pi.isPlayerTwoInBoat)
                boat_b.detachPlayerTwo(&hero_01);
            Points::getInstance()->setScore(0);
            unsigned int size = hierarchy.getProctors().size();
            for (int i = 0; i < size; i++)
                hierarchy.getProctors()[i]->activate();
            boat.activate();
            hero_00.activate();
            hero_01.activate();
            monster.activate();
        }

//#pragma region Debug Mode
//        if (isDebugModeOn)
//        {
//            ImGui::Begin("Camera and dir light");
//            {
//                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//                float variables[4];
//                /* CAMERA */
//                ImGui::Text("---- CAMERA ----");
//                // Position
//                variables[0] = camera.Position.x; variables[1] = camera.Position.y; variables[2] = camera.Position.z;
//                ImGui::DragFloat3("campos", variables);
//                camera.Position.x = variables[0]; camera.Position.y = variables[1]; camera.Position.z = variables[2];
//                // Pitch
//                variables[0] = camera.Pitch;
//                ImGui::DragFloat("pitch", variables);
//                camera.Pitch = variables[0];
//                // Yaw
//                variables[0] = camera.Yaw;
//                ImGui::DragFloat("yaw", variables);
//                camera.Yaw = variables[0];
//                ImGui::DragFloat("active radius:", &camera.activeProctorsRadius);
//                ImGui::Text("---- LIGHT -----");
//                /* Dir light */
//                // Direction
//                variables[0] = dirLight.direction.x; variables[1] = dirLight.direction.y; variables[2] = dirLight.direction.z;
//                ImGui::DragFloat3("direction", variables);
//                dirLight.direction.x = variables[0]; dirLight.direction.y = variables[1]; dirLight.direction.z = variables[2];
//                // Ambient
//                variables[0] = dirLight.ambient.x; variables[1] = dirLight.ambient.y; variables[2] = dirLight.ambient.z; variables[3] = dirLight.ambient.w;
//                ImGui::DragFloat4("ambient", variables);
//                dirLight.ambient.x = variables[0]; dirLight.ambient.y = variables[1]; dirLight.ambient.z = variables[2]; dirLight.ambient.w = variables[3];
//                // Diffuse
//                variables[0] = dirLight.diffuse.x; variables[1] = dirLight.diffuse.y; variables[2] = dirLight.diffuse.z; variables[3] = dirLight.diffuse.w;
//                ImGui::DragFloat4("diffuse", variables);
//                dirLight.diffuse.x = variables[0]; dirLight.diffuse.y = variables[1]; dirLight.diffuse.z = variables[2]; dirLight.diffuse.w = variables[3];
//                // Specular
//                variables[0] = dirLight.specular.x; variables[1] = dirLight.specular.y; variables[2] = dirLight.specular.z; variables[3] = dirLight.specular.w;
//                ImGui::DragFloat4("specular", variables);
//                dirLight.specular.x = variables[0]; dirLight.specular.y = variables[1]; dirLight.specular.z = variables[2]; dirLight.specular.w = variables[3];
//                // Bounding region min
//                variables[0] = dirLight.br.min.x; variables[1] = dirLight.br.min.y; variables[2] = dirLight.br.min.z;
//                ImGui::DragFloat3("br.min", variables);
//                dirLight.br.min.x = variables[0]; dirLight.br.min.y = variables[1]; dirLight.br.min.z = variables[2];
//                // Bounding region max
//                variables[0] = dirLight.br.max.x; variables[1] = dirLight.br.max.y; variables[2] = dirLight.br.max.z;
//                ImGui::DragFloat3("br.max", variables);
//                dirLight.br.max.x = variables[0]; dirLight.br.max.y = variables[1]; dirLight.br.max.z = variables[2];
//                // Gamma correction
//                ImGui::SliderFloat("gamma", &gammaCorrection, 0.0f, 3.0f);
//            }
//            ImGui::End();
//
//            if (keyboardInput->isKeyPressed(GLFW_KEY_Q))
//                sceneManager.changeCurrentScene("exitStory_00");
//            else if (keyboardInput->isKeyPressed(GLFW_KEY_E))
//                sceneManager.changeCurrentScene("exitStory_01");
//
//            collisionBoxShader.use();
//            collisionBoxShader.setUniform("projection", projection);
//            collisionBoxShader.setUniform("view", view);
//            collisionBoxShader.setUniformBool("collision", true);
//        }
//#pragma endregion

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
    //Loader::Exporter::exportScene(hierarchy.getProctors(), "./assets/scenes/exported_scene.xml");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    hierarchy.cleanup();

#pragma region Save Options
    std::ofstream optionsSave("./assets/optionsSave.txt");
    optionsSave << std::fixed << std::setprecision(2) << newAudioValue / 100.0f << std::endl;
    optionsSave << std::fixed << std::setprecision(2) << newMusicValue / 100.0f << std::endl;
    optionsSave << std::fixed << std::setprecision(1) << newGammaValue / 10.0f << std::endl;
    if (isMusicPlaying)
        optionsSave << 1 << std::endl;
    else
        optionsSave << 0 << std::endl;

    optionsSave.close();
#pragma endregion

    glfwTerminate();
    engine->drop();

    return 0;
}

int normalizedAudioValue(float _audioValue)
{
    int normalizedValue = _audioValue * 100;
    return normalizedValue;
}

int normalizedGammaValue(float _gammaValue)
{
    int normalizedValue = _gammaValue * 10;
    return normalizedValue;
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
    GameLogic::Boat * boat_b, GameLogic::Proctor* boat, GameLogic::PlayerInput* input_00, GameLogic::PlayerInput* input_01)
{
    //if (keyboardInput->isKeyPressed(GLFW_KEY_P)) {
    //    isDebugCameraOn = !isDebugCameraOn;
    //}

    //if (isDebugCameraOn)
    //{
    //    cameraKeyboardInput(_scene, keyboardInput);
    //    mouseOusideWindowsPos(GLFW_KEY_R, keyboardInput, mouseInput);

    //    if (keyboardInput->isKeyDown(GLFW_KEY_RIGHT_CONTROL))
    //    {
    //        cameraMouseInput(_scene->window, mouseInput);
    //        mouseInput->cursorDisable();
    //    }
    //    if (keyboardInput->isKeyReleased(GLFW_KEY_RIGHT_CONTROL))
    //    {
    //        firstMouse = true; // reset last saved mosue position
    //        mouseInput->cursorEnable();
    //    }

    //}
    if (!isDebugCameraOn)
    {       
        float xDistance = player_1->getWorldPosition()[0] - player_2->getWorldPosition()[0];
        float yDistance = player_1->getWorldPosition()[2] - player_2->getWorldPosition()[2];
        float distance = sqrt(xDistance * xDistance + yDistance * yDistance);
        int zOffset = 15;

        float camPos = minZoom + distance/(maxDistanceX)*(maxZoom-minZoom); //clamp
        camera.Yaw = -90; // set yaw to default value
        camera.Pitch = -70;
        if (boat_b->isPlayerOneInBoat() && boat_b->isPlayerTwoInBoat()) {
            camPos = maxZoom+minZoom;
            zOffset = 20;
        }
        camera.Position = glm::vec3((player_1->getWorldPosition()[0] + player_2->getWorldPosition()[0]) / 2.f, camPos, ((player_1->getWorldPosition()[2]+ + player_2->getWorldPosition()[2]) / 2.f + zOffset));
        mouseInput->cursorEnable();
        
        if (yDistance > -maxDistanceY* camPos/maxZoom)
        {
            input_00->yUpDistance = true;
            input_01->yUpDistance = true;
        }
        else
        {
            input_00->yUpDistance = false;
            input_01->yUpDistance = false;
        }

        if (yDistance < maxDistanceY * camPos/maxZoom)
        {
            input_00->yDownDistance = true;
            input_01->yDownDistance = true;
        }
        else
        {
            input_00->yDownDistance = false;
            input_01->yDownDistance = false;
        }

        if (xDistance < 70 * camPos / maxZoom)
        {
            input_00->xRightDistance = true;
            input_01->xRightDistance = true;
        }
        else
        {
            input_00->xRightDistance = false;
            input_01->xRightDistance = false;
        }

        if (xDistance > -70 * camPos / maxZoom)
        {
            input_00->xLeftDistance = true;
            input_01->xLeftDistance = true;
        }
        else
        {
            input_00->xLeftDistance = false;
            input_01->xLeftDistance = false;
        }
    }
}