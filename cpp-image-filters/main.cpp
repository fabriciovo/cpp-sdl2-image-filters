#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

bool isSimilarColor(SDL_Color color1, SDL_Color color2, int tolerance) {
    int redDiff = abs(color1.r - color2.r);
    int greenDiff = abs(color1.g - color2.g);
    int blueDiff = abs(color1.b - color2.b);
    return (redDiff <= tolerance) && (greenDiff <= tolerance) && (blueDiff <= tolerance);
}

Uint32 SDL_GetPixel(SDL_Surface* surface, int x, int y) {
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

void SDL_SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) {
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
            } else {
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

int main() {
    // Inicialização do SDL
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // Carregar imagem de primeiro plano (foreground) e imagem de fundo (background)
    SDL_Surface* foregroundImage = IMG_Load("/Users/fabriciovo/Documents/GitHub/image_chroma.png");
    SDL_Surface* backgroundImage = IMG_Load("/Users/fabriciovo/Documents/GitHub/fundo.png");

    // Criar uma nova superfície para a imagem de saída
    SDL_Surface* outputImage = SDL_CreateRGBSurface(0, foregroundImage->w, foregroundImage->h,
                                                    foregroundImage->format->BitsPerPixel,
                                                    foregroundImage->format->Rmask,
                                                    foregroundImage->format->Gmask,
                                                    foregroundImage->format->Bmask,
                                                    foregroundImage->format->Amask);

    // Definir a cor-chave e o critério de tolerância
    SDL_Color chromaKeyColor = { 0, 255, 0 }; // Cor verde é usada como exemplo
    int tolerance = 50; // Exemplo de critério de tolerância

    // Percorrer cada pixel da imagem de primeiro plano
    for (int y = 0; y < foregroundImage->h; y++) {
        for (int x = 0; x < foregroundImage->w; x++) {
            Uint32 foregroundPixel = SDL_GetPixel(foregroundImage, x, y);
            SDL_Color foregroundColor;
            SDL_GetRGB(foregroundPixel, foregroundImage->format, &foregroundColor.r, &foregroundColor.g, &foregroundColor.b);

            if (isSimilarColor(foregroundColor, chromaKeyColor, tolerance)) {
                Uint32 backgroundPixel = SDL_GetPixel(backgroundImage, x, y);
                SDL_SetPixel(outputImage, x, y, backgroundPixel);
            } else {
                SDL_SetPixel(outputImage, x, y, foregroundPixel);
            }
        }
    }

    // Salvar a imagem de saída
    IMG_SavePNG(outputImage, "/Users/fabriciovo/Documents/GitHub/output.png");

    // Liberar memória
    SDL_FreeSurface(foregroundImage);
    SDL_FreeSurface(backgroundImage);
    SDL_FreeSurface(outputImage);

    // Finalização do SDL
    IMG_Quit();
    SDL_Quit();

    return 0;
}
