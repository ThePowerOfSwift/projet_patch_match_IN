#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <climits>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "PatchMatch.h"
#include "MSE.h"
#include "brent.h"
#include "BITMAP.h"
#include "knn.h"
#include "displayKNNField.h"

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

int main(){

	BITMAP * im1 =  load_bitmap("t2.png");
	BITMAP * im2;

	cv::Mat mat1 = BITMAPToMat(im1);
	im2 = matToBITMAP(mat1);

	save_bitmap(im2,"t3.png");

	delete(im1);
	delete(im2);

	return 0;
}