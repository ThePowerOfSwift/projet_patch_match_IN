#pragma once

#include "visp/vpImage.h"
#include "BITMAP.h"

void Rotation(vpImage<unsigned char> *I, vpImage<unsigned char> &D, float *alpha);

void bitmapToVp(BITMAP * source, vpImage<unsigned char> * target);

void vpToBitmap(vpImage<unsigned char> * source, BITMAP * target);

float distance_rotation(BITMAP *patchSrc, BITMAP *patchTarget,int ax, int ay, int bx, int by, float *u);




