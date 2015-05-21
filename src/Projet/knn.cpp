#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <climits>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "knn.h"
#include "MSE.h"

const int k = 5;

void k_improve_guess(cv::Mat *a, cv::Mat *b, int ax, int ay, int &xbest, int &ybest, int &kdbest, int bx, int by, bool &guess_ok) {
  
  int d=dist(a,b);
  if (d < kdbest) {
    guess_ok=true;
    kdbest=d;
    xbest=bx;
    ybest=by;
  }
}

void trier_voisins_init(cv::Mat *knn,cv::Mat *knnd, int x, int y){
  

  if((knnd->at<cv::Vec<float, k> >(y,x))[0]>(knnd->at<cv::Vec<float, k> >(y,x))[1]){
      float inter = (knnd->at<cv::Vec<float, k> >(y,x))[0];

      (knnd->at<cv::Vec<float, k> >(y,x))[0]=(knnd->at<cv::Vec<float, k> >(y,x))[1];

      (knnd->at<cv::Vec<float, k> >(y,x))[1] =inter;

      inter = (knn->at<cv::Vec<float, k> >(y,x))[0];

      (knn->at<cv::Vec<float, k> >(y,x))[0]=(knn->at<cv::Vec<float, k> >(y,x))[1];

      (knn->at<cv::Vec<float, k> >(y,x))[1] =inter;
  }
  

for(int i=2; i < k; i++){
    for(int j=0;j<=i;j++){

       float valeurd = (knnd->at<cv::Vec<float, k> >(y,x))[i];
       float valeur = (knn->at<cv::Vec<float, k> >(y,x))[i];

      if((knnd->at<cv::Vec<float, k> >(y,x))[j]>valeurd){
          float inter = (knnd->at<cv::Vec<float, k> >(y,x))[j];
          (knnd->at<cv::Vec<float, k> >(y,x))[j] = valeurd;
          (knnd->at<cv::Vec<float, k> >(y,x))[i] = inter;

          inter = (knn->at<cv::Vec<float, k> >(y,x))[j];
          (knn->at<cv::Vec<float, k> >(y,x))[j] = valeur;
          (knn->at<cv::Vec<float, k> >(y,x))[i] = inter;
      }
    }    
  }
}

void trier_voisins(cv::Mat *knn, cv::Mat *knnd, int x, int y){
  
  float valeur = (knnd->at<cv::Vec<float, k> >(y,x))[k-1];
  float inter = (knn->at<cv::Vec<float, k> >(y,x))[k-1];
  
  for(int i=k-2; i > -1;i--){
     if(valeur<(knnd->at<cv::Vec<float, k> >(y,x))[i]){
        (knnd->at<cv::Vec<float, k> >(y,x))[i+1]=(knnd->at<cv::Vec<float, k> >(y,x))[i];
        (knnd->at<cv::Vec<float, k> >(y,x))[i]=valeur;

        (knn->at<cv::Vec<float, k> >(y,x))[i+1]=(knn->at<cv::Vec<float, k> >(y,x))[i];
        (knn->at<cv::Vec<float, k> >(y,x))[i]=inter;

     }
     else
      break;
  }
}

void inserer_fin(int ax, int ay, int kxbest, int kybest, int kdbest,cv::Mat *knn,cv::Mat *knnd){
  std::cout << XY_TO_INT(kxbest, kybest) << std::endl;
    (knn->at<cv::Vec<float,k> >(ay,ax))[k-1] = XY_TO_INT(kxbest, kybest);
    (knnd->at<cv::Vec<float,k> >(ay,ax))[k-1] = kdbest;
}