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

    SDL_Surface * getSurface() const;
    SDL_Texture* getTexture() const;

    int getWidth();
    int getHeight();

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Surface* surface;

    
    int width, height;
};
#endif /* IMAGE_H */

