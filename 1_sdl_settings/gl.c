#include <SDL.h>
#include <GL/gl.h>
 
int main()
{
    SDL_Window *win = NULL;
    SDL_GLContext context;

    SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("window", 2000, 0, 480, 800, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(win);

    SDL_bool isQuit = SDL_FALSE;
    while(!isQuit) {
        SDL_Event e;
        if(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                isQuit = SDL_TRUE;
            }
        }
        static const GLfloat red[] = {1.0f, 0.0f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, red);
        SDL_GL_SwapWindow(win);
    }
    SDL_Quit();
    return 0;
}

