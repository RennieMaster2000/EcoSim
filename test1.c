/*
 * A test code that will be the initial exploration of SDL
 * and get a simple rendering working for the initial game version
*/

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

#define SDL_WINDOW_WIDTH    720
#define SDL_WINDOW_HEIGHT   480

#define STEP_RATE_IN_MILLISECONDS 10

/*Animals*/

typedef struct {
    int64_t vx;
    int64_t vy;
    int64_t alpha; //angle that is changed
    int64_t r; //amplitude that is changed
} Swallow_Extra;

#define ANIMAL_COUNT 5
#define ANIMAL_SIZE 16

typedef enum {
    ANIMAL_SWALLOW = 0U,
    ANIMAL_FALCON = 1U
} AnimalSpecies;

typedef struct {
    Uint64 x;
    Uint64 y;
    Uint64 newx;
    Uint64 newy;
    void* extra;
    AnimalSpecies species;
} Animal;

#define POSITION_FRACTION_BITS 16

#define FALCON_SPEED (2<<POSITION_FRACTION_BITS)


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
//Begin Sparrow
void Swallow_Init(Animal *animal, Game *game){
    animal->extra = malloc(sizeof(Swallow_Extra));
}
#define SWALLOW_ALPHA_SPEED
void Swallow_Step(Animal *animal, Game *game){
    Swallow_Extra *swinfo = (Swallow_Extra *)animal->extra;
    swinfo->alpha +=
}
//End Sparrow

void init_animal(Animal *animal, Uint64 x, Uint64 y, AnimalSpecies species){
    //SDL_Log("Initializing Animal");
    animal->x = x;
    animal->y = y;
    animal->newx = x;
    animal->newy = y;
    animal->species = species;
    //SDL_Log("Initialized Animal");
}


void Player_Step(Game *game){
    const bool *key_states = SDL_GetKeyboardState(NULL);

    int dx = 0;
    int dy = 0;
    if(key_states[SDL_SCANCODE_D]) dx++;
    if(key_states[SDL_SCANCODE_W]) dy--;
    if(key_states[SDL_SCANCODE_A]) dx--;
    if(key_states[SDL_SCANCODE_S]) dy++;
    
    game->animals[0].newx += (dy)?dx*FALCON_SPEED*0.7:dx*FALCON_SPEED;
    game->animals[0].newy +=(dx)?dy*FALCON_SPEED*0.7:dy*FALCON_SPEED;
}

void Loc_Update(Game *game){
    int i;
    for(i=0;i<ANIMAL_COUNT;i++){
        game->animals[i].x=game->animals[i].newx;
        game->animals[i].y=game->animals[i].newy;
    }
}

/*
* Game Core
*/


void Render(void *appstate){
    //SDL_Log("Render reached");
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
    r.h = ANIMAL_SIZE;
    r.w = ANIMAL_SIZE;
    for(i=0; i<ANIMAL_COUNT; i++){
        r.x = ((int64_t)animals[i].x)>>POSITION_FRACTION_BITS;
        r.y = ((int64_t)animals[i].y)>>POSITION_FRACTION_BITS;
        switch (animals[i].species){ //Color
            case ANIMAL_FALCON:
                SDL_SetRenderDrawColor(rend, 255, 230, 200, SDL_ALPHA_OPAQUE);
                break;
            case ANIMAL_SWALLOW:
                SDL_SetRenderDrawColor(rend, 255, 127, 0, SDL_ALPHA_OPAQUE);
                break;
            default:
                SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
                break;
        }
        SDL_RenderFillRect(rend,&r);
    }

    /*USER INTERFACE*/

    /*Update*/
    SDL_RenderPresent(rend);
}

void Step(Game *game){

    /*Player input*/
    Player_Step(game);
    /*Updating positions*/
    Loc_Update(game);
}

void Init(Game **gameloc){
    //SDL_Log("Init reached");
    *gameloc = SDL_malloc(sizeof(Game));
    Game *game = *gameloc;
    //SDL_Log("Game Initialized");

    /*Create set of animals*/
    int i;
    game->animals = SDL_malloc(ANIMAL_COUNT*sizeof(Animal));
    //SDL_Log("allocated succesfully");
    init_animal(&(game->animals[0]),0,0,ANIMAL_FALCON);
    for(i = 1; i<ANIMAL_COUNT; i++){
        init_animal(&(game->animals[i]),i*2*ANIMAL_SIZE<<POSITION_FRACTION_BITS,i*2*ANIMAL_SIZE<<POSITION_FRACTION_BITS,ANIMAL_SWALLOW);
    }
}

static SDL_AppResult Key_Event(Game *game, SDL_Keycode key_code){
    
    return SDL_APP_CONTINUE;
}



/*
 * Interupt Handling
 */

SDL_AppResult SDL_AppIterate(void *appstate){
    //SDL_Log("SDL Iteration");
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
    //SDL_Log("Goodday");
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
    Init(&(as->game));

    //Initialize time
    as->last_step = SDL_GetTicks();
    
    //Return
    //SDL_Log("SDL Initialized");
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