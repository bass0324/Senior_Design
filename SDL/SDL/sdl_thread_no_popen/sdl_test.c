#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include <pthread.h>
#include <stdio.h>

int value;
pthread_mutex_t mutexVALUE;

SDL_Surface *imageShare = NULL;
pthread_mutex_t mutexIMAGE;

void takeScreenshot(int check) {
    if (check == 0) {
        system("adb shell screencap -p /sdcard/dump1.png");
    }
    if (check == 1) {
        system("adb shell screencap -p /sdcard/dump2.png");
    }
}

void transferScreenshot(int check) {
    if (check == 0) {
        system("adb pull /sdcard/dump1.png dump1.png");
    }
    if (check == 1) {
        system("adb pull /sdcard/dump2.png dump2.png");
    }
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

void *take_screen1(void *a) {
    takeScreenshot(0);
}

void *get_screen1(void *b) {
    transferScreenshot(0);
}

void *take_screen2(void *c) {
    takeScreenshot(1);
}

void *get_screen2(void *d) {
    transferScreenshot(1);
}

void *get_height(void *e) {
    pthread_mutex_lock (&mutexVALUE);
    value = getHeight();
    pthread_mutex_unlock(&mutexVALUE);
}

void *load_image2(void *f) {
    pthread_mutex_lock (&mutexIMAGE);
    imageShare = IMG_Load("dump2.png");
    pthread_mutex_unlock(&mutexIMAGE);
}

void *load_image1(void *g) {
    pthread_mutex_lock (&mutexIMAGE);
    imageShare = IMG_Load("dump1.png");
    pthread_mutex_unlock(&mutexIMAGE);
}
 
int main(int argc, char* argv[])
{
    SDL_Surface *screen = NULL;
    SDL_Surface *image = NULL;
    SDL_Surface *rot = NULL;
    const SDL_VideoInfo *videoInfo = NULL;
    FILE* file;
    double scale;
    int prevValue = 0;
    int rc = 0;
    void *status[7];

    pthread_t thread[7];

    pthread_mutex_init(&mutexVALUE, NULL); 
    pthread_mutex_init(&mutexIMAGE, NULL);

    pthread_create(&thread[0], NULL, take_screen1, NULL);

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

    pthread_create(&thread[1], NULL, get_screen1, NULL);

    pthread_create(&thread[2], NULL, take_screen2, NULL);

    rc = pthread_join(thread[1], &status[1]);

    pthread_create(&thread[6], NULL, load_image1, NULL);

    /*-----------------------------------------------------------------*/
while(1){
    rc = pthread_join(thread[2], &status[2]);

    pthread_create(&thread[3], NULL, get_screen2, NULL);
    pthread_create(&thread[0], NULL, take_screen1, NULL);

    pthread_create(&thread[4], NULL, get_height, NULL);

    rc = pthread_join(thread[6], &status[6]);
    image = imageShare;

    rc = pthread_join(thread[4], &status[4]);

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

    rc = pthread_join(thread[3], &status[3]);
    pthread_create(&thread[5], NULL, load_image2, NULL);

    SDL_FreeSurface(image);

    SDL_BlitSurface(rot, NULL, screen, 0);

    SDL_Flip(screen);

    SDL_FreeSurface(rot);

    prevValue = value;

    /*rc = pthread_join(thread[3], &status[3]);
    pthread_create(&thread[5], NULL, load_image2, NULL);*/

    rc = pthread_join(thread[0], &status[0]);

    rc = pthread_join(thread[5], &status[5]);
    image = imageShare;

    pthread_create(&thread[1], NULL, get_screen1, NULL);

    pthread_create(&thread[2], NULL, take_screen2, NULL);

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

    rc = pthread_join(thread[1], &status[1]);
    pthread_create(&thread[6], NULL, load_image1, NULL);

    SDL_FreeSurface(image);

    SDL_BlitSurface(rot, NULL, screen, 0);

    SDL_Flip(screen);

    SDL_FreeSurface(rot);

    prevValue = value;

    /*rc = pthread_join(thread[1], &status[1]);
    pthread_create(&thread[6], NULL, load_image1, NULL);*/

}

    SDL_Quit();

    return 0;
}
