/*
 * Sensor.h
 *
 *  Created on: Jun 1, 2010
 *      Author: eos
 */

#ifndef SENSOR_H_
#define SENSOR_H_

//#include "arObject.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

class Sensor { //: public arObject{
    public:
        Sensor();
        virtual ~Sensor();

        void setSensorBeamParameter(double dMaxBeamLength,
                                    double dScaleHorizontal,
                                    double dScaleVertical,
                                    double dRadiusBegin,
                                    double dRadiusEnd);

        void setValue(double dValue);
        void setValidity(double dValidity);
        void setValidityColor();

        void setPosition(double x, double y, double z);
        void setOrientation(double alpha, double beta, double gamma);
        void setColor(double r, double g, double b, double alpha);

        double m_dPosition[3];
        double m_dOrientation[3];
        double m_dColor[4];

        double m_dMaxBeamLength;
        double m_dScaleHorizontal;
        double m_dScaleVertical;
        double m_dRadiusBegin;
        double m_dRadiusEnd;

        double m_dValue;
        double m_dValidity;

        bool m_bKeepSize;

        GLUquadricObj *quadratic;
        GLUquadricObj *quadratic2;

        void draw();
        //void drawDiagram();

        void initDiagram();
};

#endif /* SENSOR_H_ */
