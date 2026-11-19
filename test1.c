/*
 * A test code that will be the initial exploration of SDL
 * and get a simple rendering working for the initial game version
*/

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SDL_WINDOW_WIDTH    720
#define SDL_WINDOW_HEIGHT   480

#define STEP_RATE_IN_MILLISECONDS 10

/*Animals*/

#define ANIMAL_COUNT 5

typedef enum {
    ANIMAL_SWALLOW = 0U
} AnimalSpecies;

typedef struct {
    int x;
    int y;
    AnimalSpecies species;
} Animal;


/*Core Structs*/
typedef struct {
    Uint64 rand;
    Animal *animals;
} Game;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    Uint64 last_step;
    //App specific struct
    Game *game;
} AppState;

/*
* Game Content
*/

void init_animal(Animal *animal, Uint64 x, Uint64 y, AnimalSpecies species){
    animal->x = x;
    animal->y = y;
    animal->species = species;
}

/*
* Game Core
*/

void Render(void *appstate){
    int i;
    /*Set Vars*/
    AppState *as = (AppState *)appstate;
    SDL_Renderer *rend = as->renderer;
    Game *game = as->game;
    SDL_FRect r;

    /*WIPE*/
    SDL_SetRenderDrawColor(rend, 127, 127, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rend);

    /*BACKGROUND*/

    /*ENTITIES*/
    Animal *animals = game->animals;
    r.h = 5;
    r.w = 5;
    for(i=0; i<ANIMAL_COUNT; i++){
        r.x = animals[i].x;
        r.y = animals[i].y;
        SDL_RenderFillRect(rend,&r);
    }

    /*USER INTERFACE*/

    /*Update*/
    SDL_RenderPresent(rend);
}

void Step(Game *game){

}

void Init(Game *game){
    /*Create set of animals*/
    int i;
    game->animals = SDL_malloc(ANIMAL_COUNT * sizeof(Animal));
    for(i = 0; i<ANIMAL_COUNT; i++){
        init_animal(&(game->animals[i]),i*10,i*10,ANIMAL_SWALLOW);
    }
}

static SDL_AppResult Key_Event(Game *game, SDL_Keycode key_code){
    return SDL_APP_CONTINUE;
}



/*
 * Interupt Handling
 */

SDL_AppResult SDL_AppIterate(void *appstate){
    //prep vars
    AppState *as = (AppState *)appstate;
    Game * game = as->game;
    const Uint64 now = SDL_GetTicks();

    //run steps
    while ((now - as->last_step) >= STEP_RATE_IN_MILLISECONDS) {
        Step(game);
        as->last_step += STEP_RATE_IN_MILLISECONDS;
    }

    //Render
    Render(as);

    //Return
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){

    //Create App Info
    if (!SDL_SetAppMetadata("Example Snake game", "1.0", "com.example.Snake")) {
        return SDL_APP_FAILURE;
    }

    //Setting SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    //Create AppState
    AppState *as = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (!as) {
        return SDL_APP_FAILURE;
    }
    *appstate = as;

    //Create Window
    if (!SDL_CreateWindowAndRenderer("Test EcoSim", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &as->window, &as->renderer)) {
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(as->renderer, SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    //Initialize Game
    Init(as->game);

    //Initialize time
    as->last_step = SDL_GetTicks();
    
    //Return
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
    AppState *as = (AppState *)appstate;
    SDL_DestroyRenderer(as->renderer);
    SDL_DestroyWindow(as->window);
    SDL_free(as);
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}