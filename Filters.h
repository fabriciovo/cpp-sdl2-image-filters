#ifndef FILTERS_H
#define FILTERS_H

#include <SDL.h>
#include "Image.h"
class Filters
{
public:
	Filters();
	~Filters();

	SDL_Surface* createBackground(SDL_Surface* bkgSurface, SDL_Surface* imgSurface);

	void applyBlurFilter(int blurRadius, Image* image);


	bool isSimilarColor(SDL_Color color1, SDL_Color color2, int tolerance);

    Uint32 SDL_GetPixel(SDL_Surface* surface, int x, int y);
    

    void SDL_SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);

};

#endif 