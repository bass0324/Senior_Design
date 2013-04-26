#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include <pthread.h>
#include <stdio.h>

void *run_thread(void *threadid)
{
        if((file = popen("adb shell \"screencap -p | busybox uuencode -\" | uudecode","r"))==NULL)
	{
	    fprintf(stderr, "can't open dump.png\n");
	    return 0;
	}

	rw = SDL_RWFromFP(file, 0);

	close(file);
}
 
int main(int argc, char* argv[])
{
    SDL_Surface *screen = NULL;
    SDL_Surface *image = NULL;
    SDL_Surface *rot = NULL;
    SDL_RWops *rw = NULL;
    const SDL_VideoInfo *videoInfo = NULL;
    FILE* file;
    FILE* newFile;
    double scale;
    char buffer[256];
    int value;
    int prevValue;

    pthread_t thread0;    
    

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

    /*if((file = popen("adb shell \"screencap -p | busybox uuencode -\" | uudecode","r"))==NULL)
    {
        fprintf(stderr, "can't open dump.png\n");
        return 0;
    }

    rw = SDL_RWFromFP(file, 0);

    close(file);*/

    pthread_create(&threads0, NULL, run_thread, 0);

    image = IMG_LoadPNG_RW(rw);

    SDL_FreeRW(rw);

    //scale = (image->h)/(videoInfo->current_h);
    scale = 0.5;

    /*-----------------------------------------------------------------*/

    screen = SDL_SetVideoMode((image->w)*scale,
                              (image->h)*scale,
                              videoInfo->vfmt->BitsPerPixel,
                              SDL_SWSURFACE|SDL_DOUBLEBUF);

    if (!screen)
    {
        fprintf(stderr, "SetVideoMode failed - %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
    }

    rot = rotozoomSurface( image, 0, scale, SMOOTHING_ON );

    if (!image)
    {
        fprintf(stderr, "IMG_LoadBMP_RW failed - %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_BlitSurface(rot, 0, screen, 0);

    SDL_Flip(screen);

    SDL_FreeSurface(rot);
    SDL_FreeSurface(image);

    /*-----------------------------------------------------------------*/
while(0){
    if((file = popen("adb shell dumpsys window | grep cur= | awk -F cur= '{print $NF}' | awk -F ' ' '{print $1}' | awk -F 'x' '{print $NF}'", "r"))==NULL)
    {
        fprintf(stderr, "can't get size\n");
        return 0;
    }
    while(fgets(buffer, sizeof(buffer), file) != 0)
    {
	value = atoi(buffer);
    }
    
    pclose(file);

    if((file = popen("adb shell \"screencap -p | busybox uuencode -\" | uudecode","r"))==NULL)
    {
        fprintf(stderr, "can't open dump.png\n");
        return 0;
    }

    rw = SDL_RWFromFP(file, 0);

    close(file);

    image = IMG_LoadPNG_RW(rw);

    SDL_FreeRW(rw);

    if (value != image->h) {
	if (prevValue != image->h) {
	    screen = SDL_SetVideoMode((image->h)*scale,
		                      (image->w)*scale,
		                      videoInfo->vfmt->BitsPerPixel,
		                      SDL_SWSURFACE|SDL_DOUBLEBUF);
	}
        rot = rotozoomSurface( image, 270, scale, SMOOTHING_ON );
    }

    if (value == image->h) {
	if (prevValue != image->h) {
	    screen = SDL_SetVideoMode((image->w)*scale,
		                      (image->h)*scale,
		                      videoInfo->vfmt->BitsPerPixel,
		                      SDL_SWSURFACE|SDL_DOUBLEBUF);
	}
        rot = rotozoomSurface( image, 0, scale, SMOOTHING_ON );
    }

    if (!image)
    {
        fprintf(stderr, "IMG_LoadBMP_RW failed - %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_FreeSurface(image);

    SDL_BlitSurface(rot, NULL, screen, 0);

    SDL_Flip(screen);

    SDL_FreeSurface(rot);

    prevValue = value;

}

    //SDL_Delay(5000);

    SDL_Quit();

    return 0;
}
