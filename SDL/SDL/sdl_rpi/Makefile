CC = gcc
CFLAGS = `sdl-config --libs --cflags` -lSDL_image -lSDL_gfx -lpthread

doAuto : sdl_test.c
	${CC} -o doAuto sdl_test.c ${CFLAGS}

clean :
	rm -rf doAuto
