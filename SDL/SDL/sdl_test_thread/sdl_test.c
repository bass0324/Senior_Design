#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
 
int main(int argc, char* argv[])
{
    SDL_Surface *screen = NULL;
    SDL_Surface *image = NULL;
    SDL_Surface *image2 = NULL;
    const SDL_VideoInfo *videoInfo = NULL;
    FILE* file;
    FILE* newFile;
    //char* buffer;
    //int PATH_MAX=800*1280*32/8;
    //char path[PATH_MAX];
    //long size,i; 
    //size_t result;
    
    

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

    /*-----------------------------------------------------------------*/

    screen = SDL_SetVideoMode(800,
                              1280,
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
    if((file = popen("adb shell \"screencap -p | busybox uuencode -\" | uudecode","r"))==NULL)
    {
        fprintf(stderr, "can't open dump.png\n");
        return 0;
    }

    if((newFile = popen("adb shell \"screencap -p | busybox uuencode -\" | uudecode","r"))==NULL)
    {
        fprintf(stderr, "can't open dump.png\n");
        return 0;
    }

    SDL_RWops *rw = SDL_RWFromFP(file, 0);

    image = IMG_LoadPNG_RW(rw);
    
    SDL_FreeRW(rw);
    close(file);

    if (!image)
    {
        fprintf(stderr, "IMG_LoadBMP_RW failed - %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_BlitSurface(image, 0, screen, 0);

    SDL_Flip(screen);

    SDL_RWops *rw2 = SDL_RWFromFP(newFile, 0);

    image2 = IMG_LoadPNG_RW(rw2);

    SDL_FreeRW(rw2);
    close(newFile);

    SDL_BlitSurface(image2, 0, screen, 0);

    SDL_Flip(screen);

}

    SDL_FreeSurface(image);

    //SDL_Delay(5000);

    SDL_Quit();

    return 0;
}
