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
    Shader instanceShaderProgram{};
    Shader shaderLight{};

    Camera camera;

    uint64_t deltaTime = 0;
    uint64_t lastFrame = 0;

    std::vector<glm::mat4> modelMatrices;

    Model rock;
    Model planet;
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
    instanceShaderProgram = Shader("instanceShader.vert", "instanceShader.frag");

    planet = Model("asset\\planet\\planet.obj");
    rock = Model("asset\\rock\\rock.obj");

    initTexture();
    initOffset();

    // configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    // set transformation matrices as an instance vertex attribute (with divisor 1)
    // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
    // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
    // -----------------------------------------------------------------------------------------------------------------------------------
    for (unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        unsigned int VAO = rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    return 1;
}

inline void Program::initTexture()
{
}

inline void Program::initOffset()
{
    unsigned int amount = 1'00'000;
    
    srand(SDL_GetTicks() / 1000); // initialize random seed	
    float radius = 150.0;
    float offset = 50.0f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.1f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices.push_back(model);
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

void lightHelper(Shader& shaderProgram, Camera& camera)
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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 1000.0f);

    lightHelper(shaderProgram, camera);

    shaderProgram.setValue("view", view);
    shaderProgram.setValue("projection", projection);

    //std::cout << "Got here\n";

    // draw planet
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    shaderProgram.setValue("model", model);
    planet.Draw(shaderProgram);
    

    // draw meteorites
    instanceShaderProgram.use();
    instanceShaderProgram.setValue("view", view);
    instanceShaderProgram.setValue("projection", projection);
    instanceShaderProgram.setValue("texture_diffuse1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
    for (unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        glBindVertexArray(rock.meshes[i].VAO);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, modelMatrices.size());
        glBindVertexArray(0);
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