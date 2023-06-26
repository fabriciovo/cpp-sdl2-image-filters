#include "Image.h"

Image::Image(SDL_Renderer* renderer, const std::string& imagePath, int width, int height)
        : renderer(renderer), texture(nullptr), width(width), height(height) {
        SDL_Surface* imageSurface = IMG_Load(imagePath.c_str());
        if (!imageSurface) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao carregar a imagem: %s", IMG_GetError());
            return;
        }

        texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
        surface = imageSurface;
        originalSurface = imageSurface;
        if (!texture) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao criar a textura da imagem: %s", SDL_GetError());
            SDL_FreeSurface(imageSurface);
            return;
        }
        
    }

Image::Image(SDL_Renderer* renderer, SDL_Surface* surface, int width, int height)
        : renderer(renderer), width(width), height(height)
    {
        this->texture = SDL_CreateTextureFromSurface(renderer, surface);
        this->surface = surface;
        this->originalSurface = surface;

    }

Image::~Image() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

SDL_Surface* Image::getSurface() const {
    return surface;
}

SDL_Surface* Image::getOriginalSurface() const {
    return originalSurface;
}

void Image::setSurface(SDL_Surface * newSurface) {
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    surface = newSurface;

    if (surface)
    {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture from surface: %s", SDL_GetError());
        }
    }
}

SDL_Texture* Image::getTexture() const {
    return texture;
}

void Image::render(int x, int y) {

    SDL_Rect destRect = { x, y, width, height };

    if (!texture || surface != getSurface())
    {
        if (texture)
        {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }

        texture = SDL_CreateTextureFromSurface(renderer, getSurface());
        if (!texture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture from surface: %s", SDL_GetError());
            return;
        }
    }

    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}

int Image::getWidth() {
    return this->width;
}

int Image::getHeight() {
    return this->height;
}


SDL_Renderer * Image::getRenderer() {
    return this->renderer;
}
void Image::resetSurface()
{
    if (originalSurface)
    {
        SDL_FreeSurface(surface);

        surface = SDL_ConvertSurfaceFormat(originalSurface, originalSurface->format->format, 0);
        if (!surface)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to convert original surface: %s", SDL_GetError());
            return;
        }

        if (texture)
        {
            SDL_DestroyTexture(texture);
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture from surface: %s", SDL_GetError());
            return;
        }
    }
}





