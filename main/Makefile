include main_objects.h
#
PROGRAM = thewatercolorist
#
CC = g++
CFLAGS = -g -Wall -std=c++11\
-I.. -I../../common
#
all: $(PROGRAM)
#
$(PROGRAM): $(main_objects) ../thewatercolorist.o
	$(CC) -o $@ $(main_objects)\
        ../thewatercolorist.o\
        ../FastNoise-master/FastNoise-master.o\
        ../RGBConverter-master/RGBConverter-master.o\
        ../../common/io/io.o\
        ../../common/jpeg/jpeg.o\
        ../../common/math/math.o\
        ../../common/png/png.o\
        ../../common/tiff/tiff.o\
        ../../common/util/util.o\
        -ltiff\
        -lpng\
        -ljpeg\
        -lz\
        -lm
#
.c.o:
	$(CC) -c $(CFLAGS) $<
.cpp.o:
	$(CC) -c $(CFLAGS) $<
