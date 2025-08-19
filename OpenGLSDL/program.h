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
    float getDeltaTime();
private:
    SDL_GLContext gl_context{};
    SDL_Window* window{};
    Shader shaderProgram{};
    Shader shaderLight{};

    unsigned int diffuseMap;
    unsigned int specularMap;

    Camera camera;

    uint64_t deltaTime = 0;
    uint64_t lastFrame = 0;

    Model model;
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
    shaderLight = Shader("lightshader.vert", "lightshader.frag");

    initTexture();

    model = Model("asset\\backpack\\backpack.obj");

    return 1;
}

inline void Program::initTexture()
{
    glGenTextures(1, &diffuseMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("asset\\container2.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &specularMap);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    data = stbi_load("asset\\container2_specular.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Vertices data
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };


    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // Set VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Set Vertex Buffer Object
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // vertex attribute
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // EBO
    glBindVertexArray(0);


    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Camera
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

    model.Draw(shaderProgram);


    shaderProgram.use();

    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, (float)SDL_GetTicks() / 1000.0f * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    
    shaderProgram.setValue("model", model);
    shaderProgram.setValue("view", view);
    shaderProgram.setValue("projection", projection);

    shaderProgram.setValue("viewPos", camera.Position);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    shaderProgram.setValue("material.diffuse", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    shaderProgram.setValue("material.specular", 1);

    shaderProgram.setValue("material.shininess", 32.0f);

    //shaderProgram.setValue("light.position", lightPos);

    //shaderProgram.setValue("light.direction", -0.2f, -1.0f, -0.3f);

    glm::vec3 lightColor = glm::vec3(1.0f);
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

    // directional light
    shaderProgram.setValue("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shaderProgram.setValue("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shaderProgram.setValue("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    shaderProgram.setValue("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    shaderProgram.setValue("pointLights[0].position", pointLightPositions[0]);
    shaderProgram.setValue("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    shaderProgram.setValue("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    shaderProgram.setValue("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    shaderProgram.setValue("pointLights[0].constant", 1.0f);
    shaderProgram.setValue("pointLights[0].linear", 0.09f);
    shaderProgram.setValue("pointLights[0].quadratic", 0.032f);
    // point light 2
    shaderProgram.setValue("pointLights[1].position", pointLightPositions[1]);
    shaderProgram.setValue("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    shaderProgram.setValue("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    shaderProgram.setValue("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    shaderProgram.setValue("pointLights[1].constant", 1.0f);
    shaderProgram.setValue("pointLights[1].linear", 0.09f);
    shaderProgram.setValue("pointLights[1].quadratic", 0.032f);
    // point light 3
    shaderProgram.setValue("pointLights[2].position", pointLightPositions[2]);
    shaderProgram.setValue("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    shaderProgram.setValue("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    shaderProgram.setValue("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    shaderProgram.setValue("pointLights[2].constant", 1.0f);
    shaderProgram.setValue("pointLights[2].linear", 0.09f);
    shaderProgram.setValue("pointLights[2].quadratic", 0.032f);
    // point light 4
    shaderProgram.setValue("pointLights[3].position", pointLightPositions[3]);
    shaderProgram.setValue("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shaderProgram.setValue("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    shaderProgram.setValue("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    shaderProgram.setValue("pointLights[3].constant", 1.0f);
    shaderProgram.setValue("pointLights[3].linear", 0.09f);
    shaderProgram.setValue("pointLights[3].quadratic", 0.032f);
    // spotLight
    shaderProgram.setValue("spotLight.position", camera.Position);
    shaderProgram.setValue("spotLight.direction", camera.Front);
    shaderProgram.setValue("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shaderProgram.setValue("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    shaderProgram.setValue("spotLight.specular", 1.0f, 1.0f, 1.0f);
    shaderProgram.setValue("spotLight.constant", 1.0f);
    shaderProgram.setValue("spotLight.linear", 0.09f);
    shaderProgram.setValue("spotLight.quadratic", 0.032f);
    shaderProgram.setValue("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shaderProgram.setValue("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    // Draw
    glBindVertexArray(VAO);

    for (unsigned int i = 0; i < 10; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shaderProgram.setValue("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // don't forget to use the corresponding shader program first (to set the uniform)
    shaderLight.use();

    
    //model = glm::translate(model, lightPos);
    

    shaderLight.setValue("model", model);
    shaderLight.setValue("view", view);
    shaderLight.setValue("projection", projection);
    shaderLight.setValue("lightColor", lightColor);

    glBindVertexArray(lightVAO);
    for (unsigned int i = 0; i < 4; i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        shaderLight.setValue("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
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

    return gl_context;
}