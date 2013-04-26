#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

#define SMOOTHING_OFF		0
#define SMOOTHING_ON		1

int value;
pthread_mutex_t mutexVALUE;

SDL_Surface *image = NULL;
pthread_mutex_t mutexIMAGE;

int screenH;
int screenW;

void takeScreenshot(void) {
    system("./adb shell screencap -p /sdcard/dump1.png; adb shell cp /sdcard/dump1.png /sdcard/dump2.png");
}

void transferScreenshot(void) {
    system("./adb pull /sdcard/dump2.png dump2.png");
}

int getHeight() {
    FILE* file;
    char buffer[256];
    int value;
    if((file = popen("./adb shell dumpsys window | grep cur= | awk -F cur= '{print $NF}' | awk -F ' ' '{print $1}' | awk -F 'x' '{print $NF}'", "r"))==NULL)
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

int getScreenRes() {
    int fbfd = 0; // framebuffer filedescriptor
    struct fb_var_screeninfo var_info;

    // Open the framebuffer device file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        return(1);
    }
    printf("The framebuffer device opened.\n");

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &var_info)) {
        printf("Error reading variable screen info.\n");
        return(1);
    }
    printf("Display info %dx%d, %d bpp\n", 
            var_info.xres, var_info.yres, 
            var_info.bits_per_pixel );

    // close file  
    close(fbfd);

    screenW = var_info.yres;
    screenH = var_info.xres;

    SDL_Delay(1000);
  
    return 0;
}

void *take_screen(void *a) {
    takeScreenshot();
}

void *get_screen(void *d) {
    transferScreenshot();
}

void *get_height(void *e) {
    pthread_mutex_lock (&mutexVALUE);
    value = getHeight();
    pthread_mutex_unlock(&mutexVALUE);
}

void *load_image(void *f) {
    pthread_mutex_lock (&mutexIMAGE);
    image = IMG_Load("dump2.png");
    pthread_mutex_unlock(&mutexIMAGE);
}

int doSDL(void) {
    system("./adb connect 192.168.49.1");
    SDL_Surface *screen = NULL;
    SDL_Surface *rot = NULL;
    const SDL_VideoInfo *videoInfo = NULL;
    double scale;
    int prevValue = 0;
    int rc = 0;

    pthread_t thread[7];
    void *status[7];

    pthread_mutex_init(&mutexVALUE, NULL); 
    pthread_mutex_init(&mutexIMAGE, NULL);

    pthread_create(&thread[0], NULL, take_screen, NULL);

    /*-----------------------------------------------------------------*/

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
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

    pthread_create(&thread[1], NULL, get_screen, NULL);

    pthread_create(&thread[0], NULL, take_screen, NULL);

    rc = pthread_join(thread[1], &status[1]);

    pthread_create(&thread[2], NULL, load_image, NULL);
    
    rc = pthread_join(thread[0], &status[0]);

    /*-----------------------------------------------------------------*/

    char key;
    while(1){
        scanf("%c", &key);
        // if E or e, exit
        if (key == ' ')
	    break;

        pthread_create(&thread[1], NULL, get_screen, NULL);

        pthread_create(&thread[0], NULL, take_screen, NULL);

        pthread_create(&thread[3], NULL, get_height, NULL);

        rc = pthread_join(thread[3], &status[3]);

        rc = pthread_join(thread[1], &status[1]);

        pthread_create(&thread[2], NULL, load_image, NULL);

        rc = pthread_join(thread[2], &status[2]);

        if (value != image->h) {
	    scale = screenW/(image->w);
	    //scale = 1;
	    if (prevValue != image->h) {
	        screen = SDL_SetVideoMode((image->h)*scale,
			                  (image->w)*scale,
			                  videoInfo->vfmt->BitsPerPixel,
			                  SDL_SWSURFACE);
	    }
	    rot = rotozoomSurface( image, 90, scale, SMOOTHING_ON );
        }

        else if (value == image->h) {
	    scale = screenH/(image->h);
	    //scale = 0.5;
	    if (prevValue != image->h) {
	        screen = SDL_SetVideoMode((image->w)*scale,
			                  (image->h)*scale,
			                  videoInfo->vfmt->BitsPerPixel,
	    		                  SDL_SWSURFACE);
	    }
	    rot = rotozoomSurface( image, 0, scale, SMOOTHING_ON );
        }

        SDL_FreeSurface(image);

        SDL_BlitSurface(rot, NULL, screen, 0);

        //SDL_Flip(screen);
        //SDL_UpdateRect(screen, 0, 0, 0, 0);

        SDL_FreeSurface(rot);

        prevValue = value;

        rc = pthread_join(thread[0], &status[0]);
    }

    SDL_Quit();

    return 0;
}

int main(int argc, char* argv[])
{
    if (getScreenRes() != 0) {
        printf("Error reading screen info.\n");
        return 1;
    }
    if (doSDL() != 0) {
        printf("Error displaying images.\n");
        return 1;
    }
    return 0;
}
