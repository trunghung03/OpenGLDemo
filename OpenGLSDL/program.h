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

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

SDL_Window* SDLInit();
SDL_GLContext OpenGLInit(SDL_Window* window);

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
    void initTexture();
    void initOffset();
    float getDeltaTime();
private:
    SDL_GLContext gl_context{};
    SDL_Window* window{};
    Shader shaderProgram{};
    Shader shaderLight{};

    Camera camera;

    uint64_t deltaTime = 0;
    uint64_t lastFrame = 0;

    glm::vec2 translations[100];
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

    initTexture();
    initOffset();

    return 1;
}

inline void Program::initTexture()
{
}

inline void Program::initOffset()
{
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation{};
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }
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

inline void Program::loop() 
{
    // Handle player input
    handleKey();

    // Background clear
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    shaderProgram.use();
    for (unsigned int i = 0; i < 100; i++)
    {
        std::string str("offsets[" + std::to_string(i) + "]");
        shaderProgram.setValue(str, translations[i]);
    }

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

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

    return gl_context;
}