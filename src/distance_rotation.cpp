
#include "distance_rotation.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"


//////////////////////////////////////////////////////     Rotation            /////////////////////////////////////////////////////////////////////////////
/*
void Rotation(cv::Mat *I, cv::Mat &D, float *alpha)
{
	cv::Size sz = I->size();
	int h = sz.height;
	int w = sz.width;
	//int h = I->getHeight();
	//int w = I->getWidth();
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
				//D[(int)(a+m1)][(int)(b+m2)] = (*I)[i][j];
				D.at<float>((int)(a+m1),(int)(b+m2)) = I->at<float>(i,j);
			}
}

*/

void Rotation(cv::Mat& src, cv::Mat& dst, double angle)
{
    int len = std::max(src.cols, src.rows);
    cv::Point2f pt(len/2., len/2.);
    cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);
    cv::warpAffine(src, dst, r, cv::Size(len, len));
}

//c'est à dire MSE avec l'angle theta qui est ici la variable *x
//u =angle de rotation

float distance_rotation(cv::Mat *patchSrc, cv::Mat *patchTarget, float *u, int ax, int ay, int bx, int by)
{
	//1 - faire une rotation de u sur le patch
	//2 - calculer l'erreur entre le patch rotaté et la cible

	cv::Size s = patchTarget->size();
	int h = s.height;
	int w = s.width;

	cv::Size s2 = patchSrc->size();
	int h2= s2.height;
	int w2 = s2.width;


	cv::namedWindow( "patch target", CV_WINDOW_AUTOSIZE );// Create a window for display.
    	cv::imshow( "patch target", *patchTarget );                   // Show our image inside it.
	cv::waitKey(20);

	cv::namedWindow( "patch source", CV_WINDOW_AUTOSIZE );// Create a window for display.
    	cv::imshow( "patch source", *patchSrc );                   // Show our image inside it.
	cv::waitKey(20);

	
	// envoyer dans patch_rotate la rotation de patchSrc d'un angle u
	 cv::Mat patch_rotate ;
	 patchSrc->copyTo(patch_rotate );
 	 Rotation(*patchSrc, patch_rotate, *u);
	
	//cv::imwrite("patch_rotate.jpg", patch_rotate);	
	

	cv::namedWindow( "patch source rotate", CV_WINDOW_AUTOSIZE );// Create a window for display.
    	cv::imshow( "patch source rotate", patch_rotate );                   // Show our image inside it.
	cv::waitKey(20);


	
	int x = h2/2 - h ;
	cv::Mat patch_final = sub_b_coin(patch_rotate, x, x, h);
	
	cv::namedWindow( "patch final", CV_WINDOW_AUTOSIZE );// Create a window for display.
    	cv::imshow( "patch final", patch_final );                   // Show our image inside it.
	cv::waitKey(20);

	//Calculer l'erreur entre le patch rotaté et la cible
	float erreur =  1.0; //dist(&patch_rotate, patchTarget, ax, ay, bx, by);

	//delete patch_rotate;

	return erreur;
}

cv::Mat sub_b_coin(cv::Mat src, int x, int y, int size)
{
	if(x>0 && y>0 && x < (src.cols) &&  y < (src.rows))
	{	
		cv::Mat t = src(cv::Range::all(), cv::Range(x, x+size));
		return t(cv::Range(y,y+size), cv::Range::all());
	}

	else
	{
		std::cout << "erreur lors de la création du patch" << std::endl ;
		return src ;
	}
}



cv::Mat sub_b_agrandir(cv::Mat src, int x, int y, int size, int agrandir)
{

	int droite = src.cols;
	int bas = src.rows;
	cv::Mat t_a ;
	int k1=0;
	int k2 = 0 ;
		for (int i = x-agrandir/2 ; i<size+agrandir/2+x; i++)
		{
			for (int j = y-agrandir/2; j<size+agrandir/2+y; j++)
			{
				std::cout << "k1 = " << k1 << " k2 = " << k2 << std::endl ; 
				std::cout << "i = " << i << " j = " << j << std::endl ; 
				if(i<bas && j<droite)
				{			
					std::cout << "here 1 !!" << std::endl;
					if(i<0 && j<0) { std::cout << "here 2 !!" << std::endl;t_a.at<float>(k1,k2) = src.at<float>(0,0) ; }
					else if(i>0 && j<0) {t_a.at<float>(k1,k2) = src.at<float>(i,0) ; std::cout << "here 22 !!" << std::endl;}
					else if(i<0 && j>0) {t_a.at<float>(k1,k2) = src.at<float>(0,j) ; std::cout << "here 3 !!" << std::endl;}
					else {t_a.at<float>(k1,k2) = src.at<float>(i,j) ; std::cout << "here 4 !!" << std::endl;}
				}
				else
				{
					
					if(i>bas && j>droite) {t_a.at<float>(k1,k2) = src.at<float>(bas-1,droite-1) ;}
					else if(i>bas && j<droite) {t_a.at<float>(k1,k2) = src.at<float>(i,droite-1) ;}
					else if(i<bas && j>droite) {t_a.at<float>(k1,k2) = src.at<float>(bas-1,j) ;}
					else {t_a.at<float>(i,j) = src.at<float>(k1,k2) ;}	
				}
			k2 ++ ;
			}
		k1++;
		}

return t_a ;
}


	




















