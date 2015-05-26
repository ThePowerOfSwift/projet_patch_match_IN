#ifndef PATCHMATCH_H
#define PATCHMATCH_H

#define NB_MAX_SIMILAR_PICTURE 1
#define NB_MAX_KNN 100
    typedef struct {
	int dx;
	int dy;
	float distance;
} annStruct;
 typedef struct {
	int *dx;
	int *dy;
	float *distance;
} generalizedAnnStruct;
 typedef struct {
	float U, V;
	float confidence;
} regionModeStruct;
 typedef struct {
	float hUV[256][256];
} histoStruct;
 
#endif				//patchMatch
