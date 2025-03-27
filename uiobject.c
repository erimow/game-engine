//
//  Button.c
//  CSDL
//
//  Created by Erik Morris on 7/17/24.
//

#include "uiobject.h"

void Button_init(Button *button, float x, float y, float w, float h) {
  button->buttonInfo.x = x;
  button->buttonInfo.y = y;
  button->buttonInfo.w = w;
  button->buttonInfo.h = h;
  // Texture_init(&button->buttonTextToDisplay);
  Texture_init(&button->buttonTexture);
}
void Button_initAndLoad(Button *button, SDL_Renderer *renderer, float x,
                        float y, float w, float h, const char *buttonBackground,
                        TTF_Font *gFont, const char *buttonText,
                        unsigned int textLength, SDL_Color textColor) {
  button->buttonInfo.x = x;
  button->buttonInfo.y = y;
  button->buttonInfo.w = w;
  button->buttonInfo.h = h;
  button->text = (char *)buttonText;
  button->textColor = textColor;
  button->textLength = textLength;
  // Texture_init(&button->buttonTextToDisplay);
  Texture_init(&button->buttonTexture);
  if (!Texture_loadFromFile(&button->buttonTexture, renderer,
                            buttonBackground)) {
    printf("Failed to load buttonBackground!\n");
  }
  Texture_init_andLoadFromRenderedText(
      &button->buttonTextToDisplay, renderer, gFont, (SDL_FRect){x, y, w, h},
      button->text, textLength, button->textColor);
}
void Button_free(Button *button) {
  Texture_free(&button->buttonTexture);
  Texture_free(&button->buttonTextToDisplay);
}
void Button_setPosition(Button *button, float x, float y) {
  button->buttonInfo.x = x;
  button->buttonInfo.y = y;
}
bool Button_loadTextures(Button *button, SDL_Renderer *renderer,
                         const char *buttonBackground, const char *buttonText,
                         unsigned int textLength, TTF_Font *gFont,
                         SDL_Color textColor) {
  bool success = true;
  if (!Texture_loadFromFile(&button->buttonTexture, renderer,
                            buttonBackground)) {
    printf("Failed to load buttonBackground!\n");
    success = false;
  }
  button->text = (char *)buttonText;
  button->textColor = textColor;
  if (!Texture_loadFromRenderedText(&button->buttonTextToDisplay, renderer,
                                    gFont, buttonText, textLength, textColor)) {
    printf("Failed to load buttonText!\n");
    success = false;
  }

  return success;
}
bool Button_loadTexturesDebug(Button *button, SDL_Renderer *renderer,
                              const char *buttonBackground,
                              const char *buttonText, SDL_Color textColor) {
  bool success = true;
  if (!Texture_loadFromFile(&button->buttonTexture, renderer,
                            buttonBackground)) {
    printf("Failed to load buttonBackground!\n");
    success = false;
  }
  button->text = (char *)buttonText;
  button->textColor = textColor;
  // if (!Texture_loadFromRenderedText(&button->buttonTextToDisplay,renderer,
  // gFont, buttonText, textColor))
  // {
  //     printf("Failed to load buttonText!\n");
  //     success = false;
  // }

  return success;
}
void Button_render(Button *button, SDL_Renderer *renderer) {
  Texture_render(&button->buttonTexture, renderer, NULL, &button->buttonInfo,
                 0.0, NULL, SDL_FLIP_NONE);
  Texture_render(&button->buttonTextToDisplay, renderer, NULL,
                 &button->buttonInfo, 0.0, NULL, SDL_FLIP_NONE);
  // SDL_SetRenderDrawColor(renderer, button->textColor.r, button->textColor.g,
  // button->textColor.b, button->textColor.a); SDL_RenderDebugText(renderer,
  // button->buttonInfo.x, button->buttonInfo.y, button->text);
}
void Button_handleEvent(Button *button, SDL_Event *e) {
  if (e->type == SDL_EVENT_MOUSE_MOTION ||
      e->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
      e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    // Get mouse position
    float x, y;
    SDL_GetMouseState(&x, &y);
    // Check if mouse is in button
    bool inside = true;

    // Mouse is left of the button
    if (x < button->buttonInfo.x) {
      inside = false;
    }
    // Mouse is right of the button
    else if (x > button->buttonInfo.x + button->buttonInfo.w) {
      inside = false;
    }
    // Mouse above the button
    else if (y < button->buttonInfo.y) {
      inside = false;
    }
    // Mouse below the button
    else if (y > button->buttonInfo.y + button->buttonInfo.h) {
      inside = false;
    }
    // Mouse is outside button
    if (!inside) {
      Texture_setColor(&button->buttonTexture, 255, 255, 255);
    }
    // Mouse is inside button
    else {
      // Set mouse over sprite
      switch (e->type) {
      case SDL_EVENT_MOUSE_MOTION:
        Texture_setColor(&button->buttonTexture, 200, 200, 200);
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        Texture_setColor(&button->buttonTexture, 130, 130, 130);
        button->isButtPressed = !button->isButtPressed;
        // (button->isButtPressed) ? printf("Button true\n") : printf("Button
        // false\n");
        break;

      case SDL_EVENT_MOUSE_BUTTON_UP:
        Texture_setColor(&button->buttonTexture, 200, 200, 200);
        break;
      }
    }
  }
}


// SLIDER DEFINITIONS
void Slider_init(Slider *slider, SDL_FRect *lever,  SDL_Color leverColor,SDL_FRect* bar, SDL_Color barColor, int defaultVal, int minVal, int maxVal, bool isVert){
  slider->sliderLever = *lever;
  slider->leverColor = leverColor;
  slider->sliderBar = *bar;
  slider->barColor = barColor;
  slider->value = defaultVal;
  slider->min = minVal;
  slider->max = maxVal;
  slider->isVert = isVert;
  slider->stepLength =  (isVert) ?(slider->max-slider->min)/(slider->sliderBar.h-slider->sliderLever.h): (slider->max-slider->min)/(slider->sliderBar.w-slider->sliderLever.w);
  if (slider->value<minVal)
    slider->value = minVal;
  if (isVert)
  slider->sliderLever.y = (slider->sliderBar.y)+((slider->sliderBar.h-slider->sliderLever.h)/((float)(slider->max)/(slider->value)));
  else
  slider->sliderLever.x = slider->sliderBar.x+((slider->sliderBar.w-slider->sliderLever.w)/((float)(slider->max)/(slider->value)));
}
void Slider_free(Slider *slider){
//essentially an obsolete function at the moment
  slider->value = 0;
  slider->min = 0;
  slider->max = 0;
}
void Slider_render(Slider *slider, SDL_Renderer *renderer){
SDL_SetRenderDrawColor(renderer, slider->barColor.r, slider->barColor.g,slider->barColor.b,slider->barColor.a);
SDL_RenderFillRect(renderer, &slider->sliderBar);
SDL_SetRenderDrawColor(renderer, slider->leverColor.r, slider->leverColor.g,slider->leverColor.b,slider->leverColor.a);
SDL_RenderFillRect(renderer, &slider->sliderLever);
}
void Slider_handleEvenets(Slider *slider, SDL_Event *e){
if (e->type == SDL_EVENT_MOUSE_MOTION ||
      e->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
      e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    // Get mouse position
    float x, y;
    SDL_GetMouseState(&x, &y);
    // Check if mouse is in button
    bool inside = true;

    if (e->type==SDL_EVENT_MOUSE_MOTION){
      if (slider->isSliderBeingPressed){
          //move pos
         if (slider->isVert){
           slider->sliderLever.y=y-slider->whenHeldMousePos.y;
           if (slider->sliderLever.y<slider->sliderBar.y){
             slider->sliderLever.y=slider->sliderBar.y;
          } else if (slider->sliderLever.y+slider->sliderLever.h>slider->sliderBar.y+slider->sliderBar.h){
           slider->sliderLever.y = slider->sliderBar.y+slider->sliderBar.h-slider->sliderLever.h;
          }
        slider->value = slider->min + ((slider->sliderLever.y-slider->sliderBar.y)*slider->stepLength);
        } else{
           slider->sliderLever.x=x-slider->whenHeldMousePos.x;
           if (slider->sliderLever.x<slider->sliderBar.x){
             slider->sliderLever.x=slider->sliderBar.x;
          } else if (slider->sliderLever.x+slider->sliderLever.w>slider->sliderBar.x+slider->sliderBar.w){
           slider->sliderLever.x = slider->sliderBar.x+slider->sliderBar.w-slider->sliderLever.w;
          }
        slider->value = slider->min + ((slider->sliderLever.x-slider->sliderBar.x)*slider->stepLength);
        }
        }
    }
    if (e->type==SDL_EVENT_MOUSE_BUTTON_UP){
       if (slider->isSliderBeingPressed){
          slider->isSliderBeingPressed=false;
        }
    }

    // Mouse is left of the button
    if (x < slider->sliderLever.x) {
      inside = false;
    }
    // Mouse is right of the button
    else if (x > slider->sliderLever.x + slider->sliderLever.w) {
      inside = false;
    }
    // Mouse above the button
    else if (y < slider->sliderLever.y) {
      inside = false;
    }
    // Mouse below the button
    else if (y > slider->sliderLever.y + slider->sliderLever.h) {
      inside = false;
    }
    // Mouse is outside button
    if (!inside) {
      // Texture_setColor(&button->buttonTexture, 255, 255, 255);
      if (slider->isSliderBeingPressed){

      }
    }
    // Mouse is inside button
    else {
      // Set mouse over sprite
      switch (e->type) {
      case SDL_EVENT_MOUSE_MOTION:
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        // Texture_setColor(&button->buttonTexture, 130, 130, 130);
        slider->isSliderBeingPressed = true;
        slider->whenHeldMousePos = (SDL_FPoint){x-slider->sliderLever.x,y-slider->sliderLever.y};
        break;

      case SDL_EVENT_MOUSE_BUTTON_UP:

        break;
      }
    }
  }
}

