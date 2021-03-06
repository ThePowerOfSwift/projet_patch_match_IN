#ifndef COLORCODE_H
#define COLORCODE_H
void computeColor(float fx, float fy, uchar * pix);
 
// the "official" threshold - if the absolute value of either 
// flow component is greater, it's considered unknown
#define UNKNOWN_FLOW_THRESH 1e9
    
// value to use to represent unknown flow
#define UNKNOWN_FLOW 1e10
    
// return whether flow vector is unknown
    bool unknown_flow(float u, float v);
bool unknown_flow(float *f);
 
#endif				//COLORCODE
