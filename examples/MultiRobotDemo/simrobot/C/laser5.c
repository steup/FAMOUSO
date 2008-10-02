#include <math.h>
#include "mex.h"
#include "matrix.h"
#define pi 3.14159265348979323

/* This MEX file gets the matrix, position, range, number of rays and scanning
angle of the ultrasonic sensor and returns to MATLAB distance to the nearest
obstacle within sensor's 'field of view'. If there's no visible obstacle,
999999 is returned. */



int ray(unsigned char grid[], int grid_rows, int grid_columns, int *x,
		  int *y, int x0, int y0, int x1, int y1)
{
	int dx, dy, incrE, incrNE, d, incrx, incry, cnt;
  *x = x0;
  *y = y0;
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

  if (grid[(*y - 1)*grid_rows + *x - 1] > 0)
  	return (1);

	if (dy < dx){
		incrNE = (dy - dx) << 1;   	/* incrNE = (dy - dx) * 2;*/
	  incrE = dy << 1;       	 		/* incrE = dy * 2; */
	  d = incrE - dx;       			/* d = dy * 2 - dx; */
	  for(cnt = 1; (cnt <= dx) && (*x < grid_rows - 1) && (*y < grid_columns) &&
     					(*x > 2) && (*y >= 2); cnt++){
  		if( d <= 0 ) { /* Choose E pixel*/
    		d += incrE;
			}
  	  else {         /* Choose NE pixel*/
				d += incrNE;
				*y += incry;
			}
			*x += incrx;
		  if (grid[(*y - 1)*grid_rows + *x - 1] > 0){	/* Actual (new) position */
		  	return (1);
      }
		  if (grid[(*y - 1)*grid_rows + *x - 1 + incrx] > 0){
		    *x += incrx;                          /* Left/Right-hand side pixel */
		  	return (1);
			}
		}
	}
  else{
		incrNE = (dx - dy) << 1;   	/* incrNE = (dy - dx) * 2;*/
	  incrE = dx << 1;       	 		/* incrE = dy * 2; */
	  d = incrE - dy;       			/* d = dy * 2 - dx; */
	  for(cnt = 1; (cnt <= dy) && (*x < grid_rows - 1) &&
     					(*y < grid_columns) && (*x > 2) && (*y >= 2); cnt++){
  		if( d <= 0 ) { /* Choose E pixel*/
    		d += incrE;
			}
  	  else {         /* Choose NE pixel*/
				d += incrNE;
				*x += incrx;
			}
			*y += incry;
     if (grid[(*y - 1)*grid_rows + *x - 1] > 0){   /* Actual (resp. new) position */
		  	return (1);
      }
		  if (grid[(*y - 1)*grid_rows + *x - 1 + incrx] > 0){
		    *x += incrx;                         /* Left/Right-hand side pixel */
		  	return (1);
			}
      }
  }
  return (0);
}


void scan(unsigned char grid[], int grid_rows, int grid_columns,
					int frame[], int frame_length,
					int x0, int y0,int visible_points[], int *n_points,
               double read_data[], double obstacles[])
{
//	int x1, y1, xhit, yhit, previous_was_hit, prev_xhit, prev_yhit, i;
   int i, xhit, yhit;
   //int *points;
   //double distance;

 //	*n_points = 0;
 // 	prev_xhit = 0;
 /* 	prev_yhit = 0;
  	previous_was_hit = 0;*/

  for (i = 0; i < frame_length/2; i++){
	  if (ray(grid, grid_rows, grid_columns, &xhit, &yhit, x0, y0, frame[2*i], frame[2*i+1])){
     		read_data[i] = sqrt(pow((xhit - x0),2) + pow((yhit - y0),2));
         obstacles[i] = grid[(yhit - 1) * grid_rows + (xhit - 1)];
     } else { 	read_data[i] = 999999;
     				obstacles[i] = 0;
     			}
  }
}


unsigned int round(double input)

{	unsigned int ans;
	double ipart;


	if (modf(input, &ipart) < 0.5) {
   ans = (int) floor(input);
   } else {
   			ans = (int) ceil(input);
          }

   return ans;
}





void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  double *p, *outM, *outM2, *params, *read_data, *obstacles;
  int grid_columns, i_frame;
  int grid_rows, x0, y0;
  int n_points;
  double i;
  int *frame, *visible_points;
  unsigned char *grid;

	if (nrhs != 3) {
    mexErrMsgTxt("CMLA requires three input arguments.");
    }

  p = mxGetPr(prhs[0]);

  x0 = *p++;
  y0 = *p;

  /*  Now get the matrix ("world") from MATLAB (type UINT8 - unsigned char */
   grid = (unsigned char *) mxGetData(prhs[1]);	// Get the input matrix
  /* We've got it */

   grid_rows = mxGetM(prhs[1]);
   grid_columns = mxGetN(prhs[1]);



  params = mxGetPr(prhs[2]);	/* params - begin_angle, end_angle, no._of_rays, range */

  /* Let's allocate some memory for temporary arrays */
  	frame = mxCalloc(2*params[2], sizeof(int));
  	visible_points = mxCalloc(2*params[2], sizeof(int));
   read_data = mxCalloc(params[2], sizeof(double));
	obstacles = mxCalloc(params[2], sizeof(double));

  i_frame = 0;

  /* Compute end points of rays */

  for (i = params[0]; (i < params[1]) && (i_frame < 2*params[2]);
  		 i += ((params[1] - params[0]) / (params[2]))) {

  		frame[i_frame++] = round(x0 + params[3]*cos(i*pi/180));
   	frame[i_frame++] = round(y0 + params[3]*sin(i*pi/180));
   }


  scan(grid, grid_rows, grid_columns, frame, i_frame, x0, y0, visible_points,
  		 &n_points, read_data, obstacles); // Go !!

   mxFree(frame);
  	mxFree(visible_points);

  /* Pass back some useful output data */
  plhs[0] = mxCreateDoubleMatrix(1, params[2], mxREAL);	// Create unpopulated matrix
  plhs[1] = mxCreateDoubleMatrix(1, params[2], mxREAL);	// Create unpopulated matrix2
  outM = mxGetPr(plhs[0]);								// Pointer to data in output matrix
  outM2 = mxGetPr(plhs[1]);								// Pointer to data in output matrix2 (obstacles)
  for (i = 0; i < params[2]; i++) {
	  outM[(int) i] = read_data[(int) i];
     outM2[(int) i] = obstacles[(int) i];
	}
   mxFree(read_data);
	mxFree(obstacles);
  return;
}

