#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
 
int main(int argc, char* argv[])
{
    SDL_Surface *screen = NULL;
    SDL_Surface *image = NULL;
    const SDL_VideoInfo *videoInfo = NULL;
    
    /*-----------------------------------------------------------------*/

/*    if (argc != 2)
    {
        fprintf(stderr, "single argument ... name of image to display\n");
        return 1;
    }*/

    /*-----------------------------------------------------------------*/

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr, "SDL_Init failed - %s\n", SDL_GetError());
        return 1;
    }

    /*-----------------------------------------------------------------*/

    videoInfo = SDL_GetVideoInfo();

    if (videoInfo == 0)
    {
        fprintf(stderr, "SDL_GetVideoInfo failed - %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    /*-----------------------------------------------------------------*/

    screen = SDL_SetVideoMode(800,
			      720,
			      videoInfo->vfmt->BitsPerPixel,
			      SDL_SWSURFACE);
    
    if (!screen)
    {
	fprintf(stderr, "SetVideoMode failed - %s\n", SDL_GetError());
	SDL_FreeSurface(image);
	SDL_Quit();
	return 1;
    }

/*-----------------------------------------------------------------*/

    while(1){

		/* Assuming that an ADB connection has already been made outside of this code, we can get screenshot from Android and save as screencap.png in this directory */
		system("adb shell \"screencap -p | busybox uuencode -\" | uudecode > screencap.png");
		
		image = IMG_Load("screencap.png");

		//image = SDL_DisplayFormat(image);

		if (!image)
		{
			fprintf(stderr, "IMG_Load failed - %s\n", IMG_GetError());
			SDL_Quit();
			return 1;
		}

		/*-----------------------------------------------------------------*/

		SDL_BlitSurface(image, 0, screen, 0);

		//SDL_Flip(screen);

		SDL_FreeSurface(image);
	}

    SDL_Quit();

    return 0;
}
