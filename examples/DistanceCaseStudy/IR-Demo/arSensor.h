/*
 * arSensor.h
 *
 *  Created on: 20.01.2011
 *      Author: andre
 */

#ifndef ARSENSOR_H_
#define ARSENSOR_H_

#include "arObject.h"

#include "Sensor.h"
#include "glDiagram.h"

class arSensor : public arObject {
    public:
        arSensor(char *cPattName, double dPattWidth);
        virtual ~arSensor();

        void addValue(double val);
        void draw();
        void drawRealSensor();

        void updateVirtualSensor(double distance, double posX, double posY, double posZ, double rot);
        void drawVirtualSensor();

        glDiagram *m_senDiagram;
        Sensor  *m_senBeam;

        bool  m_bVirtualSensorActive;

        glDiagram *m_virDiagram;
        Sensor  *m_virBeam;
};

#endif /* ARSENSOR_H_ */
