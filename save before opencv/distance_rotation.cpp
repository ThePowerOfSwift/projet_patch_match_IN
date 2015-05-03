
#include "BITMAP.h"
#include "pm_minimal.h"
#include "MSE.h"

#include <visp/vpImage.h>
#include <visp/vpException.h>

//////////////////////////////////////////////////////     Rotation            /////////////////////////////////////////////////////////////////////////////

void Rotation(BITMAP *I, BITMAP &D, float *alpha)
{
	int h = I->h ;//->getHeight();
	int w = I->w ;//->getWidth();
	float m1 = h/2;
	float m2 = w/2;
	float c = cos(*alpha);
	float s = sin(*alpha);

		for(int i = 0 ;  i<h; i++)
			for(int j = 0; j<w;j++)
			{

				float a = 0.0;
				float b = 0;

				a = (c*(i - m1) - s*(j - m2)) +m1;
				b = (s*(i - m1) + c*(j - m2)) +m2;
				D[(int)(a+m1)][(int)(b+m2)] = (*I)[i][j];
			}
}

////////////////////////////////////////////////////// Conversion BITMAP -> vpImage  /////////////////////////////////////////////////////////////////////////////


void bitmapToVp(BITMAP * source, vpImage<unsigned char> * target)
{
	int h = source->h;
	int w = source->w;
		
		for(int i = 0 ;  i<h; i++)
			for(int j = 0; j<w;j++)
				(*target)[i][j] = (*source)[i][j];
}



void vpToBitmap(vpImage<unsigned char> * source, BITMAP * target)
{
	int h = source->getHeight();
	int w = source->getWidth();
		
		for(int i = 0 ;  i<h; i++)
			for(int j = 0; j<w;j++)
				(*target)[i][j] = (*source)[i][j] ;
}

//c'est à dire MSE avec l'angle theta qui est ici la variable *x
//u =angle de rotation

int distance_rotation(BITMAP *patchSrc, BITMAP *patchTarget,int ax, int ay, int bx, int by, float *u)
{
	//1 - faire une rotation de u sur le patch
	//2 - calculer l'erreur entre le patch rotaté et la cible



	int h = patchSrc->h;
	int w = patchSrc->w;

	BITMAP patch_rotate(h,w);
	// envoyer dans patch_rotate la rotation de patchSrc d'un angle u
	Rotation(patchSrc, patch_rotate, u);

	
	//Calculer l'erreur entre le patch rotaté et la cible
	int erreur =  0;//
	erreur = dist(&patch_rotate, patchTarget,ax,ay,bx,by );
	//delete patch_rotate;
	return erreur;
}




