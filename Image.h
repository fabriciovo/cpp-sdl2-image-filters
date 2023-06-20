#ifndef IMAGE_H
#define IMAGE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Image {
public:
    Image(SDL_Renderer* renderer, const std::string& imagePath, int width, int height);
    Image(SDL_Renderer* renderer, SDL_Surface* surface, int width, int height);
    ~Image();
    void render(int x, int y);
    SDL_Surface * getSurface() const;

    int getWidth();
    int getHeight();
private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Surface* surface;

    
    int width, height;
};
#endif /* IMAGE_H */

