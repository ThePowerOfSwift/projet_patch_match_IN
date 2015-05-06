CC=g++
#-Werror //warning = erreur
CFLAGS=-std=c++11 -O3 -L../../lib/ -lImg -lm -Winvalid-pch -Wall -W -Wno-uninitialized -pipe -march=native -ffast-math -fopenmp -rdynamic -O6 -s -ffast-math -fomit-frame-pointer -fstrength-reduce -msse2 -funroll-loops -D_REENTRANT -pthread -lopencv_core -lopencv_imgproc -lopencv_highgui
LDFLAGS=
EXEC=displayKNN
BUILD=../../build/

all: $(EXEC)

displayKNN: $(BUILD)colorcode.o $(BUILD)displayKNNField.o
	$(CC) $(CFLAGS) -o displayKNN  $(BUILD)colorcode.o $(BUILD)displayKNNField.o $(BUILD)Image.o $(BUILD)RefCntMem.o
	
$(BUILD)colorcode.o: colorcode.cpp colorcode.h
	$(CC) -o $(BUILD)colorcode.o -c colorcode.cpp $(CFLAGS)

$(BUILD)displayKNNField.o: displayKNNField.cpp displayKNNField.h defineForPatchMatchApplication.h
	$(CC) -o $(BUILD)displayKNNField.o -c displayKNNField.cpp $(CFLAGS)
	
clean:
	rm -rf $(BUILD)*.o

mrproper: clean
	rm -rf $(EXEC)