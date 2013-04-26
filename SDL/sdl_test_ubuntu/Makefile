CC = gcc
CFLAGS = `sdl-config --cflags --libs` -lSDL_image

sdl_test : sdl_test.c
	${CC} -o sdltest sdl_test.c ${CFLAGS}

clean :
	rm -rf sdl_test
