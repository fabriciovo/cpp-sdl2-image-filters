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
    void setPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
    void updateTexture();
    void applyBlurFilter();
    void applyEmbossFilter();
    void applySharpenFilter();

    SDL_Surface * getSurface() const;
    SDL_Surface* getOriginalSurface() const;
    SDL_Texture* getTexture() const;
    SDL_Renderer * getRenderer();
    SDL_Color getPixelColor(int x, int y) const;

    int getWidth();
    int getHeight();

    Uint32 getPixel(SDL_Surface* surface, int x, int y);

    void saveImage(const std::string& outputPath) const;
    void applyChromaKeyBackground(const SDL_Color& chromaColor, const std::string& backgroundImagePath);

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Surface* surface;
    SDL_Surface* originalSurface;

    
    int width, height;
};
#endif /* IMAGE_H */

