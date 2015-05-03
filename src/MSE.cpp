#include "MSE.h"

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

float dist ( cv::Mat *a, cv::Mat *b, int ax, int ay, int bx, int by, int cutoff){
	float answer =0;
	int size =7;
	int radius = size/2;
	int W=0;
	float kernel [7][7];
	gaussianKernel(kernel);
	float amid = a->at<float>(3,3);
	float bmid = b->at<float>(3,3);
	for (int dy = 0; dy < size; dy++) {
    	for (int dx = 0; dx < size; dx++) {
      	float ac = a->at<float>(ay+dy,ax+dx);
      	float bc = b->at<float>(by+dy,bx+dx);
      	
			// Comparaison distance
			float delta = std::abs(radius*2 - dx - dy);
			float w = exp(delta/(0.5*radius))*exp(-pow (ac - amid ,2)/0.1) * exp(-pow(bc - bmid ,2)/0.1);
			// Comparaison des différents niveaux de couleurs RGB
			cv::Point3_<uchar>* pa = a->ptr<cv::Point3_<uchar> >(ay+dy,ax + dx);
			cv::Point3_<uchar>* pb = b->ptr<cv::Point3_<uchar> >(by+dy,bx + dx);
			float dr = abs(pa->z - pb->z);
			float dg = abs(pa->y - pb->y);
			float db = abs(pa->x - pb->x);
     	 	
			float erreur = dr + dg + db;
			
			// Utilisation d'une foction robuste: noyau gaussien
			//float C =1/(0.2*sqrt(2*M_PI)*exp(-(erreur*erreur))/(2*0.2*0.2));
			float C = kernel[dy][dx] * erreur;
			
			// Facteur de normalisation
			W += w;
			answer += w * C;
		}
	}	
	return answer/W;
}



