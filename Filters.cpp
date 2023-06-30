#include "Filters.h"

Filters::Filters()
{
}

Filters::~Filters()
{
}

SDL_Surface * Filters::createBackground(SDL_Surface* bkgSurface, SDL_Surface* imgSurface)
{
    SDL_Surface* outputSurface = SDL_CreateRGBSurface(0, imgSurface->w, imgSurface->h,
        imgSurface->format->BitsPerPixel,
        imgSurface->format->Rmask,
        imgSurface->format->Gmask,
        imgSurface->format->Bmask,
        imgSurface->format->Amask);

    SDL_Color chromaKeyColor = { 0, 255, 0 };
    int tolerance = 100;

    for (int y = 0; y < imgSurface->h; y++) {
        for (int x = 0; x < imgSurface->w; x++) {
            Uint32 originalImagePixel = SDL_GetPixel(imgSurface, x, y);
            SDL_Color originalImageColor;
            SDL_GetRGB(originalImagePixel, imgSurface->format, &originalImageColor.r, &originalImageColor.g, &originalImageColor.b);

            if (isSimilarColor(originalImageColor, chromaKeyColor, tolerance)) {
                Uint32 backgroundPixel = SDL_GetPixel(bkgSurface, x, y);
                SDL_SetPixel(bkgSurface, x, y, backgroundPixel);
            }
            else {
                SDL_SetPixel(outputSurface, x, y, originalImagePixel);
            }
        }
    }

    return outputSurface;

}

void Filters::applyBlurFilter(int blurRadius, Image* image)
{ 
    // Cria uma cópia da superfície original
    SDL_Surface* originalSurface = image->getSurface();
    SDL_Surface* blurredSurface = SDL_ConvertSurface(originalSurface, originalSurface->format, originalSurface->flags);

    // Aplica o filtro de desfoque
    for (int y = 0; y < originalSurface->h; y++) {
        for (int x = 0; x < originalSurface->w; x++) {
            // Calcula a cor média dos pixels vizinhos dentro do raio de desfoque
            int redSum = 0, greenSum = 0, blueSum = 0, count = 0;

            for (int dy = -blurRadius; dy <= blurRadius; dy++) {
                for (int dx = -blurRadius; dx <= blurRadius; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Verifica se o pixel está dentro dos limites da imagem
                    if (nx >= 0 && nx < originalSurface->w && ny >= 0 && ny < originalSurface->h) {
                        Uint32 pixel = SDL_GetPixel(originalSurface, nx, ny);
                        SDL_Color color;
                        SDL_GetRGB(pixel, originalSurface->format, &color.r, &color.g, &color.b);

                        redSum += color.r;
                        greenSum += color.g;
                        blueSum += color.b;
                        count++;
                    }
                }
            }

            // Calcula a cor média
            Uint8 redAvg = redSum / count;
            Uint8 greenAvg = greenSum / count;
            Uint8 blueAvg = blueSum / count;

            // Define o pixel na superfície borrada com a cor média
            SDL_SetPixel(blurredSurface, x, y, SDL_MapRGB(blurredSurface->format, redAvg, greenAvg, blueAvg));
        }
    }

    // Atualiza a superfície da imagem com a superfície borrada
    image->setSurface(blurredSurface);

    // Libera a memória da superfície original
    SDL_FreeSurface(originalSurface);
}



bool Filters::isSimilarColor(SDL_Color color1, SDL_Color color2, int tolerance) {
    int redDiff = abs(color1.r - color2.r);
    int greenDiff = abs(color1.g - color2.g);
    int blueDiff = abs(color1.b - color2.b);
    return (redDiff <= tolerance) && (greenDiff <= tolerance) && (blueDiff <= tolerance);
}

Uint32 Filters::SDL_GetPixel(SDL_Surface* surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = static_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * bpp;
    switch (bpp) {
    case 1:
        return *p;
    case 2:
        return *reinterpret_cast<Uint16*>(p);
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *reinterpret_cast<Uint32*>(p);
    default:
        return 0;
    }
}

void Filters::SDL_SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = static_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * bpp;
    switch (bpp) {
    case 1:
        *p = pixel;
        break;
    case 2:
        *reinterpret_cast<Uint16*>(p) = pixel;
        break;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;
    case 4:
        *reinterpret_cast<Uint32*>(p) = pixel;
        break;
    }
}