#include <iostream>

#include "program.h"
#define SDL_MAIN_USE_CALLBACKS 
#include "SDL3/SDL_main.h"

Program program;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) 
{
    if (program.init()) {
        return SDL_APP_CONTINUE;
    }
    else {
        std::cout << "Failed to initialize" << std::endl;
        return SDL_APP_FAILURE;
    }
}

SDL_AppResult SDL_AppIterate(void* appstate) 
{
    
    program.loop();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) 
{
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_RESIZED:
        program.resizeWindow();
        break;
    case SDL_EVENT_KEY_DOWN:
        return program.handleKey(event->key.scancode);
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    program.quit();
}