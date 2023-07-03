#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#include "Image.h"
#include "Button.h"
#include "Input.h"



bool isSimilarColor(SDL_Color color1, SDL_Color color2, int tolerance) {
    int redDiff = abs(color1.r - color2.r);
    int greenDiff = abs(color1.g - color2.g);
    int blueDiff = abs(color1.b - color2.b);
    return (redDiff <= tolerance) && (greenDiff <= tolerance) && (blueDiff <= tolerance);
}

bool colorWithinTolerance(const SDL_Color& color1, const SDL_Color& color2, int tolerance) {
    int redDiff = abs(color1.r - color2.r);
    int greenDiff = abs(color1.g - color2.g);
    int blueDiff = abs(color1.b - color2.b);

    return (redDiff <= tolerance && greenDiff <= tolerance && blueDiff <= tolerance);
}

void chromaKey(Image& foreground, const Image& background, const SDL_Color& chromaColor, int tolerance) {
    if (!foreground.getSurface() || !background.getSurface()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid surface for chromaKey.");
        return;
    }

    SDL_LockSurface(foreground.getSurface());
    SDL_LockSurface(background.getSurface());

    for (int y = 0; y < foreground.getSurface()->h; y++) {
        for (int x = 0; x < foreground.getSurface()->w; x++) {
            SDL_Color foregroundPixelColor = foreground.getPixelColor(x, y);

            if (colorWithinTolerance(foregroundPixelColor, chromaColor, tolerance)) {
                SDL_Color backgroundPixelColor = background.getPixelColor(x, y);
                foreground.setPixelColor(x, y, backgroundPixelColor);
            }
        }
    }

    SDL_UnlockSurface(foreground.getSurface());
    SDL_UnlockSurface(background.getSurface());
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

int main(int argc, char * argv[]) {
    static const int HEIGHT = 800;
    static const int WIDTH = 600;
    
    Input input = Input();
    
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao inicializar o SDL: %s", SDL_GetError());
        return 1;
    }
    
    
    if (TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao inicializar o SDL_ttf: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Filtros de imagem", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, HEIGHT, WIDTH, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("arial.ttf", 28);

    
    if (!window || !renderer || !font) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao criar janela, renderer ou carregar a fonte: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    //Image * backgroundImage = new Image(renderer, input.getBackgroundPath(), 200, 150);
    //Image * imageOriginal = new Image(renderer, input.getImagePath(), 200, 150);
    Image* outputOriginal = new Image(renderer, input.getImagePath(), 700, 500);



    if (input.getHasBackground() == "y") {
        Image* backgroundImage = new Image(renderer, input.getBackgroundPath(), 700, 500);

        if (!backgroundImage->getSurface()) {
         }


        SDL_Surface* backgroundImageSurface = backgroundImage->getSurface();
        SDL_Surface* outputOriginalSurface = outputOriginal->getSurface();
        SDL_Surface* outputImageWithBackground = SDL_CreateRGBSurface(0, outputOriginalSurface->w, outputOriginalSurface->h,
            backgroundImageSurface->format->BitsPerPixel,
            backgroundImageSurface->format->Rmask,
            backgroundImageSurface->format->Gmask,
            backgroundImageSurface->format->Bmask,
            backgroundImageSurface->format->Amask);

        SDL_Color chromaKeyColor = { 0, 255, 0 };
        int tolerance = 118;

        for (int y = 0; y < outputOriginalSurface->h; y++) {
            for (int x = 0; x < outputOriginalSurface->w; x++) {
                Uint32 originalImagePixel = SDL_GetPixel(outputOriginalSurface, x, y);
                SDL_Color originalImageColor;
                SDL_GetRGB(originalImagePixel, outputOriginalSurface->format, &originalImageColor.r, &originalImageColor.g, &originalImageColor.b);

                if (isSimilarColor(originalImageColor, chromaKeyColor, tolerance)) {
                    Uint32 backgroundPixel = SDL_GetPixel(backgroundImageSurface, x, y);
                    SDL_SetPixel(outputImageWithBackground, x, y, backgroundPixel);
                } else {
                    SDL_SetPixel(outputImageWithBackground, x, y, originalImagePixel);
                }
            }
        }
    
        outputOriginal = new Image(renderer, outputImageWithBackground, backgroundImageSurface->w, backgroundImageSurface->h);
    }

    Button * btnSaveImage = new Button(renderer, font, "Save Image", 570, 520, 200, 40);
    Button * btnAddBlur = new Button(renderer, font, "Blur", 100, 520, 100, 40);
    Button * btnAddSharpen = new Button(renderer, font, "Sharpen", 200, 520, 100, 40);
    Button * btnAddEmboss = new Button(renderer, font, "Emboss", 300, 520, 100, 40);
    Button* btnResetFilters = new Button(renderer, font, "Reset", 400, 520, 100, 40);

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            btnSaveImage->handleEvent(event);
            btnAddBlur->handleEvent(event);
            btnAddSharpen->handleEvent(event);
            btnAddEmboss->handleEvent(event);
            btnResetFilters->handleEvent(event);
        }

        SDL_RenderClear(renderer);

        if (btnSaveImage->isClicked()){
            if (IMG_SavePNG(outputOriginal->getSurface(), "output.png") != 0) {
                    printf("Failed to save image: %s\n", IMG_GetError());
                    IMG_Quit();
                    SDL_Quit();
                    return 1;
             }
        }
        if (btnAddBlur->isClicked()){
            //applyBlur(outputImage, 3);
            outputOriginal->applyBlurFilter();
        }
        if (btnAddEmboss->isClicked()){
            outputOriginal->applyEmbossFilter();
        }
        if (btnAddSharpen->isClicked()) {
            outputOriginal->applySharpenFilter();
        }
        if (btnResetFilters->isClicked()) {
            outputOriginal->resetSurface();
        }
        outputOriginal->render(0, 0);
        
        btnSaveImage->render();
        btnAddBlur->render();
        btnAddSharpen->render();
        btnAddEmboss->render();
        btnResetFilters->render();

        SDL_RenderPresent(renderer);
    }
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

