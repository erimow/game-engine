//
//  Button.c
//  CSDL
//
//  Created by Erik Morris on 7/17/24.
//

#include "Button.h"

void Button_init(Button* button, float x, float y, float w, float h){
    button->buttonInfo.x = x;
    button->buttonInfo.y = y;
    button->buttonInfo.w = w;
    button->buttonInfo.h = h;
    Texture_init(&button->buttonTextToDisplay);
    Texture_init(&button->buttonTexture);
}
void Button_free(Button* button){
    Texture_free(&button->buttonTexture);
    Texture_free(&button->buttonTextToDisplay);
}
void Button_setPosition(Button* button, float x, float y){
    button->buttonInfo.x=x;
    button->buttonInfo.y=y;
}
bool Button_loadTextures(Button* button, SDL_Renderer* renderer, const char* buttonBackground, const char* buttonText, TTF_Font *gFont, SDL_Color textColor){
    bool success = true;
    if (!Texture_loadFromFile(&button->buttonTexture, renderer, buttonBackground))
    {
        printf("Failed to load buttonBackground!\n");
        success = false;
    }
    if (!Texture_loadFromRenderedText(&button->buttonTextToDisplay,renderer, gFont, buttonText, textColor))
    {
        printf("Failed to load buttonText!\n");
        success = false;
    }
     
    return success;
}
void Button_render(Button* button, SDL_Renderer* renderer){
    Texture_render(&button->buttonTexture, renderer, NULL, &button->buttonInfo, 0.0, NULL, SDL_FLIP_NONE);
    Texture_render(&button->buttonTextToDisplay, renderer, NULL, &button->buttonInfo, 0.0, NULL, SDL_FLIP_NONE);
}
void Button_handleEvent(Button* button, SDL_Event* e, bool* pressed){
    if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState( &x, &y );
        //Check if mouse is in button
        bool inside = true;

        //Mouse is left of the button
        if( x < button->buttonInfo.x )
        {
            inside = false;
        }
        //Mouse is right of the button
        else if( x > button->buttonInfo.x + button->buttonInfo.w)
        {
            inside = false;
        }
        //Mouse above the button
        else if( y < button->buttonInfo.y )
        {
            inside = false;
        }
        //Mouse below the button
        else if( y > button->buttonInfo.y + button->buttonInfo.h)
        {
            inside = false;
        }
        //Mouse is outside button
        if( !inside )
        {
            Texture_setColor(&button->buttonTexture, 255, 255, 255);
        }
        //Mouse is inside button
        else
        {
            //Set mouse over sprite
            switch( e->type )
            {
                case SDL_MOUSEMOTION:
                    Texture_setColor(&button->buttonTexture, 200, 200, 200);
                break;
            
                case SDL_MOUSEBUTTONDOWN:
                    Texture_setColor(&button->buttonTexture, 130, 130, 130);
                    *pressed = !*pressed;
                break;
                
                case SDL_MOUSEBUTTONUP:
                    Texture_setColor(&button->buttonTexture, 200, 200, 200);
                break;
            }
        }
    }
}
