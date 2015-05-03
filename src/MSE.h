
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <cmath>
#include <climits>

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

void gaussianKernel(float kernel [][7]);
float dist ( cv::Mat *a, cv::Mat *b, int ax, int ay, int bx, int by, int cutoff=INT_MAX);

