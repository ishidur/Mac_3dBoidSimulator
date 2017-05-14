//
//  Block.cpp
//  boid
//
//  Created by Ryota Ishidu on 2017/05/04.
//  Copyright © 2017年 Ryota Ishidu. All rights reserved.
//

#include "Block.hpp"
#include <GLUT/GLUT.h>
#include <OpenGL/OpenGL.h>
#include <math.h>
#include <cmath>

Block::Block(double _x, double _y, double _r)
{
    x = _x;
    y = _y;
    r = _r;
}

void Block::setColor(double _red, double _green, double _blue)
{
    red = _red;
    green = _green;
    blue = _blue;
}
void Block::drawBlock()
{
    int n = 20;
    double angl = 2.0 * M_PI / n;
    glColor3d(red, green, blue);
    glPushMatrix();
    glTranslated(x, y, 0.0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; ++i)
    {
        glVertex2d(r * cos(double(i) * angl), r * sin(double(i) * angl));
    }
    glEnd();
    glPopMatrix();
}
void Block::setDisabled()
{
    disabled = true;
}

