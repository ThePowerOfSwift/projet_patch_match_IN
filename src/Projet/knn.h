#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <climits>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

using namespace cv;

const int aw = 1;

#define XY_TO_INT(x, y) (((y)<<12)|(x))
#define INT_TO_X(v) ((v)&((1<<12)-1))
#define INT_TO_Y(v) ((v)>>12)

void k_improve_guess(cv::Mat *a, cv::Mat *b, int ax, int ay, int &xbest, int &ybest, int &kdbest, int bx, int by, bool &guess_ok);
void trier_voisins_init(cv::Mat *knn,cv::Mat *knnd, int x, int y);
void trier_voisins(cv::Mat *knn, cv::Mat *knnd, int x, int y);
void inserer_fin(int ax, int ay, int kxbest, int kybest, int kdbest,cv::Mat *knn,cv::Mat *knnd);
