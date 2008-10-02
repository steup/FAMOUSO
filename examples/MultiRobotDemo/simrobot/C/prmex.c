#include <math.h>
#include "mex.h"
#include "matrix.h"





void ray(unsigned char grid[], int grid_rows, int grid_columns,
			int x0, int y0, int x1, int y1, double value, int *crashed,
         double robots[])
{
	int dx, dy, incrE, incrNE, d, incrx, incry, cnt, x, y;


  x = x0;
  y = y0;
  dx = x1 - x0;
  dy = y1 - y0;


  if (dx >= 0)
  	incrx = 1;
  else{
  	incrx = -1;
    dx = -dx;
  }
  if (dy >= 0)
  	incry = 1;
  else{
  	incry = -1;
    dy = -dy;
  }

	if (dy < dx){
		incrNE = (dy - dx) << 1;   		/* incrNE = (dy - dx) * 2;*/
	  	incrE = dy << 1;       	 			/* incrE = dy * 2; */
	  	d = incrE - dx;       				/* d = dy * 2 - dx; */
	  for(cnt = 1; (cnt <= dx) && (x < grid_rows) && (y < grid_columns)
     													&& (x > 1) && (y > 1); cnt++){
  		if( d <= 0 ) { 						/* Choose E pixel*/
    		d += incrE;
			}
  	  else {         							/* Choose NE pixel*/
				d += incrNE;
				y += incry;
			}
			x += incrx;
     if (grid[(y - 1)*grid_rows + x - 1] > 0) {
         *crashed = grid[(y - 1)*grid_rows + x - 1];
       if (robots[0] > -1) {
         if (*crashed > 1) {robots[*crashed - 2] = 0;
       				}
         		robots[(int)(value-2)] = 0;
       }
         } else {
						grid[(y - 1)*grid_rows + x - 1] = value;
                }

		}
	}
  else{


  		incrNE = (dx - dy) << 1;   		/* incrNE = (dy - dx) * 2;*/
	  	incrE = dx << 1;       	 			/* incrE = dy * 2; */
	  	d = incrE - dy;       				/* d = dy * 2 - dx; */
	  for(cnt = 1; (cnt <= dy) && (x < grid_rows) && (y < grid_columns)
     													&& (x > 1) && (y > 1); cnt++){
  		if( d <= 0 ) { 						/* Choose E pixel*/
    						d += incrE;
      }
  	  else {         							/* Choose NE pixel*/
				d += incrNE;
				x += incrx;
     }
     y += incry;
      if (grid[(y - 1)*grid_rows + x - 1] > 0) {
      	*crashed = grid[(y - 1)*grid_rows + x - 1];
       	if (robots[0] > -1) {
         	if (*crashed > 1) {robots[*crashed - 2] = 0;
         	}
         robots[(int)(value - 2)] = 0;
       	}
      } else {grid[(y - 1)*grid_rows + x - 1] = value;
        }

      }
  }

}


unsigned int round(double input)

{	unsigned int ans;
	double ipart;


	if (modf(input, &ipart) < 0.5) {
   ans = (int) ipart;
   } else {
   			ans = ((int) ipart) + 1;
          }

   return ans;
}




void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  double *polygon, *value, *robots;
  double *outM;
  int grid_columns, i_frame;
  int grid_rows;
  int i, crashed;
  unsigned char *grid;

	if (nrhs != 4) {
    mexErrMsgTxt("Four input arguments required.");
    }

  polygon = mxGetPr(prhs[0]);  	// Patch apexes coords
  i = mxGetN(prhs[0]);           // No. of apexes

  grid_rows = mxGetM(prhs[1]);
  grid_columns = mxGetN(prhs[1]);

  robots = mxGetPr(prhs[2]);

  value = mxGetPr(prhs[3]);




  /*  Now get the matrix ("world") from MATLAB (type UINT8 - unsigned char */
   grid = (unsigned char *) mxGetData(prhs[1]);	// Get the input matrix
  /* We've got it */

  crashed = 0;


  /* Fill in the matrix */
  for (i_frame = 1; i_frame < i; i_frame++) {
     ray(grid, grid_rows, grid_columns, round(polygon[2*(i_frame - 1)]),
     round(polygon[2*(i_frame - 1) + 1]), round(polygon[2*i_frame]),
     round(polygon[2*i_frame + 1]),*value, &crashed, robots);

  }
  /* Now close the patch */
	  ray(grid, grid_rows, grid_columns, round(polygon[2*i - 2]),
     round(polygon[2*i - 1]), round(polygon[0]), round(polygon[1]),
     *value, &crashed, robots);

   plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
   outM = mxGetPr(plhs[0]);

   /* If there is no collision, return 0. When there is collision between
   	robots, stop them. The input array "robots" can be [-1] - this
      is when putting in a new robot - then any collision means error (the robot
      has been placed on an obtacle). */


   if (crashed > 0) {
   	if ((robots[0]) == -1)  {*outM = -1;		/* Error */
								      } else {*outM = crashed;}
   } else {*outM = 0;}

  return;
}

