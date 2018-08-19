#include <SDL.h>

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 800

typedef struct AppData {
  SDL_Window* window;
  SDL_Surface* window_surface;
  SDL_Surface* player_surface;
} AppData;

int
main(int argc, char **argv)
{
    AppData ad;
    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
       return -1;
    }
    ad.window = SDL_CreateWindow(NULL, 2000, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (ad.window == 0) {
        SDL_Quit();
        return -1;
    }
    ad.player_surface = SDL_LoadBMP("./test.bmp");
    SDL_SetColorKey(ad.player_surface, 1, SDL_MapRGB(ad.player_surface->format, 255, 0, 255));
    ad.window_surface = SDL_GetWindowSurface(ad.window);

    SDL_bool isQuit = SDL_FALSE;

    SDL_Rect rcSprite = {0, 0, 50, 50};
    SDL_Rect dstSprite = {0, 0};
    int player_idx = 0;
    int player_state = 0;
	
    int lastTime = SDL_GetTicks();
    int fpsLastTime = SDL_GetTicks();
    int frameCount = 0;
    const int FPS = 20;
    while(!isQuit) {
        SDL_Event event;
        if(SDL_PollEvent(&event)) {
            switch(event.type)
            {
            case SDL_QUIT:
                isQuit = SDL_TRUE;
            break;
            case SDL_KEYUP:
             switch(event.key.keysym.scancode)
             {
                 case SDL_SCANCODE_LEFT:
                 if(dstSprite.x >= 10)
                     dstSprite.x -= 10;
                 player_state = 0;
                 break;
                 case SDL_SCANCODE_RIGHT:
                 if(dstSprite.x <= WINDOW_WIDTH - 10 - 50)
                     dstSprite.x += 10;
                 player_state = 1;
                 break;
                 case SDL_SCANCODE_UP:
                 if(dstSprite.y>=10)
                     dstSprite.y -= 10;
                 player_state = 2;
                 break;
                 case SDL_SCANCODE_DOWN:
                 if(dstSprite.y <= WINDOW_HEIGHT - 10 - 50)
                    dstSprite.y  += 10;
                 player_state = 3;
                 break;
             }
            break;
            }
        }

        if ((SDL_GetTicks() - lastTime) > 1000 / FPS )
        {
            player_idx ++;
            if(player_idx  >= 5)
                player_idx  = 0;
        

            rcSprite.x = 50 * player_idx;
            rcSprite.y = 50 * player_state;

            SDL_FillRect(ad.window_surface, NULL, SDL_MapRGB(ad.window_surface->format, 230, 210, 60));
/           SDL_BlitSurface(ad.player_surface,	&rcSprite, ad.window_surface, &dstSprite);
//            SDL_BlitSurface(ad.player_surface, &rcSprite, ad.window_surface, NULL);
        
            SDL_UpdateWindowSurface(ad.window);
            lastTime = SDL_GetTicks();
            frameCount++;
        }
        if( (SDL_GetTicks() - fpsLastTime) > 1000 )
        {
            SDL_Log("fps : %d\n", frameCount);
            fpsLastTime = SDL_GetTicks();
            frameCount = 0;
        }
    }
    SDL_Quit();
    return 0;
}
