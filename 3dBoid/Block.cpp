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

Block::Block(double _x, double _y, double _z, double _r)
{
    x = _x;
    y = _y;
    z = _z;
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
    int slice = 10;
    GLfloat color[] = {GLfloat(red),GLfloat(green),GLfloat(blue),1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    glPushMatrix();
    glTranslated(x, y, z);
    glutSolidSphere(r, slice, slice);
    glPopMatrix();
}

void Block::setDisabled()
{
    disabled = true;
}
