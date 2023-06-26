#ifndef IMAGE_H
#define IMAGE_H

#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <string>
#include <vector>

class Image {
public:
    Image(SDL_Renderer* renderer, const std::string& imagePath, int width, int height);
    Image(SDL_Renderer* renderer, SDL_Surface* surface, int width, int height);
    ~Image();
    void render(int x, int y);
    void setSurface(SDL_Surface* surface);
    void resetSurface();

    SDL_Surface * getSurface() const;
    SDL_Surface* getOriginalSurface() const;
    SDL_Texture* getTexture() const;

    int getWidth();
    int getHeight();
    SDL_Renderer * getRenderer();

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Surface* surface;
    SDL_Surface* _surface;
    SDL_Surface* originalSurface;

    
    int width, height;
};
#endif /* IMAGE_H */

