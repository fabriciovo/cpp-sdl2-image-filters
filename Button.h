#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>

class Button {
public:
    Button(SDL_Renderer* renderer, TTF_Font* font, const std::string& buttonText, int x, int y, int width, int height);
    ~Button();
    
    
    bool isClicked();
    void handleEvent(SDL_Event& e);
    void render();

private:
    SDL_Renderer* renderer_;
    SDL_Texture* texture_;
    SDL_Rect buttonRect_;
    bool isClicked_;

    void createTextureFromText(TTF_Font* font, const std::string& text);
};

#endif // BUTTON_H
