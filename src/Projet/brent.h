#pragma once

#include <opencv2/core/core.hpp>

//*************************************************************
// brent
//*************************************************************


//renvoit l'erreur minimale trouvée pour les différentes rotations testée
float brent (cv::Mat *patchSrc, cv::Mat *patchTarget, float a, float b, float eps, float t, float *x, int ax, int ay, int bx, int by, int taillePatch);
 
     
