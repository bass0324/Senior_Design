#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include "SDL/SDL_framerate.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define SMOOTHING_ON 1

int value;
pthread_mutex_t mutexVALUE;

SDL_RWops *rw = NULL;
pthread_mutex_t mutexRW;

SDL_RWops *rw1 = NULL;
pthread_mutex_t mutexRW1;

SDL_Surface *imageShare = NULL;
pthread_mutex_t mutexIMAGE;

SDL_RWops *getImage(void) {
    FILE* file;
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

void *get_screen(void *b) {
    pthread_mutex_lock (&mutexRW);
    rw = getImage();
    pthread_mutex_unlock(&mutexRW);
}

void *get_height(void *e) {
    pthread_mutex_lock (&mutexVALUE);
    value = getHeight();
    pthread_mutex_unlock(&mutexVALUE);
}

void *load_image(void *g) {
    pthread_mutex_lock (&mutexIMAGE);
    pthread_mutex_lock (&mutexRW1);
    imageShare = IMG_LoadPNG_RW(rw1);
    pthread_mutex_unlock (&mutexRW1);
    pthread_mutex_unlock (&mutexIMAGE);
}
 
int main(int argc, char* argv[])
{
    FPSmanager manex;

    SDL_Surface *screen = NULL;
    SDL_Surface *image = NULL;
    SDL_Surface *rot = NULL;
    const SDL_VideoInfo *videoInfo = NULL;
    double scale;
    int prevValue = 0;
    int rc = 0;
    void *status[3];

    //SDL_initFramerate( &manex );
    //SDL_setFramerate( &manex, 1 );

    pthread_t thread[3];

    pthread_mutex_init(&mutexVALUE, NULL); 
    pthread_mutex_init(&mutexRW, NULL);
    pthread_mutex_init(&mutexRW1, NULL);
    pthread_mutex_init(&mutexIMAGE, NULL);

    pthread_create(&thread[1], NULL, get_screen, NULL);

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

    rc = pthread_join(thread[1], &status[1]);
    
    rw1 = rw;
    pthread_create(&thread[1], NULL, get_screen, NULL);

    pthread_create(&thread[2], NULL, load_image, NULL);

    /*-----------------------------------------------------------------*/
while(1){

    pthread_create(&thread[0], NULL, get_height, NULL);

    rc = pthread_join(thread[1], &status[1]);
    rw1 = rw;

    rc = pthread_join(thread[2], &status[2]);
    image = imageShare;
    
    rc = pthread_join(thread[0], &status[0]);

    pthread_create(&thread[1], NULL, get_screen, NULL);
    
    pthread_create(&thread[2], NULL, load_image, NULL);

    if (value != image->h) {
        //scale = (image->w)/(videoInfo->current_w);
        scale = 1;
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

    SDL_BlitSurface(rot, NULL, screen, 0);

    SDL_Flip(screen);

    SDL_FreeSurface(rot);

    prevValue = value;

    //SDL_framerateDelay( &manex );
}

    SDL_Quit();

    return 0;
}
