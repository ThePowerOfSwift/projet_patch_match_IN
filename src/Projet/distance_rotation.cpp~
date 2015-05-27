#include <climits>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "distance_rotation.h"
#include "MSE.h"

//////////////////////////////////////////////////////     Rotation            /////////////////////////////////////////////////////////////////////////////

void Rotation(cv::Mat & src, cv::Mat & dst, double angle)
{
	int len = std::max(src.cols, src.rows);
	cv::Point2f pt(len / 2., len / 2.);
	cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);
	cv::warpAffine(src, dst, r, cv::Size(len, len));
}

//c'est à dire MSE avec l'angle theta qui est ici la variable *x
//u =angle de rotation

float
distance_rotation(cv::Mat * patchSrc, cv::Mat * patchTarget,
		  float *u /*, int ax, int ay, int bx, int by */ )
{
	//1 - faire une rotation de u sur le patch
	//2 - calculer l'erreur entre le patch rotaté et la cible

	cv::Size s = patchTarget->size();
	int h = s.height;
	//int w = s.width;

	cv::Size s2 = patchSrc->size();
	int h2 = s2.height;
	//int w2 = s2.width;

	/*
	   cv::namedWindow( "target", CV_WINDOW_AUTOSIZE );// Create a window for display.
	   cv::imshow( "target", *patchTarget );                   // Show our image inside it.
	   cv::waitKey(20);

	   cv::namedWindow( "source", CV_WINDOW_AUTOSIZE );// Create a window for display.
	   cv::imshow( "source", *patchSrc );                   // Show our image inside it.
	   cv::waitKey(20);
	 */

	// envoyer dans patch_rotate la rotation de patchSrc d'un angle u
	cv::Mat patch_rotate;
	patchSrc->copyTo(patch_rotate);
	Rotation(*patchSrc, patch_rotate, *u);

	//cv::imwrite("patch_rotate.jpg", patch_rotate);        
	/*

	   cv::namedWindow( "rotate", CV_WINDOW_AUTOSIZE );// Create a window for display.
	   cv::imshow( "rotate", patch_rotate );                   // Show our image inside it.
	   cv::waitKey(20);
	 */

	int x = h2 / 2l;
	//cv::Mat patch_final = sub(patch_rotate, x, x, h);
	cv::Mat patch_final = sub_milieu(patch_rotate, x, x, h);
	//cv::namedWindow( "patch final", CV_WINDOW_AUTOSIZE );// Create a window for display.
	//cv::imshow( "patch final", patch_final );                   // Show our image inside it.
	//cv::waitKey(20);

	//Calculer l'erreur entre le patch rotaté et la cible
	float erreur = dist(&patch_final, patchTarget);
	//delete patch_rotate;

	return erreur;
}

//soustrait un patch de taille size dans le sens diagonal et dont le coin en haut a gauche est (x,y)
cv::Mat sub(cv::Mat I, int x, int y, int size)
{

	//int h = I.rows ;
	//int w = I.cols ;

	//int i = y;
	//int j = x;

	if (x >= 0 && y >= 0) {
		if (x < (I.cols) && y < (I.rows)) {
			if (x + size < I.cols && y + size < I.rows) {	//patch contenu dans l'image
				cv::Mat t =
				    I(cv::Range::all(), cv::Range(x, x + size));
				//      std::cout << "patch dans l'image " << std::endl ;

				return t(cv::Range(y, y + size),
					 cv::Range::all());
			}

			else {
				cv::Mat tI =
				    I(cv::Range::all(), cv::Range(0, size));
				cv::Mat t =
				    tI(cv::Range(0, size), cv::Range::all());
				//std::cout << "patch qui depasse - cas 1" << std::endl ;
				//patch qui dépasse de l'image a droite ou vers le bas
				//copier la partie contenue
				for (int k1 = y; k1 < I.rows; k1++) {
					for (int k2 = x; k2 < I.cols; k2++) {
						t.at < int >(k1 - y, k2 - x) =
						    I.at < int >(k1, k2);
					}
				}
				//remplir la partie qui dépasse
				int d1 = size - (I.rows - x);
				int d2 = size - (I.cols - y);

				for (int k1 = 0; k1 < d1; k1++) {
					for (int k2 = 0; k2 < d2; k2++) {
						//std::cout << "partie qui depasse" << std::endl ;
						t.at < int >(I.rows - x + k1, I.cols - y + k2) = 0;	// I.at<float>(0,0);     
					}
				}

				return t;
			}
		} else {
			std::
			    cout << "erreur lors de la création du patch" <<
			    std::endl;
			return I;
		}

	} else {
		//std::cout << "patch qui depasse - cas 2" << std::endl ;

		cv::Mat t;
		I.copyTo(t);

		//patch qui dépasse de l'image a gauche ou vers le haut
		//copier la partie contenue

		for (int k1 = -y; k1 < size; k1++) {
			for (int k2 = -x; k2 < size; k2++) {
				t.at < int >(k1, k2) =
				    I.at < int >(k1 + y, k2 + x);
			}
		}
		cv::Mat t2 = t(cv::Range::all(), cv::Range(0, size));
		return t2(cv::Range(0, size), cv::Range::all());

	}

}

//soustrait un patch de taille size dans tous les sens dont le milieu est (x,y)

cv::Mat sub_milieu(cv::Mat I, int x, int y, int size)
{
	int m = size / 2;

	if (x >= 0 && y >= 0 && x < (I.cols) && y < (I.rows)) {
		if (x - m >= 0 && y - m >= 0 && x + m < I.cols
		    && y + m < I.rows) {
			//std::cout << "cas 1" << std::endl ;
			//patch dans l'image
			cv::Mat t2 =
			    I(cv::Range::all(), cv::Range(x - m, x + m));
			return t2(cv::Range(y - m, y + m), cv::Range::all());
		} else {
			//patch qui dépasse vers la gauche ou le haut
			if (x + m < I.cols && y + m < I.rows) {
				//std::cout << " cas 2" << std::endl ;
				//copie de la partie contenue dans l'image
				cv::Mat t =
				    I(cv::Range::all(), cv::Range(0, m + x));
				cv::Mat t2 =
				    t(cv::Range(0, y + m), cv::Range::all());

				//remplissage du reste
				cv::Mat t3;
				I.copyTo(t3);

				//patch qui dépasse de l'image a gauche ou vers le haut
				//copier la partie contenue

				for (int k1 = 0; k1 < t2.rows; k1++) {
					for (int k2 = 0; k2 < t2.cols; k2++) {
						t3.at < int >(k1 + m - y,
							      k2 + m - x) =
						    t2.at < int >(k1, k2);
					}
				}
				cv::Mat t4 =
				    t3(cv::Range::all(), cv::Range(0, size));
				return t4(cv::Range(0, size), cv::Range::all());

			}
			//patch qui dépasse vers la droite ou le bas
			else {
				//      std::cout << "cas 3" << std::endl ;
				//copie de la partie contenue dans l'image
				cv::Mat t =
				    I(cv::Range::all(),
				      cv::Range(x - m, I.cols));
				cv::Mat t2 =
				    t(cv::Range(y - m, I.rows),
				      cv::Range::all());

				//remplissage du reste
				cv::Mat t3;
				I.copyTo(t3);

				//copier la partie contenue

				for (int k1 = 0; k1 < t2.rows; k1++) {
					for (int k2 = 0; k2 < t2.cols; k2++) {
						t3.at < int >(k1, k2) =
						    t2.at < int >(k1, k2);
					}
				}
				cv::Mat t4 =
				    t3(cv::Range::all(), cv::Range(0, size));
				return t4(cv::Range(0, size), cv::Range::all());

			}
		}
	}

	else {
		std::cout << "coordonées de x et y non valides" << std::endl;
		return I;
	}
}
