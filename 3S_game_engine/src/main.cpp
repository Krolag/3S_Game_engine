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
#include "UIElement/UIElement.h"
#include "Background/BackgroundImage.h"
#include "Light/Light.h"
#include "Text/TextRender.h"

/* Rework Work In Progress */
#include "Loader/Loader.h"
#include "InputSystem/InputSystem.h"

#include <iostream>
#include <map>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cameraMouseInput(GLFWwindow* window, InputSystem::MouseInput *mouse);
void cameraKeyboardInput(GLFWwindow* window, InputSystem::KeyboardInput *keyboard);
void mouseOusideWindowsPos(int key, InputSystem::KeyboardInput* keyboard, InputSystem::MouseInput* mouse);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

#pragma endregion


    Shader modelShader("assets/shaders/model3D.vert", "assets/shaders/model3D.frag");
	
    BackgroundImage background("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/wall.jpg");
    
#pragma region Text
    Shader textShader("assets/shaders/text.vert", "assets/shaders/text.frag");

    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "textProjection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    TextRender text("assets/shaders/text.vert", "assets/shaders/text.frag", "assets/fonts/arial.ttf");

#pragma endregion
    
    Skybox skybox(&view, &projection, &camera);

    InputSystem::MouseInput* mouseInput = new InputSystem::MouseInput(window);
    InputSystem::KeyboardInput* keyboardInput = new InputSystem::KeyboardInput(window);
    mouseInput->cursorEnable();
    
    /* Load models */
    Loader::Model m(glm::vec3(1.0f), glm::vec3(1.0f));
    Loader::Model troll(glm::vec3(-12.0f), glm::vec3(0.02f));
    m.loadModel("assets/models/backpack/backpack.obj");
    troll.loadModel("assets/models/lotr_troll/scene.gltf");

    /* Lights */
    DirLight dirLight = {
        glm::vec3(-0.2f, -1.0f, -0.3f), 
        glm::vec3(0.1f), 
        glm::vec3(0.4f),
        glm::vec3(0.75f)
    };

    // Creation of UI Element. The last four parameters in constructor are positions of left, right, bottom and top edges of UI face
    UIElement uiELement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures", "progressbar.jpg", 0.1f, 0.4f, 0.8f, 0.9f);

    /* Animated sprites */
    UIElement marioWalking[4] = {
        UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_00.png", 0.0f, 0.1f, 0.2f, 0.1f),
        UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_01.png", 0.0f, 0.1f, 0.2f, 0.1f),
        UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_00.png", 0.0f, 0.1f, 0.2f, 0.1f),
        UIElement("assets/shaders/ui.vert", "assets/shaders/ui.frag", "assets/textures/mario_walking", "mario_03.png", 0.0f, 0.1f, 0.2f, 0.1f)
    };
    int marioWalkingIndex = 0;
    float timeBetweenFrames = 0.15f;

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

        /* ImGUI window setup */
        {
            ImGui::Text("Troll");
        }

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //use input to move camera
        cameraMouseInput(window, mouseInput);
        cameraKeyboardInput(window, keyboardInput);
        mouseOusideWindowsPos(GLFW_KEY_R, keyboardInput, mouseInput);

        //background.render(); //-----turn off skybox before use

        glEnable(GL_DEPTH_TEST);
        modelShader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);//glm::mat4(1.0f);
        modelShader.setUniform("projection", projection);
        view = camera.GetViewMatrix(); //glm::mat4(1.0f);
        modelShader.setUniform("view", view);
        model = glm::mat4(1.0f);
        dirLight.render(modelShader);
        m.render(modelShader);
        troll.render(modelShader);

        /* Sky-box -- Must be rendered almost last, before hud */
        skybox.render(); 

        /* Animated image */
        marioWalking[marioWalkingIndex].render();
        timeBetweenFrames -= deltaTime;
        if (timeBetweenFrames <= 0)
        {
            marioWalkingIndex++;
            timeBetweenFrames = 0.15f;
        }
        if (marioWalkingIndex >= 4)
            marioWalkingIndex = 0;
        
        /* Static image -- Must be rendered last */
        uiELement.render();
        
        /* Change text color in time */
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue * 10) / 2.0f + 0.5f;
        
        text.render(textShader, "Fusrodah", SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f, 1.0f, glm::vec3(0.0f, greenValue, 0.0f));
        text.render(textShader, "KAMEHAME.................................................................", 0.0f, 0.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        text.render(textShader, "HAAA!", (float)SCR_WIDTH - 150.0f, 0.0f, 1.0f, glm::vec3(greenValue, 0.0f, 0.0f));
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        keyboardInput->update();
        mouseInput->update();
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m.cleanup();
    troll.cleanup();

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
    if (keyboard->isKeyDown(GLFW_KEY_W))
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keyboard->isKeyDown(GLFW_KEY_S))
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keyboard->isKeyDown(GLFW_KEY_A))
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keyboard->isKeyDown(GLFW_KEY_D))
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouseOusideWindowsPos(int key, InputSystem::KeyboardInput* keyboard, InputSystem::MouseInput* mouse)
{
    if (keyboard->isKeyDown(key) && !mouse->isCursorEntered()) {
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