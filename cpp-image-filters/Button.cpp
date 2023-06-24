#include "Button.h"

Button::Button(SDL_Renderer* renderer, TTF_Font* font, const std::string& buttonText, int x, int y, int width, int height)
    : renderer_(renderer), isClicked_(false)
{
    buttonRect_.x = x;
    buttonRect_.y = y;
    buttonRect_.w = width;
    buttonRect_.h = height;

    createTextureFromText(font, buttonText);
}

Button::~Button()
{
    SDL_DestroyTexture(texture_);
}

void Button::handleEvent(SDL_Event& e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (mouseX >= buttonRect_.x && mouseX <= buttonRect_.x + buttonRect_.w &&
            mouseY >= buttonRect_.y && mouseY <= buttonRect_.y + buttonRect_.h)
        {
            isClicked_ = true;
        }
    }
}


bool Button::isClicked()
{
    if (isClicked_)
    {
        isClicked_ = false;
        return true;
    }
    return false;
}

void Button::render()
{
    SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer_, &buttonRect_);

    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawRect(renderer_, &buttonRect_);

    SDL_RenderCopy(renderer_, texture_, NULL, &buttonRect_);
}

void Button::createTextureFromText(TTF_Font* font, const std::string& text)
{
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (textSurface == NULL)
    {
        printf("Failed to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    texture_ = SDL_CreateTextureFromSurface(renderer_, textSurface);
    if (texture_ == NULL)
    {
        printf("Failed to create texture from surface! SDL Error: %s\n", SDL_GetError());
        return;
    }

    SDL_FreeSurface(textSurface);
}
