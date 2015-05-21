#pragma once

#include "visp/vpImage.h"
#include <opencv2/core/core.hpp>
#include "MSE.h"
#include <opencv2/highgui/highgui.hpp>

void Rotation(cv::Mat &I, cv::Mat &D, double alpha);

float distance_rotation(cv::Mat *patchSrc, cv::Mat *patchTarget, float *u, int ax, int ay, int bx, int by);

//void sub_b(cv::Mat src, cv::Mat target, int x, int y, int size);
//cv::Mat sub_b_coin(cv::Mat src, int x, int y, int size);
cv::Mat sub(cv::Mat src, int x, int y, int size);
//cv::Mat sub_b_agrandir(cv::Mat src, int x, int y, int size, int agrandir);

cv::Mat sub_milieu(cv::Mat I, int x, int y, int size);



