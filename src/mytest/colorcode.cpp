// colorcode.cpp
//
// Color encoding of flow vectors
// adapted from the color circle idea described at
//   http://members.shaw.ca/quadibloc/other/colint.htm
//
// Daniel Scharstein, 4/2007
// added tick marks and out-of-range coding 6/05/07

#include <stdlib.h>
#include <math.h>
    
#ifdef _MSC_VER
#include <float.h>		// for _isnan() on VC++
#define isnan(x) _isnan(x)	// VC++ uses _isnan() instead of isnan()
//#else
//#include <math.h>  // for isnan() everywhere else
#endif				/* 



#include "colorcode.h"


#define MAXCOLS 60
int colorwheel[MAXCOLS][3];

// return whether flow vector is unknown
    bool unknown_flow(float u, float v)
{
	
	    ||(fabs(v) > UNKNOWN_FLOW_THRESH) 



{
	



{
	
	
	


 makecolorwheel() 
{
	
	    // relative lengths of color transitions:
	    // these are chosen based on perceptual similarity
	    // (e.g. one can distinguish more shades between red and yellow 
	    //  than between yellow and green)
	int RY = 15;
	
	
	
	
	
	
	
	    //printf("ncols = %d\n", ncols);
	    if (ncols > MAXCOLS)
		
	
	
	
		setcols(255, 255 * i / RY, 0, k++);
	
		setcols(255 - 255 * i / YG, 255, 0, k++);
	
		setcols(0, 255, 255 * i / GC, k++);
	
		setcols(0, 255 - 255 * i / CB, 255, k++);
	
		setcols(255 * i / BM, 0, 255, k++);
	
		setcols(255, 0, 255 - 255 * i / MR, k++);



{
	
		
	
	
	
	
	
	
	
	    //f = 0; // uncomment to see original color wheel
	    for (int b = 0; b < 3; b++) {
		
		
		
		
			
		else
			
		pix[2 - b] = (int)(255.0 * col);
