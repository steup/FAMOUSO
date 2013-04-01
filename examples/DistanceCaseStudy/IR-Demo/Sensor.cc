/*
 * Sensor.cpp
 *
 *  Created on: Jun 1, 2010
 *      Author: eos
 */

#include "Sensor.h"
#include <stdio.h>

Sensor::Sensor() {
    // TODO Auto-generated constructor stub
    this->setPosition(0, 0, 0);
    this->setColor(0, 0, 0, 1);
    this->setOrientation(0, 0, 0);

    this->setSensorBeamParameter(100, 1, 1, 15, 40);
    this->m_bKeepSize = false;
    this->setValue(100);
    this->setValidity(0);

    quadratic = gluNewQuadric();
    quadratic2 = gluNewQuadric();
}

Sensor::~Sensor() {
    // TODO Auto-generated destructor stub
}

void Sensor::setPosition(double x, double y, double z) {
    m_dPosition[0] = x;
    m_dPosition[1] = y;
    m_dPosition[2] = z;
}

void Sensor::setOrientation(double alpha, double beta, double gamma) {
    m_dOrientation[0] = alpha;
    m_dOrientation[1] = beta;
    m_dOrientation[2] = gamma;
}

void Sensor::setColor(double r, double g, double b, double alpha) {
    m_dColor[0] = r;
    m_dColor[1] = g;
    m_dColor[2] = b;
    m_dColor[3] = alpha;
}

void Sensor::setSensorBeamParameter(double dMaxBeamLength,
                                    double dScaleHorizontal,
                                    double dScaleVertical,
                                    double dRadiusBegin,
                                    double dRadiusEnd) {
    this->m_dMaxBeamLength  = dMaxBeamLength;
    this->m_dScaleHorizontal = dScaleHorizontal;
    this->m_dScaleVertical  = dScaleVertical;
    this->m_dRadiusBegin  = dRadiusBegin;
    this->m_dRadiusEnd   = dRadiusEnd;


}

void Sensor::draw() {

    glLineWidth(1.5);
    glPushMatrix();
    glEnable(GL_BLEND);

    glTranslated(m_dPosition[0], m_dPosition[1], m_dPosition[2]);


    glScalef( this->m_dScaleHorizontal,
              1,
              this->m_dScaleVertical);

    glRotated(m_dOrientation[0], 1, 0, 0);
    glRotated(m_dOrientation[1], 0, 1, 0);
    glRotated(m_dOrientation[2], 0, 0, 1);

    glPushMatrix();

    setValidityColor();

    double len = 0;
    for ( len = this->m_dMaxBeamLength;
            len < this->m_dValue;
            len += this->m_dMaxBeamLength) {
        gluCylinder(quadratic,
                    this->m_dRadiusBegin,
                    this->m_dRadiusEnd,
                    this->m_dMaxBeamLength,
                    4, 1);
        glTranslated(0, 0, this->m_dMaxBeamLength);
    }

    gluCylinder(quadratic,
                this->m_dRadiusBegin,
                (this->m_bKeepSize) ?
                this->m_dRadiusEnd :
                this->m_dRadiusBegin + ((this->m_dRadiusEnd - this->m_dRadiusBegin) * ((this->m_dMaxBeamLength - (len - this->m_dValue)) / this->m_dMaxBeamLength)),
                this->m_dMaxBeamLength - (len - this->m_dValue),
                4, 1);
    glPopMatrix();


    //------------------------ draw Wire ----------------------------------

    // Turn on wireframe mode
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);

    glColor4d(0, 0, 0, 1);

    for ( len = this->m_dMaxBeamLength;
            len < this->m_dValue;
            len += this->m_dMaxBeamLength) {
        gluCylinder(quadratic,
                    this->m_dRadiusBegin,
                    this->m_dRadiusEnd,
                    this->m_dMaxBeamLength,
                    4, 1);
        glTranslated(0, 0, this->m_dMaxBeamLength);
    }

    gluCylinder(quadratic,
                this->m_dRadiusBegin,
                (this->m_bKeepSize) ?
                this->m_dRadiusEnd :
                this->m_dRadiusBegin + ((this->m_dRadiusEnd - this->m_dRadiusBegin) * ((this->m_dMaxBeamLength - (len - this->m_dValue)) / this->m_dMaxBeamLength)),
                this->m_dMaxBeamLength - (len - this->m_dValue),
                4, 1);

    // Turn off wireframe mode
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);

    glDisable(GL_BLEND);
    glLineWidth(1);
    glPopMatrix();
}

void Sensor::setValidityColor() {
    double dColorGood[3] = { -1, 1, 0};
    double dColorBad [3] = { 1, 0, 0};

    double dColorValidity[4] = {dColorBad[0] + this->m_dValidity * dColorGood[0],
                                dColorBad[1] + this->m_dValidity * dColorGood[1],
                                dColorBad[2] + this->m_dValidity * dColorGood[2],
                                m_dColor[3]
                               };

    if (this->m_dValidity != 0) {
        glColor4dv(dColorValidity);
    } else {
        glColor4d(1.0, 1.0, 1.0, 0.5);
    }
}

void Sensor::setValue(double dValue) {
    this->m_dValue = dValue;
}

void Sensor::setValidity(double dValidity) {
    this->m_dValidity = dValidity;
}
