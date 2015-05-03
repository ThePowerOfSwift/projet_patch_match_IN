#pragma once

/* -------------------------------------------------------------------------
   BITMAP: Minimal image class
   ------------------------------------------------------------------------- */
   
class BITMAP{ 

public:
  int w, h;
  int *data;
  BITMAP(int w_, int h_);
  ~BITMAP();
  int *operator[](int y);
};

void check_im();
BITMAP *load_bitmap(const char *filename);
void save_bitmap(BITMAP *bmp, const char *filename);