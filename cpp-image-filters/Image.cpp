#include "Image.h"

Image::Image(SDL_Renderer* renderer, const std::string& imagePath, int width, int height)
        : renderer(renderer), texture(nullptr), width(width), height(height) {
        SDL_Surface* imageSurface = IMG_Load(imagePath.c_str());
        if (!imageSurface) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao carregar a imagem: %s", IMG_GetError());
            return;
        }

        surface = imageSurface;
        originalSurface = imageSurface;
        texture = SDL_CreateTextureFromSurface(renderer, imageSurface);

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
    SDL_Surface* originalSurface = getOriginalSurface();
    setSurface(SDL_ConvertSurfaceFormat(originalSurface, SDL_PIXELFORMAT_RGBA8888, 0));
}

Uint32 Image::getPixel(SDL_Surface* surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* pixelAddress = static_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        return *pixelAddress;
    case 2:
        return *reinterpret_cast<Uint16*>(pixelAddress);
    case 3: {
        Uint8 r, g, b;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        r = pixelAddress[0];
        g = pixelAddress[1];
        b = pixelAddress[2];
#else
        r = pixelAddress[2];
        g = pixelAddress[1];
        b = pixelAddress[0];
#endif
        return SDL_MapRGB(surface->format, r, g, b);
    }
    case 4:
        return *reinterpret_cast<Uint32*>(pixelAddress);
    default:
        return 0; // Invalid format
    }
}



void Image::setPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* pixelAddress = static_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        *pixelAddress = static_cast<Uint8>(pixel);
        break;
    case 2:
        *reinterpret_cast<Uint16*>(pixelAddress) = static_cast<Uint16>(pixel);
        break;
    case 3: {
        Uint8 r, g, b;
        SDL_GetRGB(pixel, surface->format, &r, &g, &b);
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        pixelAddress[0] = r;
        pixelAddress[1] = g;
        pixelAddress[2] = b;
#else
        pixelAddress[2] = r;
        pixelAddress[1] = g;
        pixelAddress[0] = b;
#endif
        break;
    }
    case 4:
        *reinterpret_cast<Uint32*>(pixelAddress) = pixel;
        break;
    default:
        break; // Invalid format
    }
}

void Image::updateTexture() {
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid texture for updateTexture.");
        return;
    }

    SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture from surface: %s", SDL_GetError());
        return;
    }
}

void Image::applyBlurFilter() {
    if (surface) {
        int kernelSize = 3;
        float kernelData[] = { 0.111f, 0.111f, 0.111f,
                               0.111f, 0.111f, 0.111f,
                               0.111f, 0.111f, 0.111f };
        SDL_Surface* _surface = getOriginalSurface();
        SDL_Surface* blurredSurface = SDL_ConvertSurface(_surface, _surface->format, _surface->flags);
        if (!blurredSurface) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create blurred surface: %s", SDL_GetError());
            return;
        }

        for (int y = 1; y < _surface->h - 1; y++) {
            for (int x = 1; x < _surface->w - 1; x++) {
                float r = 0.0f, g = 0.0f, b = 0.0f;
                for (int ky = 0; ky < kernelSize; ky++) {
                    for (int kx = 0; kx < kernelSize; kx++) {
                        Uint32 pixel = getPixel(_surface, x + kx - 1, y + ky - 1);
                        Uint8 pr, pg, pb, pa;
                        SDL_GetRGBA(pixel, _surface->format, &pr, &pg, &pb, &pa);
                        float weight = kernelData[ky * kernelSize + kx];
                        r += pr * weight;
                        g += pg * weight;
                        b += pb * weight;
                    }
                }
                Uint32 blurredPixel = SDL_MapRGBA(_surface->format, static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 255);
                setPixel(blurredSurface, x, y, blurredPixel);
            }
        }

        setSurface(blurredSurface);
        SDL_FreeSurface(blurredSurface);
    }
}

void Image::applyEmbossFilter() {
    if (surface) {
        int kernelSize = 3;
        int kernelData[] = { -2, -1, 0,
                             -1, 1, 1,
                             0, 1, 2 };
        SDL_Surface* _surface = getOriginalSurface();
        SDL_Surface* filteredSurface = SDL_ConvertSurface(_surface, _surface->format, _surface->flags);
        if (!filteredSurface) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create filtered surface: %s", SDL_GetError());
            return;
        }

        for (int y = 1; y < _surface->h - 1; y++) {
            for (int x = 1; x < _surface->w - 1; x++) {
                int embossR = 0, embossG = 0, embossB = 0;
                for (int ky = 0; ky < kernelSize; ky++) {
                    for (int kx = 0; kx < kernelSize; kx++) {
                        Uint32 pixel = getPixel(_surface, x + kx - 1, y + ky - 1);
                        Uint8 pr, pg, pb, pa;
                        SDL_GetRGBA(pixel, _surface->format, &pr, &pg, &pb, &pa);
                        int weight = kernelData[ky * kernelSize + kx];
                        embossR += pr * weight;
                        embossG += pg * weight;
                        embossB += pb * weight;
                    }
                }
                Uint8 r = static_cast<Uint8>(std::max(0, std::min(255, embossR + 128)));
                Uint8 g = static_cast<Uint8>(std::max(0, std::min(255, embossG + 128)));
                Uint8 b = static_cast<Uint8>(std::max(0, std::min(255, embossB + 128)));

                Uint32 embossPixel = SDL_MapRGB(_surface->format, r, g, b);
                setPixel(filteredSurface, x, y, embossPixel);
            }
        }

        setSurface(filteredSurface);
        SDL_FreeSurface(filteredSurface);
    }
}

void Image::applySharpenFilter() {
    if (surface) {
        int kernelSize = 3;
        int kernelData[] = { 0, -1, 0,
                            -1, 5, -1,
                             0, -1, 0 };
        SDL_Surface* _surface = getOriginalSurface();
        SDL_Surface* filteredSurface = SDL_ConvertSurface(_surface, _surface->format, _surface->flags);
        if (!filteredSurface) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create filtered surface: %s", SDL_GetError());
            return;
        }

        for (int y = 1; y < _surface->h - 1; y++) {
            for (int x = 1; x < _surface->w - 1; x++) {
                int red = 0, green = 0, blue = 0;

                for (int ky = 0; ky < kernelSize; ky++) {
                    for (int kx = 0; kx < kernelSize; kx++) {
                        int pixelX = x + kx - 1;
                        int pixelY = y + ky - 1;

                        Uint32 pixel = getPixel(_surface, pixelX, pixelY);
                        Uint8 r, g, b;
                        SDL_GetRGB(pixel, _surface->format, &r, &g, &b);

                        int kernelValue = kernelData[ky * kernelSize + kx];
                        red += r * kernelValue;
                        green += g * kernelValue;
                        blue += b * kernelValue;
                    }
                }

                Uint8 clampedRed = static_cast<Uint8>(std::max(0, std::min(255, red)));
                Uint8 clampedGreen = static_cast<Uint8>(std::max(0, std::min(255, green)));
                Uint8 clampedBlue = static_cast<Uint8>(std::max(0, std::min(255, blue)));

                Uint32 sharpenedPixel = SDL_MapRGB(_surface->format, clampedRed, clampedGreen, clampedBlue);
                setPixel(filteredSurface, x, y, sharpenedPixel);
            }
        }

        setSurface(filteredSurface);
        SDL_FreeSurface(filteredSurface);
    }
}

SDL_Color Image::getPixelColor(int x, int y) const {
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid surface for getPixelColor.");
        return { 0, 0, 0, 0 };
    }

    SDL_PixelFormat* pixelFormat = surface->format;

    int offset = y * surface->pitch + x * pixelFormat->BytesPerPixel;

    Uint8* pixel = static_cast<Uint8*>(surface->pixels) + offset;

    SDL_Color color;
    SDL_GetRGBA(*reinterpret_cast<Uint32*>(pixel), pixelFormat, &color.r, &color.g, &color.b, &color.a);

    return color;
}

void Image::saveImage(const std::string& outputPath) const
{
    SDL_Surface* saveSurface = surface ? surface : originalSurface;

    if (!saveSurface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid surface for saveImage.");
        return;
    }

    int result = IMG_SavePNG(saveSurface, outputPath.c_str());
    if (result != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to save image to %s: %s", outputPath.c_str(), IMG_GetError());
    }
}

void Image::setPixelColor(int x, int y, const SDL_Color& color) {
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid surface for setPixelColor.");
        return;
    }

    SDL_PixelFormat* pixelFormat = surface->format;

    int offset = y * surface->pitch + x * pixelFormat->BytesPerPixel;

    Uint8* pixel = static_cast<Uint8*>(surface->pixels) + offset;

    Uint32 pixelValue = SDL_MapRGB(pixelFormat, color.r, color.g, color.b);
    *reinterpret_cast<Uint32*>(pixel) = pixelValue;
}