#include <math.h>
#include "mex.h"
#include "matrix.h"
#define pi 3.14159265348979323

/* This MEX file gets the matrix, position, range, number of rays and scanning
angle of the ultrasonic sensor and returns to MATLAB distance to the nearest
obstacle within sensor's 'field of view'. If there's no visible obstacle,
999999 is returned. */
/* Recognizes if the obstacle is a robot and if so, returns also its number */


int ray(unsigned char grid[], int grid_rows, int grid_columns, int *x, int *y, int x0, int y0, int x1, int y1)
{
	int dx, dy, incrE, incrNE, d, incrx, incry, cnt, i;
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
	  d = incrE - dx;       		/* d = dy * 2 - dx; */
	  for(cnt = 1; (cnt <= dx) && (*x < grid_rows - 1) && (*y < grid_columns) && (*x > 2) && (*y >= 2); cnt++){
  		if( d <= 0 ) { /* Choose E pixel*/
    		d += incrE;
			}
  	  else {         /* Choose NE pixel*/
				d += incrNE;
				*y += incry;
			}
			*x += incrx;
		  if (grid[(*y - 1)*grid_rows + *x - 1] > 0){					/* Actual (new) position */
		  	return (1);
      }
		  if (grid[(*y - 1)*grid_rows + *x - 1 + incrx] > 0){		/* Tady +incrx -> -incrx */
		    *x += incrx;                                      /* Left-hand side pixel */
		  	return (1);
			}
		}
	}
  else{
		incrNE = (dx - dy) << 1;   	/* incrNE = (dy - dx) * 2;*/
	  incrE = dx << 1;       	 		/* incrE = dy * 2; */
	  d = incrE - dy;       		/* d = dy * 2 - dx; */
	  for(cnt = 1; (cnt <= dy) && (*x < grid_rows - 1) && (*y < grid_columns) && (*x > 2) && (*y >= 2); cnt++){
  		if( d <= 0 ) { /* Choose E pixel*/
    		d += incrE;
			}
  	  else {         /* Choose NE pixel*/
				d += incrNE;
				*x += incrx;
			}
			*y += incry;
		  if (grid[(*y - 1)*grid_rows + *x - 1] > 0){					/* Actual (resp. new) position */
        return (1);
      }
		  if (grid[(*y - 1)*grid_rows + *x - 1 + incrx] > 0){		/* Tady +incrx -> -incrx */
		    *x += incrx;                                      /* Left-hand side pixel */
        return (1);
			}
      }
  }
  return (0);
}


void scan(unsigned char grid[], int grid_rows, int grid_columns,
					int frame[], int frame_length,
					int x0, int y0,int visible_points[], int *n_points)
{
	int x1, y1, xhit, yhit, previous_was_hit, prev_xhit, prev_yhit, i;
   int *points;

	*n_points = 0;
  	prev_xhit = 0;
  	prev_yhit = 0;
  	previous_was_hit = 0;
  for (i = 0; i < frame_length; i += 2){
	  if (ray(grid, grid_rows, grid_columns, &xhit, &yhit, x0, y0, frame[i], frame[i+1])){
				visible_points[(*n_points)++] = xhit;
				visible_points[(*n_points)++] = yhit;
     if ((*n_points > 2) &&
     		(visible_points[*n_points-3] == visible_points[*n_points-1]) &&
			(visible_points[*n_points-4] == visible_points[*n_points-2])) {
      	 *n_points -= 2;					/* eliminating doubled points */
    }
  }

  }
	if ((visible_points[0] == visible_points[*n_points - 2]) &&
   (visible_points[1] == visible_points[*n_points - 1]) && (*n_points > 2))
		*n_points -= 2;				/* First and last point are identical */
}



unsigned int round_F(double input)	  	/* Just rounds the input argument (or how to round in C)*/

{	unsigned int ans;
	double ipart;


	if (modf(input, &ipart) < 0.5) {
   ans = (int) floor(input);
   } else {
   			ans = (int) ceil(input);
          }

   return ans;
}


double nearest(unsigned char grid[], int x0, int y0, int visible_points[],
					int n_points, int *obstacle, int grid_rows)
{
	double tmp, minim;
   int i;

   minim = 999999;
   for (i = 0; i < n_points/2; i++) {
   	tmp = sqrt(pow((visible_points[2*i] - x0),2) + pow((visible_points[2*i+1] - y0),2));
   	if (tmp < minim) {
      	minim = tmp;
         *obstacle = grid[visible_points[2*i] - 1 + (visible_points[2*i+1] - 1)*grid_rows];
      }
   }
   if (minim == 999999) {*obstacle = 0;}
   return minim;
}



void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  double *p, *outM, *params, *rob_no;
  int grid_columns, i_frame;
  int grid_rows, x0, y0;
  int n_points, obstacle;
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
   grid = (unsigned char *) mxGetData(prhs[1]);	 /* Get the input matrix */
  /* We've got it */

   grid_rows = mxGetM(prhs[1]);
   grid_columns = mxGetN(prhs[1]);



  params = mxGetPr(prhs[2]);			/* params - begin_angle, end_angle, no._of_rays, range */


  /* Let's allocate some memory for temporary arrays */
  	frame = mxCalloc(2*params[2], sizeof(int));
  	visible_points = mxCalloc(2*params[2], sizeof(int));

  i_frame = 0;

  /* Compute end points of rays */

   /* Compute end points of rays */

  for (i = params[0]; (i < params[1])  && (i_frame < 2*params[2]);
  		 i += ((params[1] - params[0]) / (params[2]))) {
  		frame[i_frame++] = round_F(x0 + params[3]*cos(i*pi/180));
   	frame[i_frame++] = round_F(y0 + params[3]*sin(i*pi/180));
   /* Eliminate doubled points */
   if ((i_frame > 2) && (frame[i_frame - 3] == frame[i_frame - 1]) &&
   	 (frame[i_frame - 4] == frame[i_frame - 2])) {
      	i_frame -= 2;
      }
   }




  scan(grid, grid_rows, grid_columns, frame, i_frame, x0, y0, visible_points, &n_points); 

  /* Pass back some useful output data */

  /* Number of robot, 1 for obstacle, 0 for no visible obstacle nor robot */
  
  /* Distance to nearest visible point */
  plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);	/* Create unpopulated matrix 1-by-1 */
  outM = mxGetPr(plhs[0]);								/*Pointer to data in output matrix*/

  plhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);	/* Create unpopulated matrix 1-by-1 */
  rob_no = mxGetPr(plhs[1]);							/* Pointer to data in output matrix*/

  *outM = nearest(grid, x0, y0, visible_points, n_points, &obstacle, grid_rows);
  

  if (obstacle > 0) {
  *rob_no = obstacle;
  } else {
  *rob_no = 0;
  }

  /* Free allocated memory */
  mxFree(frame);
  mxFree(visible_points);


  return;
}
