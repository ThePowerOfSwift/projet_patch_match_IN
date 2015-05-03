#g++ -lopencv_core -lopencv_imgproc -lopencv_highgui pm_minimal.cpp pm_minimal.h distance_rotation.cpp distance_rotation.h brent.cpp brent.h -o test

g++ pm_minimal.cpp pm_minimal.h distance_rotation.cpp distance_rotation.h brent.cpp brent.h -o test $(pkg-config opencv --cflags --libs)