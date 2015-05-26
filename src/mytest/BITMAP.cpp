#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <climits>
#include <iostream>
#include <opencv2/core/core.hpp>

#include "BITMAP.h"

/* -------------------------------------------------------------------------
   BITMAP: Minimal image class
   ------------------------------------------------------------------------- */

BITMAP::BITMAP(int w_, int h_):w(w_), h(h_)
{
	data = new int[w * h];
}

BITMAP::~BITMAP()
{
	delete[]data;
}

int *BITMAP::operator[] (int y) {
	return &data[y * w];
}

void check_im()
{
	if (system("identify > null.txt") != 0) {
		fprintf(stderr,
			"ImageMagick must be installed, and 'convert' and 'identify' must be in the path\n");
		exit(1);
	}
}

BITMAP *load_bitmap(const char *filename)
{
	check_im();
	char rawname[256], txtname[256];
	strcpy(rawname, filename);
	strcpy(txtname, filename);
	if (!strstr(rawname, ".")) {
		fprintf(stderr,
			"Error reading image '%s': no extension found\n",
			filename);
		exit(1);
	}
	sprintf(strstr(rawname, "."), ".raw");
	sprintf(strstr(txtname, "."), ".txt");
	char buf[256];
	sprintf(buf, "convert %s rgba:%s", filename, rawname);
	if (system(buf) != 0) {
		fprintf(stderr,
			"Error reading image '%s': ImageMagick convert gave an error\n",
			filename);
		exit(1);
	}
	sprintf(buf, "identify -format \"%%w %%h\" %s > %s", filename, txtname);
	if (system(buf) != 0) {
		fprintf(stderr,
			"Error reading image '%s': ImageMagick identify gave an error\n",
			filename);
		exit(1);
	}
	FILE *f = fopen(txtname, "rt");
	if (!f) {
		fprintf(stderr,
			"Error reading image '%s': could not read output of ImageMagick identify\n",
			filename);
		exit(1);
	}
	int w = 0, h = 0;
	if (fscanf(f, "%d %d", &w, &h) != 2) {
		fprintf(stderr,
			"Error reading image '%s': could not get size from ImageMagick identify\n",
			filename);
		exit(1);
	}
	fclose(f);
	f = fopen(rawname, "rb");
	BITMAP *ans = new BITMAP(w, h);
	unsigned char *p = (unsigned char *)ans->data;
	for (int i = 0; i < w * h * 4; i++) {
		int ch = fgetc(f);
		if (ch == EOF) {
			fprintf(stderr,
				"Error reading image '%s': raw file is smaller than expected size %dx%dx4\n",
				filename, w, h /*, 4 */ );
			exit(1);
		}
		*p++ = ch;
	}
	fclose(f);
	return ans;
}

void save_bitmap(BITMAP * bmp, const char *filename)
{
	check_im();
	char rawname[256];
	strcpy(rawname, filename);
	if (!strstr(rawname, ".")) {
		fprintf(stderr,
			"Error writing image '%s': no extension found\n",
			filename);
		exit(1);
	}
	sprintf(strstr(rawname, "."), ".raw");
	char buf[256];
	FILE *f = fopen(rawname, "wb");
	if (!f) {
		fprintf(stderr,
			"Error writing image '%s': could not open raw temporary file\n",
			filename);
		exit(1);
	}
	unsigned char *p = (unsigned char *)bmp->data;
	for (int i = 0; i < bmp->w * bmp->h * 4; i++) {
		fputc(*p++, f);
	}
	fclose(f);
	sprintf(buf, "convert -size %dx%d -depth 8 rgba:%s %s", bmp->w, bmp->h,
		rawname, filename);
	if (system(buf) != 0) {
		fprintf(stderr,
			"Error writing image '%s': ImageMagick convert gave an error\n",
			filename);
		exit(1);
	}
}

BITMAP* matToBITMAP(cv::Mat a){
  BITMAP *bmp = new BITMAP(a.cols, a.rows);
  for (int i = 0; i < bmp->h; ++i)
    for (int j = 0; j < bmp->w; ++j)
      (*bmp)[i][j] = a.at<int>(i,j);
  
  return bmp;
}

cv::Mat BITMAPToMat(BITMAP *a){
  cv::Mat_<int> mt(a->h, a->w);
  for (int i = 0; i < a->h; ++i)
    for (int j = 0; j < a->w; ++j)
      mt.at<int>(i,j) = (*a)[i][j];

  return mt;
}
