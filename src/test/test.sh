#g++ -lopencv_core -lopencv_imgproc -lopencv_highgui pm_minimal.cpp pm_minimal.h distance_rotation.cpp distance_rotation.h brent.cpp brent.h -o test

g++  -lopencv_core -lopencv_imgproc -lopencv_highgui ../Projet/distance_rotation.cpp ../Projet/distance_rotation.h ../Projet/brent.cpp ../Projet/brent.h ../Projet/BITMAP.cpp ../Projet/BITMAP.h ../Projet/MSE.cpp ../Projet/MSE.h test.cpp -o test $(pkg-config opencv --cflags --libs)

