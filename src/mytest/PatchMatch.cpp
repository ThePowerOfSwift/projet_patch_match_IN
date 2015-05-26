#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <climits>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "PatchMatch.h"
#include "MSE.h"
#include "brent.h"
#include "BITMAP.h"
#include "knn.h"
#include "displayKNNField.h"

int patch_w = 7;
int pm_iters = 5;
int rs_max = INT_MAX;

//constantes pour le brent
float a_brent = 0.0f;
float b_brent = 180.0f;
float t_brent = 0.000001f;
float eps_brent = 10.0f * (float)sqrt((double)t_brent);
float x_brent = 0.0f;

const int k = 5;

#define XY_TO_INT(x, y) (((y)<<12)|(x))
#define INT_TO_X(v) ((v)&((1<<12)-1))
#define INT_TO_Y(v) ((v)>>12)

/*
 *
 * PATCH MATCH MODE 1
 *
 *
 * Match image a to image b, returning the nearest neighbor field mapping a => b coords, stored in an RGB 24-bit image as (by<<12)|bx. 
 *
 *
**/
void patchmatch(BITMAP * a, BITMAP * b, BITMAP * ann, BITMAP * annd)
{

	/* Initialize with random nearest neighbor field (NNF). */
	ann = new BITMAP(a->w, a->h);
	annd = new BITMAP(a->w, a->h);
	int aew = a->w - patch_w + 1, aeh = a->h - patch_w + 1;	/* Effective width and height (possible upper left corners of patches). */
	int bew = b->w - patch_w + 1, beh = b->h - patch_w + 1;
	memset(ann->data, 0, sizeof(int) * a->w * a->h);
	memset(annd->data, 0, sizeof(int) * a->w * a->h);
	for (int ay = 0; ay < aeh; ay++) {
		for (int ax = 0; ax < aew; ax++) {
			int bx = rand() % bew;
			int by = rand() % beh;
			(*ann)[ay][ax] = XY_TO_INT(bx, by);
			(*annd)[ay][ax] = dist(a, b, ax, ay, bx, by, patch_w);
		}
	}

	for (int iter = 0; iter < pm_iters; iter++) {
		/* In each iteration, improve the NNF, by looping in scanline or reverse-scanline order. */
		int ystart = 0, yend = aeh, ychange = 1;
		int xstart = 0, xend = aew, xchange = 1;
		if (iter % 2 == 1) {
			xstart = xend - 1;
			xend = -1;
			xchange = -1;
			ystart = yend - 1;
			yend = -1;
			ychange = -1;
		}

		for (int ay = ystart; ay != yend; ay += ychange) {
			for (int ax = xstart; ax != xend; ax += xchange) {
				/* Current (best) guess. */
				int v = (*ann)[ay][ax];
				int xbest = INT_TO_X(v), ybest = INT_TO_Y(v);
				int dbest = (*annd)[ay][ax];

				/* Propagation: Improve current guess by trying instead correspondences from left and above (below and right on odd iterations). */
				if ((unsigned)(ax - xchange) < (unsigned)aew) {
					int vp = (*ann)[ay][ax - xchange];
					int xp = INT_TO_X(vp) + xchange, yp =
					    INT_TO_Y(vp);
					if ((unsigned)xp < (unsigned)bew) {
						improve_guess(a, b, ax, ay,
							      xbest, ybest,
							      dbest, xp, yp,
							      patch_w);
					}
				}

				if ((unsigned)(ay - ychange) < (unsigned)aeh) {
					int vp = (*ann)[ay - ychange][ax];
					int xp = INT_TO_X(vp), yp =
					    INT_TO_Y(vp) + ychange;
					if ((unsigned)yp < (unsigned)beh)
						improve_guess(a, b, ax, ay,
							      xbest, ybest,
							      dbest, xp, yp,
							      patch_w);
				}
				/* Random search: Improve current guess by searching in boxes of exponentially decreasing size around the current best guess. */
				int rs_start = rs_max;
				if (rs_start > MAX(b->w, b->h)) {
					rs_start = MAX(b->w, b->h);
				}
				for (int mag = rs_start; mag >= 1; mag /= 2) {
					/* Sampling window */
					int xmin = MAX(xbest - mag, 0), xmax =
					    MIN(xbest + mag + 1, bew);
					int ymin = MAX(ybest - mag, 0), ymax =
					    MIN(ybest + mag + 1, beh);
					int xp = xmin + rand() % (xmax - xmin);
					int yp = ymin + rand() % (ymax - ymin);
					improve_guess(a, b, ax, ay, xbest,
						      ybest, dbest, xp, yp,
						      patch_w);
				}

				(*ann)[ay][ax] = XY_TO_INT(xbest, ybest);
				(*annd)[ay][ax] = dbest;
			}
		}
	}
	printf("Saving output images\n");
	save_bitmap(ann, "results/ann_MODE1.jpg");
	save_bitmap(annd, "results/annd_MODE1.jpg");
}

/*
 *
 * PATCH MATCH MODE 2
 *
 *
 * Match image a to image b, returning the nearest neighbor field mapping a => b coords, stored in an RGB 24-bit image as (by<<12)|bx. 
 * With OpenCV containers
 *
**/
void patchmatch(cv::Mat * a, cv::Mat * b, cv::Mat * ann, cv::Mat * annd)
{
	/* Initialize with random nearest neighbor field (NNF). */
	ann = new cv::Mat_ < int >(a->cols, a->rows);
	annd = new cv::Mat_ < int >(a->cols, a->rows);
	int aew = a->cols - patch_w + 1, aeh = a->rows - patch_w + 1;	/* Effective width and height (possible upper left corners of patches). */
	int bew = b->cols - patch_w + 1, beh = b->rows - patch_w + 1;

	for (int ay = 0; ay < aeh; ay++) {
		for (int ax = 0; ax < aew; ax++) {
			int bx = rand() % bew;
			int by = rand() % beh;
			ann->at < int >(ay, ax) = XY_TO_INT(bx, by);
			annd->at < int >(ay, ax) =
			    dist(a, b, ax, ay, bx, by, patch_w);
		}
	}
	for (int iter = 0; iter < pm_iters; iter++) {
		/* In each iteration, improve the NNF, by looping in scanline or reverse-scanline order. */
		int ystart = 0, yend = aeh, ychange = 1;
		int xstart = 0, xend = aew, xchange = 1;
		if (iter % 2 == 1) {
			xstart = xend - 1;
			xend = -1;
			xchange = -1;
			ystart = yend - 1;
			yend = -1;
			ychange = -1;
		}
		for (int ay = ystart; ay != yend; ay += ychange) {
			for (int ax = xstart; ax != xend; ax += xchange) {
				/* Current (best) guess. */
				int v = ann->at < int >(ay, ax);
				int xbest = INT_TO_X(v), ybest = INT_TO_Y(v);
				int dbest = annd->at < int >(ay, ax);

				/* Propagation: Improve current guess by trying instead correspondences from left and above (below and right on odd iterations). */
				if ((unsigned)(ax - xchange) < (unsigned)aew) {
					int vp =
					    ann->at < int >(ay, ax - xchange);
					int xp = INT_TO_X(vp) + xchange, yp =
					    INT_TO_Y(vp);
					if ((unsigned)xp < (unsigned)bew) {
						improve_guess(a, b, ax, ay,
							      xbest, ybest,
							      dbest, xp, yp,
							      patch_w);
					}
				}

				if ((unsigned)(ay - ychange) < (unsigned)aeh) {
					int vp =
					    ann->at < int >(ay - ychange, ax);
					int xp = INT_TO_X(vp), yp =
					    INT_TO_Y(vp) + ychange;
					if ((unsigned)yp < (unsigned)beh) {
						improve_guess(a, b, ax, ay,
							      xbest, ybest,
							      dbest, xp, yp,
							      patch_w);
					}
				}

				/* Random search: Improve current guess by searching in boxes of exponentially decreasing size around the current best guess. */
				int rs_start = rs_max;
				if (rs_start > MAX(b->cols, b->rows)) {
					rs_start = MAX(b->cols, b->rows);
				}
				for (int mag = rs_start; mag >= 1; mag /= 2) {
					/* Sampling window */
					int xmin = MAX(xbest - mag, 0), xmax =
					    MIN(xbest + mag + 1, bew);
					int ymin = MAX(ybest - mag, 0), ymax =
					    MIN(ybest + mag + 1, beh);
					int xp = xmin + rand() % (xmax - xmin);
					int yp = ymin + rand() % (ymax - ymin);

					improve_guess(a, b, ax, ay, xbest,
						      ybest, dbest, xp, yp,
						      patch_w);
				}

				ann->at < int >(ax, ay) =
				    XY_TO_INT(xbest, ybest);
				annd->at < int >(ax, ay) = dbest;
			}
		}
	}
	printf("Saving output images\n");
	BITMAP *test1 = matToBITMAP(*annd);
	BITMAP *test2 = matToBITMAP(*ann);

	save_bitmap(test1, "annd_matToBITMAP_Mode2.jpg");
	save_bitmap(test2, "ann_matToBITMAP_Mode2.jpg");
	delete test1;
	delete test2;

	cv::imwrite("ann_mat_opencv_Mode2.png", *ann);
	cv::imwrite("annd_mat_opencv_Mode2.png", *annd);
}

/*
 *
 * PATCH MATCH MODE 3
 *
 *
 * Match image a to image b, returning the nearest neighbor field mapping a => b coords, stored in an RGB 24-bit image as (by<<12)|bx. 
 * With OpenCV containers & Brent & Rotations
 *
**/
/* Match image a to image b, returning the nearest neighbor field mapping a => b coords, stored in an RGB 24-bit image as (by<<12)|bx. */
void patchmatch_brent(cv::Mat * a, cv::Mat * b, cv::Mat * ann, cv::Mat * annd)
{
	/* Initialize with random nearest neighbor field (NNF). */
	ann = new cv::Mat_ < int >(a->cols, a->rows);
	annd = new cv::Mat_ < int >(a->cols, a->rows);

	int aew = a->cols - patch_w + 1, aeh = a->rows - patch_w + 1;	/* Effective width and height (possible upper left corners of patches). */
	int bew = b->cols - patch_w + 1, beh = b->rows - patch_w + 1;

	for (int ay = 0; ay < aeh; ay++) {
		for (int ax = 0; ax < aew; ax++) {
			int bx = rand() % bew;
			int by = rand() % beh;
			ann->at < int >(ay, ax) = XY_TO_INT(bx, by);
			annd->at < int >(ay, ax) =
			    brent(a, b, a_brent, b_brent, eps_brent, t_brent,
				  &x_brent, ax, ay, bx, by, patch_w);
		}
	}

	for (int iter = 0; iter < pm_iters; iter++) {
		/* In each iteration, improve the NNF, by looping in scanline or reverse-scanline order. */
		int ystart = 0, yend = aeh, ychange = 1;
		int xstart = 0, xend = aew, xchange = 1;
		if (iter % 2 == 1) {
			xstart = xend - 1;
			xend = -1;
			xchange = -1;
			ystart = yend - 1;
			yend = -1;
			ychange = -1;
		}

		for (int ay = ystart; ay != yend; ay += ychange) {
			for (int ax = xstart; ax != xend; ax += xchange) {
				/* Current (best) guess. */
				int v = ann->at < int >(ay, ax);
				int xbest = INT_TO_X(v), ybest = INT_TO_Y(v);
				int dbest = annd->at < int >(ay, ax);

				/* Propagation: Improve current guess by trying instead correspondences from left and above (below and right on odd iterations). */
				if ((unsigned)(ax - xchange) < (unsigned)aew) {
					int vp =
					    ann->at < int >(ay, ax - xchange);
					int xp = INT_TO_X(vp) + xchange, yp =
					    INT_TO_Y(vp);
					if ((unsigned)xp < (unsigned)bew)
						improve_guess(a, b, xbest,
							      ybest, dbest, xp,
							      yp);
				}

				if ((unsigned)(ay - ychange) < (unsigned)aeh) {
					int vp =
					    ann->at < int >(ay - ychange, ax);
					int xp = INT_TO_X(vp), yp =
					    INT_TO_Y(vp) + ychange;
					if ((unsigned)yp < (unsigned)beh)
						improve_guess(a, b, xbest,
							      ybest, dbest, xp,
							      yp);
				}

				/* Random search: Improve current guess by searching in boxes of exponentially decreasing size around the current best guess. */
				int rs_start = rs_max;
				if (rs_start > MAX(b->cols, b->rows)) {
					rs_start = MAX(b->cols, b->rows);
				}
				for (int mag = rs_start; mag >= 1; mag /= 2) {
					/* Sampling window */
					int xmin = MAX(xbest - mag, 0), xmax =
					    MIN(xbest + mag + 1, bew);
					int ymin = MAX(ybest - mag, 0), ymax =
					    MIN(ybest + mag + 1, beh);
					int xp = xmin + rand() % (xmax - xmin);
					int yp = ymin + rand() % (ymax - ymin);
					improve_guess(a, b, xbest, ybest, dbest,
						      xp, yp);
				}

				ann->at < int >(ax, ay) =
				    XY_TO_INT(xbest, ybest);
				annd->at < int >(ax, ay) = dbest;
			}
		}
	}
	printf("Saving output images\n");
	BITMAP *test1 = matToBITMAP(*annd);
	BITMAP *test2 = matToBITMAP(*ann);

	save_bitmap(test1, "results/annd_matToBITMAP_Mode3.jpg");
	save_bitmap(test2, "results/ann_matToBITMAP_Mode3.jpg");
	delete test1;
	delete test2;

	cv::imwrite("results/ann_mat_opencv_Mode3.jpg", *ann);
	cv::imwrite("results/annd_mat_opencv_Mode3.jpg", *annd);
}

/*
 *
 * PATCH MATCH MODE 4
 *
 *
 * Match image a to image b, returning the nearest neighbor field mapping a => b coords, stored in an RGB 24-bit image as (by<<12)|bx. 
 * With OpenCV containers & Brent & Rotations & KNN
 *
**/
/* Match image a to image b, returning the nearest neighbor field mapping a => b coords, stored in an RGB 24-bit image as (by<<12)|bx. */
void
patchmatch(cv::Mat * a, cv::Mat * b, cv::Mat * ann, cv::Mat * annd,
	   cv::Mat * knn, cv::Mat * knnd)
{
	/* Initialize with random nearest neighbor field (NNF). */
	ann = new cv::Mat_ < int >(a->cols, a->rows);
	annd = new cv::Mat_ < int >(a->cols, a->rows);
	knn = new cv::Mat_ < cv::Vec < int, k > >(a->cols, a->rows);
	knnd = new cv::Mat_ < cv::Vec < int, k > >(a->cols, a->rows);

	int aew = a->cols - patch_w + 1, aeh = a->rows - patch_w + 1;	/* Effective width and height (possible upper left corners of patches). */
	int bew = b->cols - patch_w + 1, beh = b->rows - patch_w + 1;

	//generalizedAnnStruct kNN[a->rows * a->cols];

	for (int ay = 0; ay < aeh; ay++) {
		for (int ax = 0; ax < aew; ax++) {
			int bx = rand() % bew;
			int by = rand() % beh;
			ann->at < int >(ay, ax) = XY_TO_INT(bx, by);
			annd->at < int >(ay, ax) =
			    brent(a, b, a_brent, b_brent, eps_brent, t_brent,
				  &x_brent, ax, ay, bx, by, patch_w);
		}
	}

	//initialisation de knn et knnd  
	for (int ay = 0; ay < aeh; ay++)
		for (int ax = 0; ax < aew; ax++) {
			(knn->at < cv::Vec < int, k > >(ay, ax)) =
			    cv::Vec < int, k > ();
			(knnd->at < cv::Vec < int, k > >(ay, ax)) =
			    cv::Vec < int, k > ();

			for (int i = 0; i < k; i++) {
				int bx = rand() % bew;
				int by = rand() % beh;

				(knn->at < cv::Vec < int, k > >(ay, ax))[i] =
				    XY_TO_INT(bx, by);
				(knnd->at < cv::Vec < int, k > >(ay, ax))[i] =
				    brent(a, b, a_brent, b_brent, eps_brent,
					  t_brent, &x_brent, ax, ay, bx, by,
					  patch_w);
			}
		}

	//rangement des voisins
	for (int y = 0; y < aeh; y++) {
		for (int x = 0; x < aew; x++) {
			trier_voisins_init(knn, knnd, x, y);
		}
	}

	for (int iter = 0; iter < pm_iters; iter++) {
		/* In each iteration, improve the NNF, by looping in scanline or reverse-scanline order. */
		int ystart = 0, yend = aeh, ychange = 1;
		int xstart = 0, xend = aew, xchange = 1;
		if (iter % 2 == 1) {
			xstart = xend - 1;
			xend = -1;
			xchange = -1;
			ystart = yend - 1;
			yend = -1;
			ychange = -1;
		}

		for (int ay = ystart; ay != yend; ay += ychange) {
			for (int ax = xstart; ax != xend; ax += xchange) {
				/* Current (best) guess. */
				int v = ann->at < int >(ay, ax);
				int xbest = INT_TO_X(v), ybest = INT_TO_Y(v);
				v = (knn->at < cv::Vec < int, k > >(ay, ax))[k - 1];	//on récupère le moins intéressant de la liste des k voisin du pixel actuel
				int kxbest = INT_TO_X(v), kybest = INT_TO_Y(v);
				int dbest = annd->at < int >(ay, ax);
				int kdbest = (knnd->at < cv::Vec < int, k > >(ay, ax))[k - 1];	//on récupère le moins intéressant de la liste des k voisin du pixel actuel
				bool guess_ok = false;

				/* Propagation: Improve current guess by trying instead correspondences from left and above (below and right on odd iterations). */
				if ((unsigned)(ax - xchange) < (unsigned)aew) {
					int vp =
					    ann->at < int >(ay, ax - xchange);
					int xp = INT_TO_X(vp) + xchange, yp =
					    INT_TO_Y(vp);
					if ((unsigned)xp < (unsigned)bew) {
						improve_guess(a, b, xbest,
							      ybest, dbest, xp,
							      yp);
						k_improve_guess(a, b, xbest,
								ybest, kdbest,
								xp, yp,
								guess_ok);
					}
				}

				if ((unsigned)(ay - ychange) < (unsigned)aeh) {
					int vp =
					    ann->at < int >(ay - ychange, ax);
					int xp = INT_TO_X(vp), yp =
					    INT_TO_Y(vp) + ychange;
					if ((unsigned)yp < (unsigned)beh) {
						improve_guess(a, b, xbest,
							      ybest, dbest, xp,
							      yp);
						k_improve_guess(a, b,
								/*ax,ay, */
								xbest, ybest,
								kdbest, xp, yp,
								guess_ok);
					}
				}

				/* Random search: Improve current guess by searching in boxes of exponentially decreasing size around the current best guess. */
				int rs_start = rs_max;
				if (rs_start > MAX(b->cols, b->rows)) {
					rs_start = MAX(b->cols, b->rows);
				}
				for (int mag = rs_start; mag >= 1; mag /= 2) {
					/* Sampling window */
					int xmin = MAX(xbest - mag, 0), xmax =
					    MIN(xbest + mag + 1, bew);
					int ymin = MAX(ybest - mag, 0), ymax =
					    MIN(ybest + mag + 1, beh);
					int xp = xmin + rand() % (xmax - xmin);
					int yp = ymin + rand() % (ymax - ymin);
					improve_guess(a, b, xbest, ybest, dbest,
						      xp, yp);
					k_improve_guess(a, b, xbest, ybest,
							kdbest, xp, yp,
							guess_ok);

				}

				//kNN[xend * yend].dx[0] = xbest;
				//kNN[xend * yend].dy[0] = ybest;

				ann->at < int >(ax, ay) =
				    XY_TO_INT(xbest, ybest);
				annd->at < int >(ax, ay) = dbest;

				if (guess_ok) {
					inserer_fin(ax, ay, kxbest, kybest,
						    kdbest, knn, knnd);
					trier_voisins(knn, knnd, ax, ay);
				}
			}
		}
	}

	//sauvegarde des meilleurs voisins
	cv::Mat * knn1 = new cv::Mat_ < cv::Vec < int, k > >(a->cols, a->rows);;
	cv::Mat * knnd1 =
	    new cv::Mat_ < cv::Vec < int, k > >(a->cols, a->rows);;

	for (int ay = 0; ay < aeh; ay++)
		for (int ax = 0; ax < aew; ax++) {
			(knn1->at < cv::Vec < int, k > >(ay, ax)) =
			    (knn->at < cv::Vec < int, k > >(ay, ax))[0];
			(knnd1->at < cv::Vec < int, k > >(ay, ax)) =
			    (knnd->at < cv::Vec < int, k > >(ay, ax))[0];
		}

	printf("Saving output images\n");
	BITMAP *test1 = matToBITMAP(*annd);
	BITMAP *test2 = matToBITMAP(*ann);

	save_bitmap(test1, "results/annd_matToBITMAP_Mode4.jpg");
	save_bitmap(test2, "results/ann_matToBITMAP_Mode4.jpg");
	delete test1;
	delete test2;

	cv::imwrite("results/ann_mat_opencv_Mode4.jpg", *ann);
	cv::imwrite("results/annd_mat_opencv_Mode4.jpg", *annd);
	cv::imwrite("results/knn_mat_opencv_Mode4.jpg", *knn1);
	cv::imwrite("results/knnd_mat_opencv_Mode4.jpg", *knnd1);

	//lchar test[10] = "test.png\0";
	//displayMotionField(kNN, aew, aeh, test, patch_w, 0);
}
