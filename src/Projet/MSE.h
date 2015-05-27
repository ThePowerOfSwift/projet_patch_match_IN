#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <cmath>
#include <climits>
#include <iostream>

#include "BITMAP.h"

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

// Problème possible avec le cutoff ( int, float)
void improve_guess(BITMAP * a, BITMAP * b, int ax, int ay, int &xbest,
		   int &ybest, int &dbest, int bx, int by, int patch_w);
void improve_guess(cv::Mat * a, cv::Mat * b, int ax, int ay, int &xbest,
		   int &ybest, int &dbest, int bx, int by, int patch_w);
void improve_guess(cv::Mat * a, cv::Mat * b, float a_brent, float b_brent, float eps_brent,
      float t_brent, float *x_brent, int ax, int ay, int bx, int by, int patch_w, int &dbest, int &xbest, int &ybest);
	  
void gaussianKernel(float kernel[][7]);


float dist(cv::Mat * a, cv::Mat * b, float cutoff=FLT_MAX);
float dist(cv::Mat * a, cv::Mat * b, int ax, int ay, int bx, int by, float cutoff=FLT_MAX);

int dist(BITMAP * a, BITMAP * b, int ax, int ay, int bx, int by, int patch_w,
	 int cutoff = INT_MAX);
int dist(cv::Mat * a, cv::Mat * b, int ax, int ay, int bx, int by,
	 int patch_w, int cutoff = INT_MAX);
