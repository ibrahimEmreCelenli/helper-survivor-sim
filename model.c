
 
#include "simulator.h"
#define MAX_SURIVOR_PER_CELL 3
#include <pthread.h>
#include "SDL2/SDL.h"
#include <unistd.h>
extern SDL_bool done;

pthread_mutex_t hodor= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pax= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gandalf= PTHREAD_MUTEX_INITIALIZER; 

int speed = 1;
int totaltime=0;


Map map;
int numberofcells = 0;
List *survivors;
List *drones;
List *helpedsurvivors;

struct tm* returnTime(void){
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    return info;
}

int calcDistance(Drone*d,Survivor*s){

    return sqrt(pow((d->coord.x - s->coord.x),2) + pow((d->coord.y - s->coord.y),2));

}

void init_map(int height, int width) {
    map.height = height;
    map.width = width;
    numberofcells = height * width;
    
    survivors = create_list(sizeof(Survivor), numberofcells * 10);
    drones= create_list(sizeof(Drone),10);
    helpedsurvivors = create_list(sizeof(Survivor), numberofcells * 100);
    
    /*pointer array*/
    
    
    map.cells = malloc(sizeof(MapCell *) * map.height);
    for (int j = 0; j < map.height; j++) {
        /*rows for each pointer*/
        map.cells[j] = malloc(sizeof(MapCell) * map.width);
    }

    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            map.cells[i][j].coord.x = i;
            map.cells[i][j].coord.y = j; /**/
            map.cells[i][j].survivors = create_list(sizeof(Survivor), 10);
        }
    }
    printf("height: %d, width:%d\n", map.height, map.width);
}
void freemap() {
    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            List *list = map.cells[i][j].survivors;
            list->destroy(list);
        }
        free(map.cells[i]);
    }
    free(map.cells);
}
Survivor *create_survivor(Coord *coord, char *info, struct tm *discovery_time) {
    Survivor *s = malloc(sizeof(Survivor));
    memset(s, 0, sizeof(Survivor));
    memcpy(&(s->discovery_time), discovery_time, sizeof(struct tm));
    strncpy(s->info, info, sizeof(s->info));
    memcpy(&(s->coord), coord, sizeof(Coord));
    s->status=0;
    //printf("survivor: %s\n", asctime(&s->discovery_time));
    //printf("%s\n\n", s->info);
    return s;
}

/*THREAD FUNCTION: generates random survivor
 */
void survivor_generator(void *args) {
    // generate random location
    if (map.cells != NULL) {
        time_t traw;
        struct tm t; /*used for localtime*/

        /*survivor info*/
        char info[5] = {'A' + (random() % 26), 
                        'A' + (random() % 26),
                        '0' + (random() % 9),
                        '0' + (random() % 9)}; 

        Coord coord = {random() % map.height, random() % map.width};

        time(&traw);
        localtime_r(&traw, &t);

        //printf("creating survivor...%s\n", asctime(&t));
        Survivor *s = create_survivor(&coord, info, &t);

        /*add to general list*/
        add(survivors, s);

        /*add to the list in the cell*/
        List *list = map.cells[coord.x][coord.y].survivors;
        add(list, s);
        s->status=0;
        //printf("survivor added, celllist-size:%d\n", list->number_of_elements);
    }
}


Drone *create_drone(Coord *coord, char *info, struct tm *stime) { 
    Drone* d;
    d=malloc(sizeof(Drone));
    d->coord=*coord;
    strncpy(d->info,info,30);
    d->numberofhelped=0;
    d->status=0;
    d->destination.x=0;
    d->destination.y=0; 
    d->stime=*stime;
    map.cells[coord->x][coord->y].isDrone=1;
    return d; 
    }

/** a drone delivers aid pack to survivor,
the survivor is marked as helped and removed*/
void help_survivor(Drone *d, Survivor *s) {
    pthread_mutex_lock(&pax);
    int removed=removedata(survivors,s);
    int cellremove = removedata(map.cells[s->coord.x][s->coord.y].survivors,s);
    pthread_mutex_unlock(&pax);
    
    //printf("cellremove:%i\n",cellremove);
    //printf("removd:%i\n",removed);
    
    s->helped_time=*returnTime();
    add(helpedsurvivors,s);
    d->numberofhelped++;
    d->status=0;
    printf("\n-----------------------------\nhelped at X:%i,y:%i\n",d->coord.x,d->coord.y);
    s->status=1;
    printf("\nhelpedtime+%i\n-----------------------------",((s->helped_time.tm_sec) - (s->discovery_time.tm_sec)+60)%60);
    
    pthread_mutex_lock(&gandalf);

    totaltime+=((s->helped_time.tm_sec) - (s->discovery_time.tm_sec)+60)%60;
    pthread_mutex_unlock(&gandalf);

    

}


/** moves(flies) drone on the map:
based on its speed it jumps cells toward its destination*/
void move_drone(Drone *drone) {

    if(drone->coord.x < drone->destination.x){
        while(drone->coord.x != drone->destination.x){
        map.cells[drone->coord.x][drone->coord.y].isDrone=0;

        drone->coord.x++;
        map.cells[drone->coord.x][drone->coord.y].isDrone=1;
        sleep(speed);
        }
    }

    else if(drone->coord.x > drone->destination.x){
        while(drone->coord.x != drone->destination.x){
        map.cells[drone->coord.x][drone->coord.y].isDrone=0;

        drone->coord.x--;
        map.cells[drone->coord.x][drone->coord.y].isDrone=1;

        sleep(speed);
        }
    }

    if(drone->coord.y < drone->destination.y){
       while(drone->coord.y != drone->destination.y){
            map.cells[drone->coord.x][drone->coord.y].isDrone=0;
            drone->coord.y++;
            map.cells[drone->coord.x][drone->coord.y].isDrone=1;
            sleep(speed);
        }
    }

    else if(drone->coord.y > drone->destination.y){
            while(drone->coord.y != drone->destination.y){
            map.cells[drone->coord.x][drone->coord.y].isDrone=0;
            drone->coord.y--;
            map.cells[drone->coord.x][drone->coord.y].isDrone=1;
            sleep(speed);
        }
    }
    //printf("\n\n\ndrone moved x:%i,y:%i\n\n\n",drone->coord.x,drone->coord.y);
    }

/*THREAD FUNCTION: simulates a drone: */
void *drone_runner(void *drone){
    Drone *d=(Drone*)drone;
    int smallest=100000000;
    Survivor *target=NULL;
    sleep(5);
    while(!done){
        pthread_mutex_lock(&hodor);
        
        int tmp=survivors->number_of_elements;
        for(int i=0;i<tmp;i++){
            
            void *temp1 = nth_element(survivors,&i);
            Survivor* temp2 = (Survivor*)temp1;
            if(temp2->status==0){
                if(calcDistance(d,temp2)<smallest/*&&calcDistance(d,temp2)!=0*/){
                    smallest = calcDistance(d,temp2);
                    target=temp2;
                    //printf("\ndistance \n%i",smallest);
            }}

        }
        if(target !=NULL&&target->status==0){
        printf("\ntarget acquired:%s\n",target->info);
        target->status=1;
        target->discovery_time=*returnTime();
        d->status=1;
        d->destination=target->coord;}
        pthread_mutex_unlock(&hodor);
        
        if(d->status==1){
        move_drone(d);
        help_survivor(d,target);
        }
        
        smallest=100000;
    }

    



}
/*THREAD FUNCTION: an AI that controls drones based on survivors*/
void* drone_controller(void *args){
    int n=6;

    pthread_t droneIds[n];
    
    for(int i=0;i<n;i++){
         Coord base;
        base.x=rand()%map.height;
        base.y=rand()%map.width;

        char empty[30]={0};

    
        Drone * d = create_drone(&base,empty,returnTime());
        add(drones,d);
        map.cells[base.x][base.y].isDrone=1;
        
        pthread_create(&(droneIds[i]),NULL,drone_runner,d);
    }
     for(int i=0;i<n;i++){    
        pthread_join((droneIds[i]),NULL);
    }
    //printf("avg:%i",totaltime/helpedsurvivors->number_of_elements);

}


