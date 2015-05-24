CC=g++
#-Werror //warning = erreur
CFLAGS=-std=c++11 -O3 -L../../lib/ -lImg -lm -Winvalid-pch -Wall -W -Wno-uninitialized -pipe -march=native -ffast-math -fopenmp -rdynamic -O6 -s -ffast-math -fomit-frame-pointer -fstrength-reduce -msse2 -funroll-loops -D_REENTRANT -pthread -lopencv_core -lopencv_imgproc -lopencv_highgui
#LDFLAGS=$(pkg-config opencv --cflags --libs)
FLAGS = `pkg-config opencv --cflags --libs`
#LIBS = `pkg-config --libs opencv`
EXEC=PatchMatch
BUILD=../../build/
BUILDIMG=../../build/build_imageLib/

all: $(EXEC)

PatchMatch: $(BUILD)main.o $(BUILD)distance_rotation.o $(BUILD)brent.o $(BUILD)BITMAP.o $(BUILD)MSE.o $(BUILD)knn.o $(BUILD)PatchMatch.o $(BUILD)colorcode.o $(BUILD)displayKNNField.o $(BUILDIMG)Image.o $(BUILDIMG)RefCntMem.o
	$(CC) $(CFLAGS) -o ../../PatchMatch $(BUILD)main.o $(BUILD)distance_rotation.o $(BUILD)brent.o $(BUILD)BITMAP.o $(BUILD)MSE.o $(BUILD)knn.o $(BUILD)PatchMatch.o $(BUILD)colorcode.o $(BUILD)displayKNNField.o $(BUILDIMG)Image.o $(BUILDIMG)RefCntMem.o $(FLAGS)

$(BUILD)main.o: main.cpp PatchMatch.h BITMAP.h
	$(CC) $(CFLAGS) -o $(BUILD)main.o -c main.cpp 

$(BUILD)distance_rotation.o: distance_rotation.cpp distance_rotation.h MSE.h
	$(CC) $(CFLAGS) -o $(BUILD)distance_rotation.o -c distance_rotation.cpp 

$(BUILD)brent.o: brent.cpp brent.h distance_rotation.h
	$(CC) $(CFLAGS) -o $(BUILD)brent.o -c brent.cpp 

$(BUILD)BITMAP.o: BITMAP.cpp BITMAP.h
	$(CC) $(CFLAGS) -o $(BUILD)BITMAP.o -c BITMAP.cpp 

$(BUILD)MSE.o: MSE.cpp MSE.h brent.h BITMAP.h
	$(CC) $(CFLAGS) -o $(BUILD)MSE.o -c MSE.cpp 
	
$(BUILD)knn.o: knn.cpp knn.h MSE.h
	$(CC) $(CFLAGS) -o $(BUILD)knn.o -c knn.cpp
	
$(BUILD)colorcode.o: colorcode.cpp colorcode.h
	$(CC) $(CFLAGS) -o $(BUILD)colorcode.o -c colorcode.cpp

$(BUILD)displayKNNField.o: displayKNNField.cpp displayKNNField.h defineForPatchMatchApplication.h
	$(CC) $(CFLAGS) -o $(BUILD)displayKNNField.o -c displayKNNField.cpp
	
$(BUILD)PatchMatch.o: PatchMatch.cpp PatchMatch.h brent.h BITMAP.h displayKNNField.h knn.h MSE.h
	$(CC) $(CFLAGS) -o $(BUILD)PatchMatch.o -c PatchMatch.cpp
	
clean:
	rm -rf $(BUILD)*.o

mrproper: clean
	rm -rf $(EXEC)
