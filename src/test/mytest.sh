g++  -lopencv_core -lopencv_imgproc -lopencv_highgui distance_rotation.cpp distance_rotation.h brent.cpp brent.h MSE.cpp MSE.h mytest.cpp BITMAP.cpp PatchMatch.h PatchMatch.cpp -o test $(pkg-config opencv --cflags --libs)

