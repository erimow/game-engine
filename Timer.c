//
//  Timer.c
//  CSDL
//
//  Created by Erik Morris on 7/17/24.
//

#include "Timer.h"

void Timer_init(Timer* timer){
    timer->startTicks=0;
    timer->pausedTicks=0;
    timer->paused=false;
    timer->started=false;
}
void Timer_start(Timer* timer){
    timer->started=true;
    timer->paused=false;
    timer->startTicks=SDL_GetTicks();
    timer->pausedTicks=0;
}
void Timer_stop(Timer* timer){
    timer->started=false;
    timer->paused=false;
    timer->startTicks=0;
    timer->pausedTicks=0;
}
void Timer_pause(Timer* timer){
    if (timer->started && !timer->paused)
    {
        timer->paused=true;
        timer->pausedTicks=SDL_GetTicks()-timer->startTicks;
        timer->startTicks=0;
    }
}
void Timer_unpause(Timer* timer){
    if (timer->started && timer->paused)
    {
        timer->paused=false;
        timer->startTicks=SDL_GetTicks()-timer->pausedTicks;
        timer->pausedTicks=0;
    }
}
unsigned int Timer_getTicks(Timer* timer){
    unsigned int time = 0;
    if (timer->started)
    {
        if(timer->paused)
        {
            time = timer->pausedTicks;
        }
        else
        {
            time = SDL_GetTicks() - timer->startTicks;
        }
    }
    return time;
}
bool Timer_isStarted(Timer* timer){
    return timer->started;
}
bool Timer_isPaused(Timer* timer){
    return timer->paused && timer->started;
}
