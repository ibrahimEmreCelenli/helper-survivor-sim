

#include "SDL2/SDL.h"
#include "simulator.h"


extern SDL_bool done;

int main(int argc, char* argv[]) {
    /*initializes map*/
    init_map(40, 30);

    /*initializes window*/
    printf("initialize window\n");
    init_sdl_window(map);

    printf("draw map\n");
    /*draws initial map*/
    draw_map();
    
    survivor_generator(NULL);


    int n=6;

    pthread_t controllerId;
    
 
    pthread_create(&(controllerId),NULL,drone_controller,NULL); 
    

    
    /* repeat until window is closed */
    while (!done) {

        /*check user events: e.g. click to close x*/
        check_events();

        /* update model:survivors, drones etc. */
        survivor_generator(NULL);
       
        
        /*draws new updated map*/
        draw_map();
   
        SDL_Delay(1000); /*sleep(1);*/
  
    }
    pthread_join((controllerId),NULL);

   

    printf("quitting...\n");
    freemap();
    /*quit everything*/
    quit_all();
    return 0;
}
