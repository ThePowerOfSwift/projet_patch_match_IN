#include	<string.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<math.h>
#include    <time.h>

#include "defineForPatchMatchApplication.h"
#include "../imageLib/Image.h"
#include "colorcode.h"

void MotionToColor(CFloatImage motim, CByteImage &colim, float maxmotion)
{
    CShape sh = motim.Shape();
    int width = sh.width, height = sh.height;
    colim.ReAllocate(CShape(width, height, 3));
    int x, y;
    // determine motion range:
    float maxx = -999, maxy = -999;
    float minx =  999, miny =  999;
    float maxrad = -1;
    for (y = 0; y < height; y++) {
	for (x = 0; x < width; x++) {
	    float fx = motim.Pixel(x, y, 0);
	    float fy = motim.Pixel(x, y, 1);
	    if (unknown_flow(fx, fy))
		continue;
	    maxx = __max(maxx, fx);
	    maxy = __max(maxy, fy);
	    minx = __min(minx, fx);
	    miny = __min(miny, fy);
	    float rad = sqrt(fx * fx + fy * fy);
	    maxrad = __max(maxrad, rad);
	}
    }
    
	printf("max motion: %.4f  motion range: u = %.3f .. %.3f;  v = %.3f .. %.3f\n", maxrad, minx, maxx, miny, maxy);


    if (maxmotion > 0) // i.e., specified on commandline
	maxrad = maxmotion;

    if (maxrad == 0) // if flow == 0 everywhere
	maxrad = 1;

    
	fprintf(stderr, "normalizing by %g\n", maxrad);

    for (y = 0; y < height; y++) {
	for (x = 0; x < width; x++) {
	    float fx = motim.Pixel(x, y, 0);
	    float fy = motim.Pixel(x, y, 1);
	    uchar *pix = &colim.Pixel(x, y, 0);
	    if (unknown_flow(fx, fy)) {
		pix[0] = pix[1] = pix[2] = 0;
	    } else {
		computeColor(fx/maxrad, fy/maxrad, pix);
	    }
	}
    }
}

void displayMotionField(generalizedAnnStruct *kNN, int sizeX, int sizeY, const char *outputColorFileName, int patchSize, int k)
{
    CFloatImage im;
    CShape sh(sizeX, sizeY, 2);
    im.ReAllocate(sh);

    for(int y=0; y<sizeY; y++)
    {
		for(int x=0; x<sizeX; x++)
		{
		 if (y<patchSize || y>sizeY-patchSize || x<patchSize || x>sizeX-patchSize)
		 {
          im.Pixel(x,y,0) = 0.0f;
          im.Pixel(x,y,1) = 0.0f;
		 }
		 else
		 {
          im.Pixel(x,y,0) = kNN[x + y*sizeX].dx[k]-x;
          im.Pixel(x,y,1) = kNN[x + y*sizeX].dy[k]-y;
		 }
		}
    }

	//--------------------------------------
    // Image couleur de sortie
	//--------------------------------------
    CByteImage outim;
    sh.nBands = 3;
    outim.ReAllocate(sh);
    outim.ClearPixels();
	

	//--------------------------------------
	//
	//--------------------------------------
    MotionToColor(im, outim, -1.0f);
	
	//--------------------------------------
	//
	//--------------------------------------
    FILE *fp;
    unsigned char * buffer = (unsigned char *)calloc(3*sizeX*sizeY, sizeof(unsigned char));
 
	// Interlaced mode
	for(int y=0; y<sizeY; y++)
    {
		for(int x=0; x<sizeX; x++)
		{         
		 int index = 3*(x+y*sizeX);
		 buffer[index] = outim.Pixel(x,y,2);
		 buffer[index+1] = outim.Pixel(x,y,1); 
		 buffer[index+2] = outim.Pixel(x,y,0);		 
		}
    }


    if((fp = fopen (outputColorFileName,"wb+"))==NULL)
    {
     printf("\n - ERROR: can not open PPM file %s \n", outputColorFileName);
     exit(-1);
    }
    //if (mode == INTERLACED)   

    // ppm header
    fprintf(fp, "P6\n""%d %d\n""%d\n", sizeX, sizeY, 255);
    fwrite(buffer, 1, 3*sizeX*sizeY, fp);
   putc('\n', fp);
  
   fclose(fp);
   free (buffer);   
}