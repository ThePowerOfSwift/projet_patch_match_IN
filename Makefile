CC=g++
#-Werror //warning = erreur
CFLAGS=-std=c++11 -O3 -Winvalid-pch -Wall -W -Wno-uninitialized -pipe -march=native -ffast-math -fopenmp -rdynamic -O6 -s -ffast-math -fomit-frame-pointer -fstrength-reduce -msse2 -funroll-loops -D_REENTRANT -pthread -lopencv_core -lopencv_imgproc -lopencv_highgui
LDFLAGS=$(pkg-config opencv --cflags --libs)
EXEC=PatchMatch
SRC=src/
BUILD=build/

all: $(EXEC)

PatchMatch: $(BUILD)pm_minimal.o $(BUILD)distance_rotation.o $(BUILD)brent.o $(BUILD)BITMAP.o $(BUILD)MSE.o
	$(CC) $(CFLAGS) -o PatchMatch $(BUILD)pm_minimal.o $(BUILD)distance_rotation.o $(BUILD)brent.o $(BUILD)BITMAP.o $(BUILD)MSE.o $(LDFLAGS)

$(BUILD)pm_minimal.o: $(SRC)pm_minimal.cpp $(SRC)pm_minimal.h $(SRC)brent.h $(SRC)BITMAP.h
	$(CC) -o $(BUILD)pm_minimal.o -c $(SRC)pm_minimal.cpp $(CFLAGS)

$(BUILD)distance_rotation.o: $(SRC)distance_rotation.cpp $(SRC)distance_rotation.h
	$(CC) -o $(BUILD)distance_rotation.o -c $(SRC)distance_rotation.cpp $(CFLAGS)

$(BUILD)brent.o: $(SRC)brent.cpp $(SRC)brent.h $(SRC)distance_rotation.h
	$(CC) -o $(BUILD)brent.o -c $(SRC)brent.cpp $(CFLAGS)

$(BUILD)BITMAP.o: $(SRC)BITMAP.cpp $(SRC)BITMAP.h
	$(CC) -o $(BUILD)BITMAP.o -c $(SRC)BITMAP.cpp $(CFLAGS)

$(BUILD)MSE.o: $(SRC)MSE.cpp $(SRC)MSE.h
	$(CC) -o $(BUILD)MSE.o -c $(SRC)MSE.cpp $(CFLAGS)

clean:
	rm -rf $(BUILD)*.o

mrproper: clean
	rm -rf $(EXEC)