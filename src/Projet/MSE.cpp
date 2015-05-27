#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <cmath>
#include <climits>
#include <iostream>

#include "MSE.h"
#include "BITMAP.h"
#include "brent.h"

#define XY_TO_INT(x, y) (((y)<<12)|(x))
#define INT_TO_X(v) ((v)&((1<<12)-1))
#define INT_TO_Y(v) ((v)>>12)
//remplacer dist par brent
void improve_guess(cv::Mat * a, cv::Mat * b, float a_brent, float b_brent, float eps_brent,
      float t_brent, float *x_brent, int ax, int ay, int bx, int by, int patch_w, int &dbest, int &xbest, int &ybest)
{
	int d = brent(a, b, a_brent, b_brent, eps_brent, t_brent, x_brent, ax, ay, bx, by, patch_w);
	if (d < dbest) {
		dbest = d;
		xbest = bx;
		ybest = by;
	}
}

void
improve_guess(cv::Mat * a, cv::Mat * b, int ax, int ay, int &xbest,
	      int &ybest, int &dbest, int bx, int by, int patch_w)
{
	int d = dist(a, b, ax, ay, bx, by, patch_w, dbest);
	if (d < dbest) {
		dbest = d;
		xbest = bx;
		ybest = by;
	}
}

void
improve_guess(BITMAP * a, BITMAP * b, int ax, int ay, int &xbest,
	      int &ybest, int &dbest, int bx, int by, int patch_w)
{
	int d = dist(a, b, ax, ay, bx, by, patch_w, dbest);
	if (d < dbest) {
		dbest = d;
		xbest = bx;
		ybest = by;
	}
}

void gaussianKernel(float kernel[][7])
{
	float mean = 3;
	float sum = 0.0;
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			kernel[i][j] =
			    exp(-0.5 *
				(pow((i - mean), 2.0) +
				 pow((j - mean), 2.0))) / (2 * M_PI);
			sum += kernel[i][j];
		}
	}
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			kernel[i][j] /= sum;
		}
	}
}

<<<<<<< HEAD
float dist(cv::Mat * a, cv::Mat * b, float cutoff)
=======
float dist(cv::Mat * a, cv::Mat * b)
>>>>>>> eb2f56540b6c3d22932ae53177e6b4308435da1e
{
	float answer = 0;
	int size = a->size().height;
	int radius = size / 2;
	int W = 0;
	int amid = a->at < int >(3, 3);
	int bmid = b->at < int >(3, 3);
	float variance = 34.641016151; // (Racine de 3) * 20

	for (int dy = 0; dy < size; dy++) {
		for (int dx = 0; dx < size; dx++) {

			int ac = a->at < int >(dy, dx);
			int bc = b->at < int >(dy, dx);

			// Comparaison distance Faire un define
			float delta = std::abs(radius * 2 - dx - dy);
			float w =
			    exp(delta / (0.5 * radius)) *
			    exp(-pow(ac - amid, 2) / 0.1) *
			    exp(-pow(bc - bmid, 2) / 0.1);
			// Comparaison des différents niveaux de couleurs RGB
			cv::Point3_ < uchar > *pa =
			    a->ptr < cv::Point3_ < uchar > >(dy, dx);
			cv::Point3_ < uchar > *pb =
			    b->ptr < cv::Point3_ < uchar > >(dy, dx);

			float dr = abs(pa->z - pb->z);
			float dg = abs(pa->y - pb->y);
			float db = abs(pa->x - pb->x);

			float erreur = dr + dg + db;
			// Utilisation d'une foction robuste: noyau gaussien
			float C = log(1 + 1/2*(erreur/variance));
			// Facteur de normalisation
			W += w;
			answer += w * C;
		}
	}
	if (answer >= cutoff) {
		return cutoff;
	}
	return answer / W;
}

/* dist marchant avec l'image et les coordonnées du patch */
float dist(cv::Mat * a, cv::Mat * b, int ax, int ay, int bx, int by, float cutoff)
{
	float answer = 0;
	int size = a->size().height;
	int radius = size / 2;
	int W = 0;
	int amid = a->at < int >(ay+3, ax+3);
	int bmid = b->at < int >(by+3, bx+3);
	float variance = 34.641016151; // (Racine de 3) * 20

	for (int dy = 0; dy < size; dy++) {
		for (int dx = 0; dx < size; dx++) {

			int ac = a->at < int >(ay+dy, ax+dx);
			int bc = b->at < int >(by+dy, bx+dx);

			// Comparaison distance Faire un define
			float delta = std::abs(radius * 2 - dx - dy);
			float w =
			    exp(delta / (0.5 * radius)) *
			    exp(-pow(ac - amid, 2) / 0.1) *
			    exp(-pow(bc - bmid, 2) / 0.1);
			// Comparaison des différents niveaux de couleurs RGB
			cv::Point3_ < uchar > *pa =
			    a->ptr < cv::Point3_ < uchar > >(ay+dy, ax+dx);
			cv::Point3_ < uchar > *pb =
			    b->ptr < cv::Point3_ < uchar > >(by+dy, bx+dx);

			float dr = abs(pa->z - pb->z);
			float dg = abs(pa->y - pb->y);
			float db = abs(pa->x - pb->x);

			float erreur = dr + dg + db;
			// Utilisation d'une foction robuste
			float C = log(1 + 1/2*(erreur/variance));
			// Facteur de normalisation
			W += w;
			answer += w * C;
		}
	}
	if (answer >= cutoff) {
		return cutoff;
	}
	return answer / W;
}

/* Measure distance between 2 patches with upper left corners (ax, ay) and (bx, by), terminating early if we exceed a cutoff distance.
   You could implement your own descriptor here. */
float
dist(BITMAP * a, BITMAP * b, int ax, int ay, int bx, int by, int patch_w, int cutoff)
{
	int ans = 0;
	for (int dy = 0; dy < patch_w; dy++) {
		int *arow = &(*a)[ay + dy][ax];
		int *brow = &(*b)[by + dy][bx];
		for (int dx = 0; dx < patch_w; dx++) {
			int ac = arow[dx];
			int bc = brow[dx];
			int dr = (ac & 255) - (bc & 255);
			int dg = ((ac >> 8) & 255) - ((bc >> 8) & 255);
			int db = (ac >> 16) - (bc >> 16);
			ans += dr * dr + dg * dg + db * db;
		}
		if (ans >= cutoff) {
			return cutoff;
		}
	}
	return ans;
}

/* Measure distance between 2 patches with upper left corners (ax, ay) and (bx, by), terminating early if we exceed a cutoff distance.
   You could implement your own descriptor here. */
float dist(cv::Mat * a, cv::Mat * b, int ax, int ay, int bx, int by, int patch_w, int cutoff)
{
	int ans = 0;
	for (int dy = 0; dy < patch_w; dy++) {
		cv::Mat arow = a->row(ay + dy);
		cv::Mat brow = b->row(by + dy);
		for (int dx = 0; dx < patch_w; dx++) {
			int ac = arow.at < int >(ax + dx);
			int bc = brow.at < int >(bx + dx);
			int dr = (ac & 255) - (bc & 255);
			int dg = ((ac >> 8) & 255) - ((bc >> 8) & 255);
			int db = (ac >> 16) - (bc >> 16);
			ans += dr * dr + dg * dg + db * db;
		}
		if (ans >= cutoff) {
			return cutoff;
		}
	}
	return ans;
}
