
#include "MSE.h"

int dist ( BITMAP *a, BITMAP *b, int ax, int ay, int bx, int by, int cutoff){
	int answer =0;
	int size =7;
	int radius = size/2;
	int W=0;
	for (int dy = 0; dy < size; dy++) {
    	int *arow = &(*a)[ay+dy][ax];
    	int *brow = &(*b)[by+dy][bx];
    	for (int dx = 0; dx < size; dx++) {
      	int ac = arow[dx];
      	int bc = brow[dx];
		
			float delta = std::abs(dx + dy - radius*2);
			float w = exp(delta/(0.5*radius))*exp(-pow (ac - (*a)[3][3],2)/0.1) * exp(-pow(bc - (*b)[3][3],2)/0.1);

			int dr = (ac&255)-(bc&255);
      	int dg = ((ac>>8)&255)-((bc>>8)&255);
     	 	int db = (ac>>16)-(bc>>16);
     	 	
			float erreur = dr + dg + db;
			float C =1/(0.2*sqrt(2*M_PI)*exp(-(erreur*erreur))/(2*0.2*0.2)); ;

				W += w;
				answer += w * C;
		}
	}
	return answer/W;
}



