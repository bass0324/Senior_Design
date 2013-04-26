#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
 
int main(int argc, char* argv[])
{
    SDL_Surface *screen = NULL;
    SDL_Surface *image = NULL;
    const SDL_VideoInfo *videoInfo = NULL;
    FILE* file;
    char* buffer;
    int PATH_MAX=800*1280*32/8;
    char path[PATH_MAX];
    long size,i; 
    size_t result;
    
    if((file = popen("adb shell \"screencap -p | busybox uuencode -\" | uudecode","r"))==NULL)
    {
        fprintf(stderr, "can't open dump.png\n");
        return 0;
    }

    size = 0;
    while (fgets(path, PATH_MAX, file) != NULL)
        //printf("%s", path);
        size = size + PATH_MAX;
    //printf("%li",size);

    buffer = (char*) malloc(sizeof(char)*size);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
    
    result = fread(buffer,1,size,file);
    printf("%zi",result);
    if (result != size) {fputs ("Reading error",stderr); exit (3);}
    
    fclose( file );

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

    image = IMG_Load("screencap.png");

    //image = SDL_DisplayFormat(image);

    if (!image)
    {
        fprintf(stderr, "IMG_Load failed - %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    /*-----------------------------------------------------------------*/

    screen = SDL_SetVideoMode(image->w,
                              image->h,
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

    SDL_BlitSurface(image, 0, screen, 0);

    SDL_Flip(screen);
    
    SDL_Delay(5000);

    SDL_FreeSurface(image);

    SDL_Quit();

    return 0;
}
