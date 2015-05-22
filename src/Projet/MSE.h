
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <cmath>
#include <climits>
#include <iostream>

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

void gaussianKernel(float kernel [][7]);
float dist( cv::Mat *a, cv::Mat *b/*, int cutoff=INT_MAX*/);

