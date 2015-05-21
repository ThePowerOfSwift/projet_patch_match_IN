#include "MSE.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

void gaussianKernel(float kernel [][7]){
	float mean = 3;
	float sum = 0.0; 
	for (int i = 0; i < 7; i++){
    	for (int j = 0; j < 7; j++) {
        	kernel[i][j] = exp( -0.5 * (pow((i-mean), 2.0) + pow((j-mean),2.0)) )/ (2 * M_PI);
        	sum += kernel[i][j];
    	}
    }
	for (int i = 0; i < 7; i++){
    	for (int j = 0; j < 7; j++) {
        	kernel[i][j] /= sum;
        }
   }
}

float dist ( cv::Mat *a, cv::Mat *b, int cutoff){
	float answer =0;
	int size =a->size().height;
	int radius = size/2;
	int W=0;
	float amid = a->at<float>(3,3);
	float bmid = b->at<float>(3,3);
	//Faire un define
	float kernel [7][7];
	gaussianKernel(kernel);
	
	for (int dy = 0; dy < size; dy++) {
    	for (int dx = 0; dx < size; dx++) {
    	
      	float ac = a->at<float>(dy,dx);
      	float bc = b->at<float>(dy,dx);
      	
			// Comparaison distance Faire un define
			float delta = std::abs(radius*2 - dx - dy);
			float w = exp(delta/(0.5*radius))*exp(-pow (ac - amid ,2)/0.1) * exp(-pow(bc - bmid ,2)/0.1);
			// Comparaison des différents niveaux de couleurs RGB
			cv::Point3_<uchar>* pa = a->ptr<cv::Point3_<uchar> >(dy,dx);
			cv::Point3_<uchar>* pb = b->ptr<cv::Point3_<uchar> >(dy,dx);
			
			float dr = abs(pa->z - pb->z);
			float dg = abs(pa->y - pb->y);
			float db = abs(pa->x - pb->x);
     	 	
			float erreur = dr + dg + db;
			// Utilisation d'une foction robuste: noyau gaussien
			float C = kernel[dy][dx] * erreur;
			// Facteur de normalisation
			W += w;
			answer += w * C;
		}
	}
	return answer/W;
}



