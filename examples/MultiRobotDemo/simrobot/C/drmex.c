#include <math.h>
#include "mex.h"
#include "matrix.h"





void ray(unsigned char grid[], int grid_rows, int grid_columns,
			int x0, int y0, int x1, int y1, double robot_number)

/* Bresenham's algorithm - connects input points [x0,y0] & [x1,y1] by line
	segment */

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
	  for(cnt = 1; (cnt <= dx) && (x < grid_rows - 1) && (y < grid_columns - 1)
     															&& (x > 1) && (y > 1); cnt++){
  		if( d <= 0 ) { 						/* Choose E pixel*/
    		d += incrE;
			}
  	  else {         							/* Choose NE pixel*/
				d += incrNE;
				y += incry;
			}
			x += incrx;

         if (grid[(y - 1)*grid_rows + x - 1] == robot_number) {
          	 grid[(y - 1)*grid_rows + x - 1] = 0;
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

         if (grid[(y - 1)*grid_rows + x - 1] == robot_number) {
         	 grid[(y - 1)*grid_rows + x - 1] = 0;
             }


      }
  }

}

unsigned int round_F(double input)	  		/* Just rounds the input argument*/

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
  double *polygon, *robot_number, *robots;
  double *outM;
  int grid_columns, i_frame;
  int grid_rows;
  int i;
  unsigned char *grid;				/* This is the matrix (converted to a vector)*/

	if (nrhs != 4) {
    mexErrMsgTxt("Four input arguments required.");
    }

  polygon = mxGetPr(prhs[0]);
  i = mxGetN(prhs[0]);

  grid_rows = mxGetM(prhs[1]);
  grid_columns = mxGetN(prhs[1]);

  robots = mxGetPr(prhs[2]);				/* State vector */

  robot_number = mxGetPr(prhs[3]);     /* ID number of the robot - we need
  														to know what value can be deleted */




  /*  Now get the matrix ("world") from MATLAB (type UINT8 - unsigned char */
   grid = (unsigned char *) mxGetData(prhs[1]);	/* Get the input matrix */
  /* We've got it */




  /* Fill in the matrix */
  for (i_frame = 1; i_frame < i; i_frame++) {
     ray(grid, grid_rows, grid_columns, round_F(polygon[2*(i_frame - 1)]),
     round_F(polygon[2*(i_frame - 1) + 1]), round_F(polygon[2*i_frame]),
     round_F(polygon[2*i_frame + 1]), *robot_number);
  }
  /* Now close the patch */
  ray(grid, grid_rows, grid_columns, round_F(polygon[2*i - 2]),
  		round_F(polygon[2*i - 1]), round_F(polygon[0]), round_F(polygon[1]),
      *robot_number);


  return;
}

