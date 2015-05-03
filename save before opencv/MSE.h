
#include "BITMAP.h"
#include <math.h>
#include <cmath>
#include <climits>

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

int dist ( BITMAP *a, BITMAP *b, int ax, int ay, int bx, int by, int cutoff=INT_MAX);
