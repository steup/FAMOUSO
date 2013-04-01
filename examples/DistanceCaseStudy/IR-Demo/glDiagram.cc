/*******************************************************************************
 *
 * Copyright (c) 2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2010 Andre Dietrich  <adietric@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: $
 *
 ******************************************************************************/
#include "glDiagram.h"

#include "text3d.h"

#include <iostream>
#include <math.h>

#include <stdio.h>

double abs(double val) {
    return ( val < 0 ) ? -val : val;
}

glDiagram::glDiagram(unsigned int uiBufferLength, double dInitialValue) {
    m_uiBufferLength = uiBufferLength;
    m_uiBufferPosition = 0;

    m_dValueBuffer  = new double [m_uiBufferLength];

    for (unsigned int i = 0; i < m_uiBufferLength; i++)
        m_dValueBuffer[i] = dInitialValue;

    this->m_cMain = NULL;
    this->m_cXlab = NULL;
    this->m_cYlab = NULL;
}

void glDiagram::setColorFrame(float r, float g, float b, float alpha) {
    m_fColorFrame[0] = r;
    m_fColorFrame[1] = g;
    m_fColorFrame[2] = b;
    m_fColorFrame[3] = alpha;
}
void glDiagram::setColorText(float r, float g, float b, float alpha) {
    m_fColorText[0] = r;
    m_fColorText[1] = g;
    m_fColorText[2] = b;
    m_fColorText[3] = alpha;
}
void glDiagram::setColorValues(float r, float g, float b, float alpha) {
    m_fColorValues[0] = r;
    m_fColorValues[1] = g;
    m_fColorValues[2] = b;
    m_fColorValues[3] = alpha;
}
void glDiagram::setColorGrid(float r, float g, float b, float alpha) {
    m_fColorGrid[0] = r;
    m_fColorGrid[1] = g;
    m_fColorGrid[2] = b;
    m_fColorGrid[3] = alpha;
}
void glDiagram::setColorBackground(float r, float g, float b, float alpha) {
    m_fColorBackground[0] = r;
    m_fColorBackground[1] = g;
    m_fColorBackground[2] = b;
    m_fColorBackground[3] = alpha;
}

void glDiagram::setColorInfo(float r, float g, float b, float alpha) {
    m_fColorInfo[0] = r;
    m_fColorInfo[1] = g;
    m_fColorInfo[2] = b;
    m_fColorInfo[3] = alpha;
}

void glDiagram::setXlab(const char *cXlab) {
    m_cXlab = cXlab;
}
void glDiagram::setYlab(const char *cYlab) {
    m_cYlab = cYlab;
}

void glDiagram::setMain(const char *cMain) {
    m_cMain = cMain;
}

void glDiagram::add(double val) {
    m_dValueBuffer[m_uiBufferPosition++] = val;
    m_uiBufferPosition%=m_uiBufferLength;
}

void glDiagram::drawContinuous(GLenum mode) {
    float y = 0;

    glPushMatrix();      //Save the transformations performed thus far

    glColor4fv( m_fColorFrame );
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-m_fWidth, m_fHeight);
    glVertex2f( m_fWidth, m_fHeight);
    glVertex2f( m_fWidth, -m_fHeight);
    glVertex2f(-m_fWidth, -m_fHeight);
    glEnd();

    drawText();

    glTranslatef(-m_fWidth, -m_fHeight, 0);

    glScalef( abs(2*m_fWidth  / m_uiBufferLength ),
              abs(2*m_fHeight / (m_dMaxY - m_dMinY))
              , 0);

    glColor4fv( m_fColorValues );
    glBegin(mode);
    for (unsigned int i = m_uiBufferPosition; i < m_uiBufferLength; i++) {
        glVertex2f(y , m_dValueBuffer[i]);
        y++;
    }

    for (unsigned int i = 0; i < m_uiBufferPosition; i++) {
        glVertex2f(y , m_dValueBuffer[i]);
        y++;
    }
    glEnd();

    glPopMatrix();
}

void glDiagram::draw(GLenum mode) {
    glPushMatrix();      //Save the transformations performed thus far

    glColor4fv( m_fColorFrame );
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-m_fWidth, m_fHeight);
    glVertex2f( m_fWidth, m_fHeight);
    glVertex2f( m_fWidth, -m_fHeight);
    glVertex2f(-m_fWidth, -m_fHeight);
    glEnd();

    drawText();

    glTranslatef(-m_fWidth, -m_fHeight, 0);

    glScalef( abs(2*m_fWidth  / m_uiBufferLength ),
              abs(2*m_fHeight / (m_dMaxY - m_dMinY))
              , 0);

    double *dValue = &m_dValueBuffer[m_uiBufferPosition];
    glColor4fv( m_fColorValues );
    glBegin(mode);
    for (float i = 0; i < m_uiBufferLength; i++) {
        glVertex2f(i , *dValue);
        dValue ++;
    }
    glEnd();

    glPopMatrix();

}


void glDiagram::drawText() {
    glColor4fv(this->m_fColorText);
    if (this->m_cMain != NULL) {
        glPushMatrix();
        glTranslatef(0, m_fHeight + 0.4, 0.0f);
        glScalef(0.4f, 0.4f, 0.4f);
        t3dDraw2D(this->m_cMain, 0, 0, 1);
        glPopMatrix();
    }

    if (this->m_cXlab != NULL) {
        glPushMatrix();
        glTranslatef(0, -m_fHeight - 0.3, 0.0f);
        glScalef(0.3f, 0.3f, 0.3f);
        t3dDraw2D(this->m_cXlab, 0, 0, 1);
        glPopMatrix();
    }

    if (this->m_cYlab != NULL) {
        glPushMatrix();
        glTranslatef(-m_fWidth - 0.3, 0, 0.0f);
        glRotatef(90, 0, 0, 1);
        glScalef(0.3f, 0.3f, 0.3f);
        t3dDraw2D(this->m_cYlab, 0, 0, 1);
        glPopMatrix();
    }


}

void glDiagram::drawGrid(unsigned int cols, unsigned int rows) {
    cols++;
    rows++;

    glPushMatrix();

    glColor4fv( m_fColorGrid );
    glLineWidth(1);

    glTranslatef(-m_fWidth, -m_fHeight, 0);

    glScalef(2*m_fWidth / cols, 1, 0);
    glBegin(GL_LINES);
    for (unsigned int i = 1; i < cols; i++) {
        glVertex2f( i, 0);
        glVertex2f( i, 2*this->m_fHeight);
    }
    glEnd();

    glScalef( 1 / (2*m_fWidth / cols), 2*m_fHeight / rows, 0);
    glBegin(GL_LINES);
    for (unsigned int i = 1; i < rows; i++) {
        glVertex2f( 0, i);
        glVertex2f( 2*this->m_fWidth, i);
    }
    glEnd();

    glPopMatrix();
}

void glDiagram::printInfo(const char *info, float size) {
    glPushMatrix();
    glColor4fv( m_fColorInfo );
    glScalef(size, size, 0);
    t3dDraw2D(info, 0, 0, 1);
    glPopMatrix();
}

void glDiagram::drawBackground() {
    glEnable(GL_BLEND);
    glColor4fv( m_fColorBackground );

    glBegin(GL_QUADS);
    glVertex2f(-m_fWidth, m_fHeight);
    glVertex2f( m_fWidth, m_fHeight);
    glVertex2f( m_fWidth, -m_fHeight);
    glVertex2f(-m_fWidth, -m_fHeight);
    glEnd();
    glDisable(GL_BLEND);
}
