
#include "SDL2/SDL.h"
#include "simulator.h"

#define CSIZE 20 


SDL_bool done = 1; /*controls the view loop*/
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Event event;

int windowheight = 0;
int windowwidth = 0;


int init_sdl_window(Map map) {
    windowheight = map.height * CSIZE; 
    windowwidth = map.width * CSIZE;
    printf("height: %d, width:%d\n", windowheight, windowwidth);
    printf("height: %d, width:%d\n", map.height, map.width);
  
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    window =
        SDL_CreateWindow("SDL Example",           
                         SDL_WINDOWPOS_UNDEFINED, 
                         SDL_WINDOWPOS_UNDEFINED, 
                         windowwidth,  
                         windowheight, 
                         0);           
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer != NULL) {
        SDL_SetWindowTitle(window, "Medeniyet Drone simulator");
        done = 0;
    } else {
        perror("renderer");
    }
    return done;
}


static void draw_rectangles() {

    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            SDL_Rect rect = {CSIZE * j, CSIZE * i, CSIZE, CSIZE};
            if(map.cells[i][j].isDrone==1){
                    
                SDL_SetRenderDrawColor(renderer, 214, 2, 112, SDL_ALPHA_OPAQUE);



            }
            else if (map.cells[i][j].survivors != NULL &&
                map.cells[i][j].survivors->number_of_elements != 0) {
                /*change color to black*/
				int r = map.cells[i][j].survivors->number_of_elements;
                r = (r%15);
				SDL_SetRenderDrawColor(renderer, 100+r*10, 255-r*10, 0, SDL_ALPHA_OPAQUE);
            } 
            
            
            
            else {
                /*change color to nonblack*/
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            }

            /*draw cell with the determined color*/
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

int draw_map() {
    draw_rectangles();
    /* draw lines for grid */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    /*draw horizontal*/
    for (int i = 0; i <= windowheight; i = i + CSIZE) {
        SDL_RenderDrawLine(renderer, 0, i, windowwidth, i);
    }
    /* draw lines for grid */
    SDL_SetRenderDrawColor(renderer, 255, 155, 155, SDL_ALPHA_OPAQUE);

    /* draw vertical */
    for (int j = 0; j <= windowwidth; j = j + CSIZE) {
        SDL_RenderDrawLine(renderer, j, 0, j, windowheight);
    }

    SDL_RenderPresent(renderer);

    return 0;
}


int check_events() {

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            done = 1;
        }
        if (event.type == SDL_KEYDOWN) {
            done = 1;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            done = 1;
        }
    }

    return done;
}


int quit_all() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();

    return 0;
}
