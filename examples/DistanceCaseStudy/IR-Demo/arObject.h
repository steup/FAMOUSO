/*
 * arObject.h
 *
 *  Created on: Jun 3, 2010
 *      Author: eos
 */

#ifndef AROBJECT_H_
#define AROBJECT_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

#include "glDiagram.h"

using namespace std;

class arObject {
    public:
        arObject();
        arObject(char *cPattName);
        arObject(char *cPattName, double dPattWidth);
        virtual ~arObject();

        void setPosition(double x, double y, double z);
        void setOrientation(double alpha, double beta, double gamma);
        void setColor(double r, double g, double b, double alpha);

        bool getActive();
        void setActive(bool value);

        void init(char *cPattName);

        void drawX(ARMarkerInfo *marker_info, bool fastMode = true);
        virtual void draw();

        double m_dPosition[3];
        double m_dOrientation[3];
        double m_dColor[4];

        char  *m_cPattName;
        int    m_iPattID;
        double m_dPattWidth;
        double m_dPattCenter[2];
        double m_dPattTransformation[3][4];

        bool m_bActive;
};

#endif /* AROBJECT_H_ */
