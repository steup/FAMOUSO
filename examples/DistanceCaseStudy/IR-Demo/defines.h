/********************************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string.h>
#include <vector>

#include "text3d.h"
/********************************************************************************************************/
#include "debug.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include "famouso_bindings.h"

std::string strIRChannel = "distance";
famouso::config::SEC *secIR;
/********************************************************************************************************/
#include <GL/gl.h>
#include <GL/glut.h>
/********************************************************************************************************/
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

int     xsize, ysize;
int     thresh = 100;
int     frame_count = 0;

char    cparam_name[] = "./dat/camera_para.dat";
ARParam cparam;

char pattName[]  = "./dat/Famouso.patt";
double pattWidth  = 80;

bool fastMode  = false;
bool fixMode   = false;
bool videoOnly = false;

/********************************************************************************************************/
#include "arSensor.h"

arSensor *ir_Sensor;
