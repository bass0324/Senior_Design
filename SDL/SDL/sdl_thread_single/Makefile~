C = gcc
CFLAGS = `sdl-config --libs --cflags` -lSDL_image -lpthread

sdl_test : sdl_test.c
	${CC} -o sdl_test sdl_test.c ${CFLAGS}

clean :
	rm -rf sdl_test
