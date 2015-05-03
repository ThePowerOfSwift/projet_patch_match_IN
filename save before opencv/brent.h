#pragma once

#include "BITMAP.h"
//#include "MSE.h"
//*************************************************************
// brent
//*************************************************************


//renvoit l'erreur minimale trouvée pour les différentes rotations testée
float brent (BITMAP *patchSrc, BITMAP *patchTarget, float a, float b, float eps, float t, float *x, int ax, int ay, int bx, int by);

     
