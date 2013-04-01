/*
 * arSensor.cpp
 *
 *  Created on: 20.01.2011
 *      Author: andre
 */

#include "arSensor.h"

arSensor::arSensor(char *cPattName, double dPattWidth) : arObject(cPattName, dPattWidth) {

    m_bVirtualSensorActive = false;
    // initializing real sensor
    m_senDiagram = new glDiagram(200, 0);
    m_senBeam    = new Sensor();

    // TODO Auto-generated constructor stub
    this->m_senDiagram->setColorFrame(1, 0, 1, 1);
    this->m_senDiagram->setColorValues(1, 1, 1, 1);
    this->m_senDiagram->setColorText(1, 1, 1, 1);
    this->m_senDiagram->setColorInfo(1, 0, 0, 1);
    this->m_senDiagram->setColorGrid(0.5, 0.5, 0.5, 1);
    this->m_senDiagram->setColorBackground(1, 1, 1, 1);
    this->m_senDiagram->m_dMinX = -2;
    this->m_senDiagram->m_dMaxX = 100;
    this->m_senDiagram->m_dMinY = 0;
    this->m_senDiagram->m_dMaxY = 32;
    this->m_senDiagram->m_fHeight = 1;
    this->m_senDiagram->m_fWidth = 2;
    this->m_senDiagram->setMain("IR-Sensor");
    this->m_senDiagram->setXlab("time");
    this->m_senDiagram->setYlab("distance");

    this->m_senBeam->setPosition(0, 90, -58);
    this->m_senBeam->setOrientation(0, 0, 45);
    this->m_senBeam->setColor(0, 0.7, 0, 0.5);
    this->m_senBeam->setSensorBeamParameter(100, 1, 1, 5, 10);
    this->m_senBeam->setValue(0);
    this->m_senBeam->setValidity(100);

    // initializin virtual sensor
    m_virDiagram = new glDiagram(200, 0);
    m_virBeam    = new Sensor();

    this->m_virDiagram->setColorFrame(1, 0, 1, 1);
    this->m_virDiagram->setColorValues(1, 1, 1, 1);
    this->m_virDiagram->setColorText(1, 1, 1, 1);
    this->m_virDiagram->setColorInfo(1, 0, 0, 1);
    this->m_virDiagram->setColorGrid(0.5, 0.5, 0.5, 1);
    this->m_virDiagram->setColorBackground(1, 1, 1, 1);
    this->m_virDiagram->m_dMinX = -2;
    this->m_virDiagram->m_dMaxX = 100;
    this->m_virDiagram->m_dMinY = 0;
    this->m_virDiagram->m_dMaxY = 100;
    this->m_virDiagram->m_fHeight = 1;
    this->m_virDiagram->m_fWidth = 2;
    this->m_virDiagram->setMain("Virtual-Sensor");
    this->m_virDiagram->setXlab("time");
    this->m_virDiagram->setYlab("distance");

    this->m_virBeam->setPosition( 0, 0, 0);
    this->m_virBeam->setOrientation(0, 0, 45);
    this->m_virBeam->setColor(1, 0, 0, 0.5);
    this->m_virBeam->setSensorBeamParameter(100, 1, 1, 5, 10);
    this->m_virBeam->setValue(0);

}

arSensor::~arSensor() {
    // TODO Auto-generated destructor stub
}

void arSensor::draw() {
    double gl_para[16];
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );

    // load the camera transformation matrix
    argConvGlpara(this->m_dPattTransformation, gl_para);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( gl_para );

    glRotatef(90, 1, 0, 0);

    this->drawRealSensor();
    this->drawVirtualSensor();
}

void arSensor::drawRealSensor() {

    char info[100];

    glPushMatrix();
    glTranslatef(this->m_senBeam->m_dPosition[0], this->m_senBeam->m_dPosition[1], this->m_senBeam->m_dPosition[2]);
    glTranslatef(0, 50, -20);
    glRotatef(0, 0, 1, 0);
    glScalef(20, 20, 20);
    this->m_senDiagram->drawGrid(8, 4);
    this->m_senDiagram->drawContinuous(GL_LINE_STRIP);
    sprintf(info, "%.0f cm", this->m_senBeam->m_dValue / 10);
    this->m_senDiagram->printInfo(info, 0.6);
    glPopMatrix();
    glPushMatrix();
    this->m_senBeam->draw();
    glPopMatrix();
}

void arSensor::addValue(double val) {
    this->m_senDiagram->add(val / 10);
    this->m_senBeam->setValue(val);
}

void arSensor::updateVirtualSensor(double distance, double posX, double posY, double posZ, double rot) {
    this->m_virBeam->setPosition( posX, posY, posZ);
    //this->m_virBeam->setOrientation(-90,180,45);
    this->m_virBeam->setValue(distance);
    this->m_virBeam->setOrientation(0, rot, 45);
    this->m_virDiagram->add(distance / 10);
}

void arSensor::drawVirtualSensor() {

    // if not activated -> do not draw
    if (!this->m_bVirtualSensorActive) {
        return;
    }

    char info[100];

    glTranslatef(-125, 80, -70);
    // draw sensor beam
    glPushMatrix();
    this->m_virBeam->draw();
    glPopMatrix();
    // draw Sphere
    glPushMatrix();
    glTranslatef(this->m_virBeam->m_dPosition[0], this->m_virBeam->m_dPosition[1], this->m_virBeam->m_dPosition[2]);
    glColor4f(0, 0, 1, 0.5);
    glutSolidSphere(10, 10, 10);
    glPopMatrix();
    // draw Diagram
    glPushMatrix();
    glTranslatef(this->m_virBeam->m_dPosition[0], this->m_virBeam->m_dPosition[1], this->m_virBeam->m_dPosition[2]);
    glTranslatef(0, 50, -20);
    glRotatef(0, 0, 1, 0);
    glScalef(20, 20, 20);
    this->m_virDiagram->drawGrid(8, 3);
    this->m_virDiagram->drawContinuous(GL_LINE_STRIP);
    sprintf(info, "%.0f mm", this->m_virBeam->m_dValue);
    this->m_virDiagram->printInfo(info, 0.6);
    //this->m_virDiagram->drawBackground();
    //this->m_virDiagram->drawGrid(8,4);
    //this->m_virDiagram->drawContinuous(GL_LINE_STRIP);
    glPopMatrix();
}
