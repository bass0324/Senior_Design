CC = gcc
CFLAGS = `sdl-config --libs --cflags` -lSDL_image -lSDL_gfx -lpthread

sdl_test : sdl_test.c
	${CC} -o sdl_test sdl_test.c ${CFLAGS}

clean :
	rm -rf sdl_test
