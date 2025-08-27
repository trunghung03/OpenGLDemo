#pragma once
#include <iostream>

#include "glad/glad.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "world.h"
#include "shape.h"
#include "skybox.h"
#include <random>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

SDL_Window* SDLInit();
SDL_GLContext OpenGLInit(SDL_Window* window);
int randomHelper(int min, int max);

class Program 
{
public:
    int init();
    void loop();
    void quit();
    int resizeWindow();
    void handleKey();
    void handleMouse(float, float);
    void handleMouse(float);
    float getDeltaTime();
private:
    SDL_GLContext gl_context{};
    SDL_Window* window{};
    Shader shaderProgram{};
    Shader shaderLight{};
    Shader skyboxShader{};

    Camera camera;

    uint64_t deltaTime = 0;
    uint64_t lastFrame = 0;

    Skybox skybox;
    World world;
    Model tree;

    std::vector<glm::mat4> treeLocations;
};

inline int Program::init()
{
    // Setup Application
    window = SDLInit();
    if (window == nullptr)
    {
        std::cout << "Failed to initialize SDL" << std::endl;
        return 0;
    }

    gl_context = OpenGLInit(window);
    if (gl_context == nullptr)
    {
        std::cout << "Failed to initialize OpenGL" << std::endl;
        return 0;
    }

    shaderProgram = Shader("shader.vert", "shader.frag");
    skyboxShader = Shader("skyboxShader.vert", "skyboxShader.frag");

    auto plane = std::make_unique<Shape>();
    glm::mat4 planeLocation = glm::mat4(1.0f);
    //planeLocation = glm::translate(planeLocation, glm::vec3(-250.0f, -2.0f, -250.0f));


    int planeWidth = 100;
    int planeHeight = 100;
    plane->generatePlane(100, 100, 0.1f);

    tree = Model("asset\\tree\\tree_oak.obj");

    
    for (int i = 0; i < 100; i++) {
        glm::mat4 treeLocation = glm::mat4(1.0f);
        glm::vec3 randPoint = plane->randomPoint();
        treeLocation = glm::translate(treeLocation, randPoint);
        treeLocations.push_back(treeLocation);
    }

    skybox = Skybox();
    skybox.generateSkybox();
    world.addObject(std::move(plane), planeLocation);
    

    return 1;
}

inline int Program::resizeWindow() 
{
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    std::cout << width << ' ' << height << "\n";
    glViewport(0, 0, width, height);
    return 1;
}

float Program::getDeltaTime()
{
    uint64_t currentFrame = SDL_GetTicks();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    return (float) deltaTime;
}

inline void Program::handleKey() 
{
    auto deltaTime = getDeltaTime();

    const bool* keystates = SDL_GetKeyboardState(nullptr);

    //SDL_assert(event->type == SDL_EVENT_KEY_DOWN); /* just checking key presses here... */
    if (keystates[SDL_SCANCODE_W]) {
        camera.processKeyboard(FORWARD, deltaTime);
    }
    if (keystates[SDL_SCANCODE_S]) {
        camera.processKeyboard(BACKWARD, deltaTime);
    }
    if (keystates[SDL_SCANCODE_A]) {
        camera.processKeyboard(LEFT, deltaTime);
    }
    if (keystates[SDL_SCANCODE_D]) {
        camera.processKeyboard(RIGHT, deltaTime);
    }
    if (keystates[SDL_SCANCODE_LSHIFT]) {
        camera.processKeyboard(DOWN, deltaTime);
    }
    if (keystates[SDL_SCANCODE_SPACE]) {
        camera.processKeyboard(UP, deltaTime);
    }
}

inline void Program::handleMouse(float xrel, float yrel)
{
    camera.processMouse(xrel, -yrel);
}

inline void Program::handleMouse(float y)
{
    camera.processMouseScroll(y);
}

void lightHelper(Shader& shaderProgram)
{
    // directional light
    shaderProgram.setValue("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shaderProgram.setValue("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shaderProgram.setValue("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    shaderProgram.setValue("dirLight.specular", 0.5f, 0.5f, 0.5f);
}

inline void Program::loop() 
{
    // Handle player input
    handleKey();

    // Background clear
    glClearColor(0.9f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 1000.0f);

    skyboxShader.use();
    glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.getViewMatrix()));
    skyboxShader.setValue("view", skyboxView);
    skyboxShader.setValue("projection", projection);

    skybox.Draw(skyboxShader);


    shaderProgram.use();

    shaderProgram.setValue("view", view);
    shaderProgram.setValue("projection", projection);

    lightHelper(shaderProgram);

    // draw main
    world.Draw(shaderProgram);

    for (auto&& treeLoc : treeLocations) {
        shaderProgram.setValue("model", treeLoc);
        tree.Draw(shaderProgram);
    }
    
    SDL_GL_SwapWindow(window);
}

inline void Program::quit() {
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static SDL_Window* SDLInit()
{
    // Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::cerr << "Error: " << SDL_GetError() << "\n";
        return nullptr;
    }
    // GL 3.3 + GLSL 130
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    SDL_Window* window = SDL_CreateWindow("OpenGL SDL", WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return nullptr;
    }

    // Cursor disable
    SDL_SetWindowRelativeMouseMode(window, true);

    return window;
}

static SDL_GLContext OpenGLInit(SDL_Window* window)
{
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    int version = gladLoadGL();
    if (version == 0)
    {
        printf("Failed to initialize OpenGL context\n");
        return nullptr;
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Property
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    return gl_context;
}