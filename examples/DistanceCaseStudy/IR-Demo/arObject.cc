/*
 * arObject.cpp
 *
 *  Created on: Jun 3, 2010
 *      Author: eos
 */

#include "arObject.h"

arObject::arObject() {
    this->setPosition(0, 0, 0);
    this->setColor(0, 0, 0, 1);
    this->setOrientation(0, 0, 0);

    this->m_dPattWidth     = 80.0;
    this->m_dPattCenter[0] =  0.0;
    this->m_dPattCenter[1] =  0.0;
}

arObject::arObject(char *cPattName) {
    this->setPosition(0, 0, 0);
    this->setColor(0, 0, 0, 1);
    this->setOrientation(0, 0, 0);

    this->m_dPattWidth     = 80.0;
    this->m_dPattCenter[0] =  0.0;
    this->m_dPattCenter[1] =  0.0;

    init(cPattName);
}

arObject::arObject(char *cPattName, double dPattWidth) {
    this->setPosition(0, 0, 0);
    this->setColor(0, 0, 0, 1);
    this->setOrientation(0, 0, 0);

    this->m_dPattWidth     = dPattWidth;
    this->m_dPattCenter[0] = 0.0;
    this->m_dPattCenter[1] = 0.0;

    init(cPattName);
}

arObject::~arObject() {
    arFreePatt(this->m_iPattID);
}

void arObject::init(char *cPattName) {
    this->m_cPattName = cPattName;

    if ( (this->m_iPattID = arLoadPatt(this->m_cPattName)) < 0 ) {
        printf("pattern load error !!\n");
        exit(0);
    }

    //setActive(true);
}

void arObject::setPosition(double x, double y, double z) {
    m_dPosition[0] = x;
    m_dPosition[1] = y;
    m_dPosition[2] = z;
}

void arObject::setOrientation(double alpha, double beta, double gamma) {
    m_dOrientation[0] = alpha;
    m_dOrientation[1] = beta;
    m_dOrientation[2] = gamma;
}

void arObject::setColor(double r, double g, double b, double alpha) {
    m_dColor[0] = r;
    m_dColor[1] = g;
    m_dColor[2] = b;
    m_dColor[3] = alpha;
}

void arObject::drawX(ARMarkerInfo *marker_info, bool fastMode) {
    if (fastMode) {
        arGetTransMat(  marker_info,
                        this->m_dPattCenter,
                        this->m_dPattWidth,
                        this->m_dPattTransformation);
    } else {
        arGetTransMatCont( marker_info,
                           this->m_dPattTransformation,
                           this->m_dPattCenter,
                           this->m_dPattWidth,
                           this->m_dPattTransformation);
    }
    //this->draw();
}

void arObject::draw() {

}

bool arObject::getActive() {
    return this->m_bActive;
}

void arObject::setActive(bool value) {
    this->m_bActive = value;
    /*
     if(this->m_bActive)
      arActivatePatt( this->m_iPattID );
     else
      arDeactivatePatt(this->m_iPattID);
      */
}
