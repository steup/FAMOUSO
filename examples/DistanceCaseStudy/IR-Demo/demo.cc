#include "defines.h"

double pos[3] = {0, 0, 0};
double rot[3] = {0, 0, 0};

void setOrthographicProjection() {
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);
    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();
    // reset matrix
    glLoadIdentity();
    // set a 2D orthographic projection
    gluOrtho2D(0, xsize, 0, ysize);
    // invert the y axis, down is positive
    glScalef(1, -1, 1);
    // mover the origin from the bottom left corner
    // to the upper left corner
    glTranslatef(0, -ysize, 0);
    glMatrixMode(GL_MODELVIEW);
}

/* cleanup function called when program exits */
static void cleanup(void) {
    arVideoCapStop();
    arVideoClose();
    argCleanup();

    delete secIR;
    delete ir_Sensor;
    strIRChannel.clear();
}

void resetPerspectiveProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}



static void mainLoop(void) {
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             i;
    int             bestFit=-1;

    /* grab a vide frame */
    if ( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    if ( frame_count == 0 ) arUtilTimerReset();
    frame_count++;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glDisable(GL_BLEND);
    argDrawMode2D();
    argDispImage( dataPtr, 0, 0 );
    glEnable(GL_BLEND);

    /* detect the markers in the video frame */
    if ( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
        cleanup();
        exit(0);
    }

    arVideoCapNext();

    if ( arDebug ) {
        argDispImage( dataPtr, 0, 0 );
        if ( arImageProcMode == AR_IMAGE_PROC_IN_HALF )
            argDispHalfImage( arImage, 0, 0 );
        else
            argDispImage( arImage, 0, 0);

        glColor3f( 1.0, 0.0, 0.0 );
        glLineWidth( 2.0 );
        for ( i = 0; i < marker_num; i++ ) {
            argDrawSquare( marker_info[i].vertex, 0, 0 );
        }
        glLineWidth( 1.0 );
    } else {
        argDraw3dCamera( 0, 0 );
    }

    if (!fixMode) {
        for ( i = 0; i < marker_num; i++ ) {
            if ( marker_info[i].id == ir_Sensor->m_iPattID) {
                if ( bestFit == -1 ) {
                    bestFit = i;
                } else if ( marker_info[bestFit].cf < marker_info[i].cf ) {
                    bestFit = i;
                }
            }
        }
    }
    if (bestFit != -1) {
        if (arDebug) {
            glColor3f( 0.0, 1.0, 0.0 );
            glLineWidth( 2.0 );
            argDrawSquare( marker_info[bestFit].vertex, 0, 0 );
            glLineWidth( 1.0 );
        }

        if (!fixMode) {
            ir_Sensor->drawX(&marker_info[bestFit], fastMode);
        }
    }

    if (!videoOnly)
        ir_Sensor->draw();

    argSwapBuffers();
}

static void keyEvent( unsigned char key, int x, int y) {
    //quit if the ESC key is pressed
    switch ( key ) {
        case 0x1b: {
                printf("*** %f (frame/sec)\n", (double)frame_count / arUtilTimer());
                cleanup();
                exit(0);
            }

        case 'y': {
                arDebug = !arDebug;
                if ( arDebug == 0 ) {
                    glClearColor( 0.0, 0.0, 0.0, 0.0 );
                    glClear(GL_COLOR_BUFFER_BIT);
                    argSwapBuffers();
                    glClear(GL_COLOR_BUFFER_BIT);
                    argSwapBuffers();
                }
                break;
            }
        case 'x' : thresh--; break;
        case 'c' : thresh++; break;

        case '#' : fastMode = !fastMode; break;

        case 'f' : fixMode = !fixMode; break;

        case 'v' : videoOnly=!videoOnly;break;
    }
}






void callbackIR(famouso::mw::api::SECCallBackData& cbd) {

    int iID = (int)cbd[0];

    if (iID == 1) {
        int iCommand = (int)cbd[1];
        short *sDistance = (short*) & cbd[2];
        short *sPosX = (short*) & cbd[4];
        short *sPosY = (short*) & cbd[6];
        short *sPosZ = (short*) & cbd[8];
        short *sRotY = (short*) & cbd[10];

        // activate virtual sensor
        if (iCommand == 1) {
            ir_Sensor->m_bVirtualSensorActive = true;
        }
        // deactivate virtual sensor
        else if (iCommand == 2) {
            ir_Sensor->m_bVirtualSensorActive = false;
        }

        ir_Sensor->updateVirtualSensor( (double) *sDistance * 10,
                                        (double) *sPosX,
                                        (double) *sPosY,
                                        (double) *sPosZ,
                                        (double) *sRotY);
    }

    if (iID == 0) {
        int val = cbd[1];
        ir_Sensor->addValue((double)10*val);
    }
}

static void init(const char *config ) {
    ARParam  wparam;

    // open the video path
    if ( arVideoOpen( const_cast<char*>(config) ) < 0 ) exit(0);
    // find the size of the window
    if ( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    // set the initial camera parameters
    if ( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

    // open the graphics window
    argInit( &cparam, 1.0, 0, 0, 0, 0 );

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//GL_ONE_MINUS_CONSTANT_COLOR);
    glAlphaFunc(GL_GREATER, 0.9f);
    glEnable(GL_BLEND);

    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE) ;
    glEnable ( GL_COLOR_MATERIAL ) ;

    // initializing Famouso
    famouso::init<famouso::config>();

    secIR = new famouso::config::SEC(strIRChannel.c_str());
    secIR->subscribe();
    secIR->callback.bind<callbackIR>();



    ir_Sensor = new arSensor(pattName, pattWidth);

    t3dInit();
}

#include "util/CommandLineParameterGenerator.h"
// Commandline options
CLP3( VideoOptions,
      "Video options",
      "video,v",
      "video device, width and height (e.g. /dev/video0:640:480 (default)",
      std::string, videodev, "/dev/video0",
      std::string, width,  "640",
      std::string, height, "480"
);

int main(int argc, char **argv) {
    // get command line options
    famouso::util::clp(argc,argv);
    CLP::config::VideoOptions::Parameter param;
    CLP::config::VideoOptions::instance().getParameter(param);

    string str("v4l2src device=");
    str.append(param.videodev);
    str.append(" ! video/x-raw-yuv,width=");
    str.append(param.width);
    str.append(",height=");
    str.append(param.height);
    str.append(" ! ffmpegcolorspace ! video/x-raw-rgb ! identity name=artoolkit ! fakesink");

    std::cout<<str<<std::endl;
    // Initialize graphics
    glutInit(&argc, argv);

    // Initialize Augmented Reality stuff
    init(str.c_str());

    // start video capturing
    arVideoCapStart();

    // and visualize thing according to markers
    argMainLoop( NULL, keyEvent, mainLoop );
    return (0);
}
