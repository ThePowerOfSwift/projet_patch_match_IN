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

	BITMAP * im1 =  load_bitmap("t1.png");
	BITMAP * im2 = load_bitmap("t2.png");
	cv::Mat mat1 ;
	mat1 = BITMAPToMat(im1);
	cv::Mat  mat2 ;
	mat2 = BITMAPToMat(im1);

	cv::Mat * ann;
	cv::Mat * annd;
	
	//patchmatch(im1,im2,ann,annd);
	patchmatch(&mat1,&mat2,ann,annd);




	delete(im1);
	delete(im2);

	return 0;
}