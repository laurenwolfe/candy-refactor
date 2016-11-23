SRC=game_model.cpp game_window.c adaptors.c
HEADERS=game_model.h game_window.h adaptors.h
LIBS=../hw2/libhw2.a
CFLAGS = -Wall -Wpedantic -std=c++11
IFLAGS =  -I../hw2 -I../jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS =  -L../hw2 -L../jansson/lib -lhw2 -l:libjansson.a
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`

all: hw4

hw4: $(SRC) $(HEADERS) $(LIBS)
	g++ $(CFLAGS) -g -O0 $(IFLAGS) $(GTKCFLAGS) -o hw4 $(SRC) $(LDFLAGS) $(GTKLDFLAGS)


clean:
	rm -f *.o *~ hw4