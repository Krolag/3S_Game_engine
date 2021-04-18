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
#include "Primitives/Cube/Cube.h"
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
void keyboardMovementWSAD(float* positionData, Loader::Model* model, InputSystem::KeyboardInput* keyboard);
void keyboardMovementIJKL(float* positionData, Loader::Model* model, InputSystem::KeyboardInput* keyboard);

//Switch camera position(debug)
void cameraSwitch(bool &isDebugMode, int cameraPositionY,InputSystem::MouseInput *mouseInput, InputSystem::KeyboardInput *keyboardInput, GLFWwindow* window);

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

	/* Create Cube */
    Cube cube(0.5, &camera, &model, &view, &projection);
	
    /* Create InputSystem elements */
    InputSystem::MouseInput* mouseInput = new InputSystem::MouseInput(window);
    InputSystem::KeyboardInput* keyboardInput = new InputSystem::KeyboardInput(window);

    /* Create object hierarchy */
    Hierarchy hierarchy;

    /* Load models */
    positionOfIjklObject[0] = 12.0f;
    positionOfIjklObject[1] = -12.0f;
    positionOfIjklObject[2] = -12.0f;
    Loader::Model m(glm::vec3(positionOfIjklObject[0], positionOfIjklObject[1], positionOfIjklObject[2]), glm::vec3(0.02f));
    m.loadModel("assets/models/lotr_troll/scene.gltf");

    positionOfWsadObject[0] = -12.0f;
    positionOfWsadObject[1] = -12.0f;
    positionOfWsadObject[2] = -12.0f;
    Loader::Model trollModel(glm::vec3(positionOfWsadObject[0], positionOfWsadObject[1], positionOfWsadObject[2]), glm::vec3(0.02f));
    trollModel.loadModel("assets/models/lotr_troll/scene.gltf");

    /* Load hierarchy */
    Proctor troll("troll_name", 0, NULL);
    MeshRenderer trollMR(C_MESH, &troll);
    trollMR.setModel(&trollModel);
    trollMR.setShader(model3D);
    troll.addComponent(&trollMR);
    hierarchy.addObject(&troll);

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

        /* ImGUI window setup */
        {
            /*ImGui::Begin("Hierarchy");
            for (auto obj : hierarchy.getObjects())
            {
                ImGui::Button(obj.name.c_str());
            }
            ImGui::End();*/
        }

        /* Per-frame time logic */
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* Use InputSystem to move camera */
        /* Use 'P' to swicth between camera modes, CTRL to look around, 'R' to get cursor position*/
        cameraSwitch(isDebugModeOn, 40, mouseInput, keyboardInput, window);
              
        glEnable(GL_DEPTH_TEST);
        model3D.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);//glm::mat4(1.0f);
        model3D.setUniform("projection", projection);
        view = camera.GetViewMatrix(); //glm::mat4(1.0f);
        model3D.setUniform("view", view);
        model = glm::mat4(1.0f);
        dirLight.render(model3D);
        keyboardMovementIJKL(positionOfIjklObject, &m, keyboardInput);
        keyboardMovementWSAD(positionOfWsadObject, &trollModel, keyboardInput);
        trollModel.render(model3D);
        m.render(model3D);
        /* Render models */
        //troll.update();
        hierarchy.update();
        //cube.render();

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

    m.cleanup();
    trollModel.cleanup();

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

void keyboardMovementWSAD(float* positionData, Loader::Model* model, InputSystem::KeyboardInput* keyboard)
{
    if (keyboard->isKeyDown(GLFW_KEY_W))
    {
        positionData[2] -= 0.2;
        model->position = glm::vec3(positionData[0], positionData[1], positionData[2]);
    }

    if (keyboard->isKeyDown(GLFW_KEY_S))
    {
        positionData[2] += 0.2;
        model->position = glm::vec3(positionData[0], positionData[1], positionData[2]);
    }

    if (keyboard->isKeyDown(GLFW_KEY_A))
    {
        positionData[0] -= 0.2;
        model->position = glm::vec3(positionData[0], positionData[1], positionData[2]);
    }

    if (keyboard->isKeyDown(GLFW_KEY_D))
    {
        positionData[0] += 0.2;
        model->position = glm::vec3(positionData[0], positionData[1], positionData[2]);
    }
}

void keyboardMovementIJKL(float* positionData, Loader::Model* model, InputSystem::KeyboardInput* keyboard)
{
    if (keyboard->isKeyDown(GLFW_KEY_I))
    {
        positionData[2] -= 0.2;
        model->position = glm::vec3(positionData[0], positionData[1], positionData[2]);
    }
   
    if (keyboard->isKeyDown(GLFW_KEY_K))
    {
        positionData[2] += 0.2;
        model->position = glm::vec3(positionData[0], positionData[1], positionData[2]);
    }
    
    if (keyboard->isKeyDown(GLFW_KEY_J))
    {
        positionData[0] -= 0.2;
        model->position = glm::vec3(positionData[0], positionData[1], positionData[2]);
    }
   
    if (keyboard->isKeyDown(GLFW_KEY_L)) 
    {
        positionData[0] += 0.2;
        model->position = glm::vec3(positionData[0], positionData[1], positionData[2]);
    }
    
}

void cameraSwitch(bool &isDebugMode, int cameraPositionY, InputSystem::MouseInput *mouseInput, InputSystem::KeyboardInput *keyboardInput, GLFWwindow* window)
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
        camera.Position = glm::vec3((positionOfWsadObject[0] + positionOfIjklObject[0]) / 2.f, cameraPositionY, (positionOfWsadObject[2] + positionOfIjklObject[2]) / 2.f);
        mouseInput->cursorEnable();
    }
}
