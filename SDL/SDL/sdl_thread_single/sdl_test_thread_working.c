#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include <pthread.h>
#include <stdio.h>

/*struct arg_struct {
    //FILE* newFile;
    SDL_RWops *rw1;
}args;*/

SDL_RWops *rw = NULL;
pthread_mutex_t mutexRW;

SDL_RWops *getImage(void) {
    FILE* file;
    SDL_RWops *rw;
    if((file = popen("adb shell \"screencap -p | busybox uuencode -\" | uudecode","r"))==NULL)
    {
        fprintf(stderr, "can't open dump.png\n");
        return 0;
    }

    rw = SDL_RWFromFP(file, 0);

    close(file);
    return rw;
}

int getHeight() {
    FILE* file;
    char buffer[256];
    int value;
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

    return value;
}

void drawScreen(SDL_Surface *image, int value, int prevValue, double scale, const SDL_VideoInfo *videoInfo) {
    SDL_Surface *screen = NULL;
    SDL_Surface *rot = NULL;

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
        //return 1;
    }

    SDL_BlitSurface(rot, NULL, screen, 0);

    SDL_Flip(screen);

    SDL_FreeSurface(rot);

    //return screen;
}

void *run_thread(void *rw_void_ptr) {
    pthread_mutex_lock (&mutexRW);
    rw = getImage();
    pthread_mutex_unlock(&mutexRW);
}
 
int main(int argc, char* argv[])
{
    SDL_Surface *screen = NULL;
    SDL_Surface *image = NULL;
    SDL_Surface *rot = NULL;
    const SDL_VideoInfo *videoInfo = NULL;
    FILE* file;
    double scale;
    int value;
    int prevValue = 0;
    int rc = 0;
    void *status[2];

    pthread_t thread[2]; 

    pthread_mutex_init(&mutexRW, NULL);

    pthread_create(&thread[0], NULL, run_thread, NULL);   

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

    rc = pthread_join(thread[0], &status[0]);

    //image = IMG_LoadPNG_RW(getImage());

    image = IMG_LoadPNG_RW(rw);

    //scale = (image->h)/(videoInfo->current_h);
    scale = 0.5;

    /*-----------------------------------------------------------------*/
while(1){
    //pthread_mutex_init(&mutexRW, NULL);

    pthread_create(&thread[0], NULL, run_thread, NULL);

    value = getHeight();

    //drawScreen(image, value, prevValue, scale, videoInfo);

    if (value != image->h) {
        //scale = (image->w)/(videoInfo->current_w);
        scale = 0.5;
	if (prevValue != image->h) {
	    screen = SDL_SetVideoMode((image->h)*scale,
		                      (image->w)*scale,
		                      videoInfo->vfmt->BitsPerPixel,
		                      SDL_SWSURFACE|SDL_DOUBLEBUF);
	}
        rot = rotozoomSurface( image, 90, scale, SMOOTHING_ON );
    }

    if (value == image->h) {
	//scale = (image->h)/(videoInfo->current_h);
        scale = 0.5;
	if (prevValue != image->h) {
	    screen = SDL_SetVideoMode((image->w)*scale,
		                      (image->h)*scale,
		                      videoInfo->vfmt->BitsPerPixel,
		                      SDL_SWSURFACE|SDL_DOUBLEBUF);
	}
        rot = rotozoomSurface( image, 0, scale, SMOOTHING_ON );
    }

    SDL_FreeSurface(image);

    /*if (!image)
    {
        fprintf(stderr, "IMG_LoadBMP_RW failed - %s\n", IMG_GetError());
        SDL_Quit();
        //return 1;
    }*/

    SDL_BlitSurface(rot, NULL, screen, 0);

    SDL_Flip(screen);

    SDL_FreeSurface(rot);

    prevValue = value;

    rc = pthread_join(thread[0], &status[0]);

    //image = IMG_LoadPNG_RW(getImage());
    image = IMG_LoadPNG_RW(rw);

    //SDL_FreeSurface(rw);

}

    //SDL_Delay(5000);

    SDL_Quit();

    return 0;
}
