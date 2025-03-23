//
//  Timer.h
//  CSDL
//
//  Created by Erik Morris on 7/17/24.
//

#ifndef Timer_h
#define Timer_h

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct{
    unsigned int startTicks;
    unsigned int pausedTicks;
    bool paused;
    bool started;
}Timer;

void Timer_init(Timer* timer);
void Timer_start(Timer* timer);
void Timer_stop(Timer* timer);
void Timer_pause(Timer* timer);
void Timer_unpause(Timer* timer);
unsigned int Timer_getTicks(Timer* timer);
bool Timer_isStarted(Timer* timer);
bool Timer_isPaused(Timer* timer);


#endif /* Timer_h */
