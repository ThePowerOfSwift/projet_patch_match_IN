
/* -------------------------------------------------------------------------
  Minimal (unoptimized) example of PatchMatch. Requires that ImageMagick be installed.

  To improve generality you can:
   - Use whichever distance function you want in dist(), e.g. compare SIFT descriptors computed densely.
   - Search over a larger search space, such as rotating+scaling patches (see MATLAB mex for examples of both)
  
  To improve speed you can:
   - Turn on optimizations (/Ox /Oi /Oy /fp:fast or -O6 -s -ffast-math -fomit-frame-pointer -fstrength-reduce -msse2 -funroll-loops)
   - Use the MATLAB mex which is already tuned for speed
   - Use multiple cores, tiling the input. See our publication "The Generalized PatchMatch Correspondence Algorithm"
   - Tune the distance computation: manually unroll loops for each patch size, use SSE instructions (see readme)
   - Precompute random search samples (to avoid using rand, and mod)
   - Move to the GPU
  -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <climits>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "pm_minimal.h"
#include "brent.h"
#include "BITMAP.h"
#include "knn.h" 
#include "displayKNNField.h"

using namespace cv;

/* -------------------------------------------------------------------------
   PatchMatch, using L2 distance between upright patches that translate only
   ------------------------------------------------------------------------- */

int patch_w  = 7;
int pm_iters = 5;
int rs_max   = INT_MAX;

//constantes pour le brent
float a_brent = 0.0f;
float b_brent = 180.0f;
float t_brent = 0.000001f;
float eps_brent = 10.0f*(float)sqrt((double)t_brent);
float * x_brent =0;

const int k = 5;

#define XY_TO_INT(x, y) (((y)<<12)|(x))
#define INT_TO_X(v) ((v)&((1<<12)-1))
#define INT_TO_Y(v) ((v)>>12)

/* Measure distance between 2 patches with upper left corners (ax, ay) and (bx, by), terminating early if we exceed a cutoff distance.
   You could implement your own descriptor here. */
/*int dist(BITMAP *a, BITMAP *b, int ax, int ay, int bx, int by, int cutoff) {
  int ans = 0;
  for (int dy = 0; dy < patch_w; dy++) {
    int *arow = &(*a)[ay+dy][ax];
    int *brow = &(*b)[by+dy][bx];
    for (int dx = 0; dx < patch_w; dx++) {
      int ac = arow[dx];
      int bc = brow[dx];
      int dr = (ac&255)-(bc&255);
      int dg = ((ac>>8)&255)-((bc>>8)&255);
      int db = (ac>>16)-(bc>>16);
      ans += dr*dr + dg*dg + db*db;
    }
    if (ans >= cutoff) { return cutoff; }
  }
  return ans;
}*/

void improve_guess(cv::Mat *a, cv::Mat *b, int ax, int ay, int &xbest, int &ybest, int &dbest, int bx, int by) {
  //int d = dist(a, b, ax, ay, bx, by, dbest);

  int d = brent (a, b, a_brent, b_brent, eps_brent, t_brent, x_brent, ax, ay, bx, by, patch_w);
  
  std::cout << "improve_guess __BRENT d : " << d << std::endl;
  
  if (d < dbest) {
    dbest = d;
    xbest = bx;
    ybest = by;
  }
}

/* Match image a to image b, returning the nearest neighbor field mapping a => b coords, stored in an RGB 24-bit image as (by<<12)|bx. */
void patchmatch(cv::Mat *a, cv::Mat *b, cv::Mat *ann, cv::Mat *annd,cv::Mat *knn, cv::Mat *knnd) {
  /* Initialize with random nearest neighbor field (NNF). */
  ann = new cv::Mat_<float>(a->cols, a->rows);
  annd = new cv::Mat_<float>(a->cols, a->rows);
  knn = new cv::Mat_< cv::Vec<float, k> >(a->cols, a->rows);
  knnd = new cv::Mat_< cv::Vec<float, k> >(a->cols, a->rows);

  int aew = a->cols - patch_w+1, aeh = a->rows - patch_w + 1;       /* Effective width and height (possible upper left corners of patches). */
  int bew = b->cols - patch_w+1, beh = b->rows - patch_w + 1;
  
  /*
  memset(ann->data, 0, sizeof(int)*a->cols*a->rows);
  memset(annd->data, 0, sizeof(int)*a->cols*a->rows);
  */
  
  generalizedAnnStruct kNN[a->rows * a->cols];

 

  

  memset(ann->data, 0, sizeof(int)*a->cols*a->rows);
  memset(annd->data, 0, sizeof(int)*a->cols*a->rows);

  for (int ay = 0; ay < aeh; ay++) {
    for (int ax = 0; ax < aew; ax++) {
      int bx = rand()%bew;
      int by = rand()%beh;
      ann->at<int>(ay,ax) = XY_TO_INT(bx, by);
      //SEGFAULT/
      annd->at<int>(ay,ax) = brent (a, b, a_brent, b_brent, eps_brent, t_brent, x_brent, ax, ay, bx, by, patch_w);
      //FIN SEGFAULT/ 
    }
  }

  //initialisation de knn et knnd  
  for (int ay = 0; ay < aeh; ay++) {
    for (int ax = 0; ax < aew; ax++) {
      (knn->at<cv::Vec<float, k> >(ay,ax))= cv::Vec<float, k>();
      (knnd->at<cv::Vec<float, k> >(ay,ax))= cv::Vec<float, k>();

      for (int i = 0; i < k; i++) {
          int bx = rand()%bew;
          int by = rand()%beh;

          (knn->at<cv::Vec<float, k> >(ay,ax))[i]= XY_TO_INT(bx, by);
          (knnd->at<cv::Vec<float, k> >(ay,ax))[i]= brent (a, b, a_brent, b_brent, eps_brent, t_brent, x_brent, ax, ay, bx, by, patch_w);
      }      
    }
  }

  //rangement des voisins
  for (int y = 0; y < aeh; y++) {
    for (int x = 0; x < aew; x++) {
      trier_voisins_init(knn,knnd,x,y);
    }
  }
  
  for (int iter = 0; iter < pm_iters; iter++) {
    /* In each iteration, improve the NNF, by looping in scanline or reverse-scanline order. */
    int ystart = 0, yend = aeh, ychange = 1;
    int xstart = 0, xend = aew, xchange = 1;
    if (iter % 2 == 1) {
      xstart = xend-1; xend = -1; xchange = -1;
      ystart = yend-1; yend = -1; ychange = -1;
    }
    
    for (int ay = ystart; ay != yend; ay += ychange) {
      for (int ax = xstart; ax != xend; ax += xchange) { 
        /* Current (best) guess. */
        int v = ann->at<float>(ay,ax);
        int xbest = INT_TO_X(v), ybest = INT_TO_Y(v);
        v = (knn->at<cv::Vec<float, k> >(ay,ax))[k-1];//on récupère le moins intéressant de la liste des k voisin du pixel actuel
        int kxbest = INT_TO_X(v), kybest = INT_TO_Y(v);
        int dbest = annd->at<float>(ay,ax);
        int kdbest = (knnd->at<cv::Vec<float, k> >(ay,ax))[k-1];//on récupère le moins intéressant de la liste des k voisin du pixel actuel
        bool guess_ok=false;

        /* Propagation: Improve current guess by trying instead correspondences from left and above (below and right on odd iterations). */
        if ((unsigned) (ax - xchange) < (unsigned) aew) {
          int vp = ann->at<int>(ay,ax-xchange);
          int xp = INT_TO_X(vp) + xchange, yp = INT_TO_Y(vp);
          if ((unsigned) xp < (unsigned) bew) {
            improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
            k_improve_guess(a,b,ax,ay,xbest,ybest,kdbest,xp,yp,guess_ok);

          }
        }

        if ((unsigned) (ay - ychange) < (unsigned) aeh) {
          int vp = ann->at<int>(ay-ychange,ax);
          int xp = INT_TO_X(vp), yp = INT_TO_Y(vp) + ychange;
          if ((unsigned) yp < (unsigned) beh) {
            improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
            k_improve_guess(a,b,ax,ay,xbest,ybest,kdbest,xp,yp,guess_ok);

          }
        }

        /* Random search: Improve current guess by searching in boxes of exponentially decreasing size around the current best guess. */
        int rs_start = rs_max;
        if (rs_start > MAX(b->cols, b->rows)) { rs_start = MAX(b->cols, b->rows); }
        for (int mag = rs_start; mag >= 1; mag /= 2) {
          /* Sampling window */
          int xmin = MAX(xbest-mag, 0), xmax = MIN(xbest+mag+1,bew);
          int ymin = MAX(ybest-mag, 0), ymax = MIN(ybest+mag+1,beh);
          int xp = xmin+rand()%(xmax-xmin);
          int yp = ymin+rand()%(ymax-ymin);
          improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
          k_improve_guess(a,b,ax,ay,xbest,ybest,kdbest,xp,yp,guess_ok);

        }
        
        kNN[xend * yend].dx[0] = xbest;
        kNN[xend * yend].dy[0] = ybest;
        
        ann->at<int>(ax,ay) = XY_TO_INT(xbest, ybest);
        annd->at<int>(ax,ay) = dbest;

        if(guess_ok){
          inserer_fin(ax,ay,kxbest,kybest,kdbest,knn,knnd);
          trier_voisins(knn,knnd,ax,ay);
        }
      }
    }
  }

  //sauvegarde des meilleurs voisins
  Mat * knn1 = new cv::Mat_< cv::Vec<float, k> >(a->cols, a->rows);;
  Mat * knnd1 = new cv::Mat_< cv::Vec<float, k> >(a->cols, a->rows);;

  for (int ay = 0; ay < aeh; ay++) {
    for (int ax = 0; ax < aew; ax++) {
      (knn1->at<cv::Vec<float, k> >(ay,ax))= (knn->at<cv::Vec<float, k> >(ay,ax))[0];
      (knnd1->at<cv::Vec<float, k> >(ay,ax))= (knnd->at<cv::Vec<float, k> >(ay,ax))[0];
    }
  }


  printf("Saving output images\n");
  BITMAP *test1 = matToBITMAP(*annd);
  BITMAP *test2 = matToBITMAP(*ann);
  BITMAP *test3 = matToBITMAP(*knnd1);
  BITMAP *test4 = matToBITMAP(*knn1);

  save_bitmap(test1, "results/anndTEST.jpg");
  save_bitmap(test2, "results/annTEST.jpg");
  save_bitmap(test3, "results/knndTEST.jpg");
  save_bitmap(test4, "results/knnTEST.jpg");

  delete test1;
  delete test2;
  delete test3;
  delete test4;


  cv::imwrite("results/ann.png", *ann);
  cv::imwrite("results/annd.png", *annd);

  cv::imwrite("results/knn.png", *knn1);
  cv::imwrite("results/knnd.png", *knnd1);
  
  char test[10] = "test.png\0";
  displayMotionField(kNN, aew, aeh, test, patch_w, 0);
}

int main(int argc, char *argv[]) {
  argc--;
  argv++;
  if (argc != 4) { fprintf(stderr, "pm_minimal a b ann annd\n"
                                   "Given input images a, b outputs nearest neighbor field 'ann' mapping a => b coords, and the squared L2 distance 'annd'\n"
                                   "These are stored as RGB 24-bit images, with a 24-bit int at every pixel. For the NNF we store (by<<12)|bx."); exit(1); }
  printf("Loading input images\n");
  BITMAP *aa = load_bitmap(argv[0]);
  BITMAP *bb = load_bitmap(argv[1]);
  
  Mat a = BITMAPToMat(aa);
  Mat b = BITMAPToMat(bb);

  Mat *ann = NULL, *annd = NULL, *knn = NULL, *knnd = NULL;

  printf("Running PatchMatch\n");
  patchmatch(&a, &b, ann, annd, knn, knnd);

  delete ann;
  delete annd;
  delete knn;
  delete knnd;

  return 0;
}
