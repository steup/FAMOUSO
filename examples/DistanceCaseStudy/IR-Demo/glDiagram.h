/*
 * glDiagram.h
 *
 *  Created on: 29.01.2010
 *      Author: andre
 */

#ifndef GLDIAGRAM_H_
#define GLDIAGRAM_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define SIZE_XLAB 0.002f
#define SIZE_YLAB 0.002f
#define SIZE_MAIN 0.002f

class glDiagram {
    public:
        glDiagram(unsigned int uiBufferLength, double dInitialValue);

        void setColorFrame(float r, float g, float b, float alpha);
        void setColorText(float r, float g, float b, float alpha);
        void setColorValues(float r, float g, float b, float alpha);
        void setColorGrid(float r, float g, float b, float alpha);
        void setColorBackground(float r, float g, float b, float alpha);
        void setColorInfo(float r, float g, float b, float alpha);

        void setXlab(const char *cXlab);
        void setYlab(const char *cYlab);
        void setMain(const char *cMain);

        void drawText(void);

        void draw(GLenum mode);
        void drawGrid(unsigned int cols, unsigned int rows);
        void drawContinuous(GLenum mode);
        void add(double val);
        void drawBackground(void);

        void printInfo(const char *info, float size = 0.3);

//    private:
        float m_fColorFrame [4];
        float m_fColorText[4];
        float m_fColorValues[4];
        float m_fColorGrid[4];
        float m_fColorBackground[4];
        float m_fColorInfo[4];

        float m_fWidth;
        float m_fHeight;

        double m_dMinX;
        double m_dMinY;
        double m_dMaxX;
        double m_dMaxY;

        const char *m_cXlab;
        const char *m_cYlab;
        const char *m_cMain;

        unsigned int m_uiBufferLength;
        unsigned int m_uiBufferPosition;
        double *m_dValueBuffer;
};

#endif /* GLDIAGRAM_H_ */
