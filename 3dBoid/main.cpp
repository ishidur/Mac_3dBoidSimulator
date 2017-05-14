//
//  main.cpp
//  boid
//
//  Created by Ryota Ishidu on 2017/05/04.
//  Copyright © 2017年 Ryota Ishidu. All rights reserved.
//

#include <iostream>
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <tuple>
#include <algorithm>
#include "Grid.hpp"
#include "BaseBoid.hpp"
#include "Direction.hpp"
#include "Block.hpp"
#include "parameter.h" //import common parameters
#include "Eigen/Core"

int tim = 0; //time
bool isPress = false;
double mouseX = 0.0;
double mouseY = 0.0;

GLdouble vertex[][3] = {
    {-BOUNDARY + WALL_SIZE, -BOUNDARY + WALL_SIZE, -BOUNDARY + WALL_SIZE},
    {-BOUNDARY, -BOUNDARY + WALL_SIZE, -BOUNDARY + WALL_SIZE},
    {-BOUNDARY, -BOUNDARY, -BOUNDARY + WALL_SIZE},
    {-BOUNDARY + WALL_SIZE, -BOUNDARY, -BOUNDARY + WALL_SIZE},
    {-BOUNDARY + WALL_SIZE, -BOUNDARY + WALL_SIZE, -BOUNDARY},
    {-BOUNDARY, -BOUNDARY + WALL_SIZE, -BOUNDARY},
    {-BOUNDARY, -BOUNDARY, -BOUNDARY},
    {-BOUNDARY + WALL_SIZE, -BOUNDARY, -BOUNDARY}
};

int edge[][2] = {
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 0},
    {4, 5},
    {5, 6},
    {6, 7},
    {7, 4},
    {0, 4},
    {1, 5},
    {2, 6},
    {3, 7}
};

int face[][4] = {
    {0, 1, 2, 3},
    {1, 5, 6, 2},
    {5, 4, 7, 6},
    {4, 0, 3, 7},
    {4, 5, 1, 0},
    {3, 2, 6, 7}
};

GLdouble normal[][3] = {
    {0.0, 0.0,-1.0},
    {1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0},
    {-1.0, 0.0, 0.0},
    {0.0,-1.0, 0.0},
    {0.0, 1.0, 0.0}
};


GLdouble color[][3] = {
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0},
    {1.0, 1.0, 0.0},
    {1.0, 0.0, 1.0},
    {0.0, 1.0, 1.0}
};

GLfloat light0pos[] = {0.0, 3.0, 5.0, 1.0};
GLfloat light1pos[] = {5.0, 3.0, 0.0, 1.0};

GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
GLfloat red[] = {0.8, 0.2, 0.2, 1.0};
GLfloat blue[] = {0.2, 0.2, 0.8, 1.0};

double calcDist(double x1, double y1, double z1, double x2, double y2, double z2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
}

//[x][y][z]
Grid grids[GRID_NO + 2][GRID_NO + 2][GRID_NO + 2];

std::vector<Block> blocks;

//this function needs grids
std::vector<int> getAroundGridBoids(int id, int grid_x, int grid_y, int grid_z)
{
    //something went wrong
    std::vector<int> indexes = grids[grid_x][grid_y][grid_z].boidIndexes;
    //	for (int i = -1; i <= 1; ++i)
    //	{
    //		for (int j = -1; j <= 1; ++j)
    //		{
    //			for (int k = -1; k <= 1; ++k)
    //			{
    //				indexes.insert(indexes.end(), grids[grid_x + i][grid_y + j][grid_z + k].boidIndexes.begin(), grids[grid_x + i][grid_y + k][grid_z + j].boidIndexes.end());
    //			}
    //		}
    //	}
    auto result = remove(indexes.begin(), indexes.end(), id);
    auto result2 = unique(indexes.begin(), result);
    indexes.erase(result2, indexes.end());
    return indexes;
}

std::vector<BaseBoid> boids;


double degreeToRadian(double deg)
{
    return deg * M_PI / 180.0;
}

//this needs for Biod::isVisible
double _viewAngle = degreeToRadian(THETA_1) / 2.0;
BaseBoid updateSpeedAndAngle(BaseBoid& boid)
{
    Eigen::Vector3d q1 = Eigen::Vector3d::Zero();
    Eigen::Vector3d q2 = Eigen::Vector3d::Zero();
    Eigen::Vector3d q3 = Eigen::Vector3d::Zero();
    Eigen::Vector3d q4 = Eigen::Vector3d::Zero();
    int n1 = 0;
    int n2 = 0;
    int n3 = 0;
    int n4 = 0;
    auto indexes = getAroundGridBoids(boid.id, boid.grid_x, boid.grid_y, boid.grid_z);
    /*loop starts here*/
    for (auto i : indexes)
    {
        double dist = calcDist(boid.x, boid.y, boid.z, boids[i].x, boids[i].y, boids[i].z);
        
        //currently not working
        //		if (boid.isVisible(boids[i].x, boids[i].y, _viewAngle))
        //		{
        
        if (dist == 0.0)
        {
            std::cout << "boo;" << boid.x << ", " << boid.y << ", " << boid.z << ", " << boids[i].x << ", " << boids[i].y << ", " << boids[i].z << ", " << std::endl;
        }
        /*boidが見える範囲内にいる*/
        if (dist - 2.0 * BOID_SIZE < R_1)
        {
            /*rule1*/
            n1++;
            q1 += boids[i].vctr.normalized();
            if (boid.id == 0)
            {
                boids[i].setColor(0.6, 0.6, 1.0);
            }
        }
        if (dist - 2.0 * BOID_SIZE < R_2)
        {
            /*rule2*/
            n2++;
            q2 += Eigen::Vector3d(boids[i].x - boid.x, boids[i].y - boid.y, boids[i].z - boid.z) / dist / dist * R_2;
            if (boid.id == 0)
            {
                boids[i].setColor(0.4, 0.4, 1.0);
            }
        }
        if (dist - 2.0 * BOID_SIZE < R_3)
        {
            /*rule3*/
            n3++;
            q3 += Eigen::Vector3d(boids[i].x - boid.x, boids[i].y - boid.y, boids[i].z - boid.z) / dist / dist * R_3;
            if (boid.id == 0)
            {
                boids[i].setColor(0.2, 0.2, 1.0);
            }
        }
        
        //		}
    }
    /*loop ends here*/
    for (int n : grids[boid.grid_x][boid.grid_y][boid.grid_z].blockIndexes)
    {
        double dist = calcDist(boid.x, boid.y, boid.z, blocks[n].x, blocks[n].y, blocks[n].z);
        if (dist - BLOCK_SIZE - BOID_SIZE <= R_4 && !blocks[n].disabled)
        {
            /*rule4*/
            n4++;
            q4 += Eigen::Vector3d(blocks[n].x - boid.x, blocks[n].y - boid.y, blocks[n].z - boid.z) / dist / dist * R_4;
        }
    }
    //	if (isPress)
    //	{
    //		double dist = calcDist(boid.x, boid.y, mouseX, mouseY);
    //		if (dist - BOID_SIZE <= R_4)
    //		{
    //			/*rule4*/
    //			n4++;
    //			q4 += Eigen::Vector3d(mouseX - boid.x, mouseY - boid.y) / dist / dist * R_4;
    //		}
    //	}
    if (n1 != 0)
    {
        q1 /= double(n1);
    }
    if (n2 != 0)
    {
        q2 /= double(n2);
    }
    if (n3 != 0)
    {
        q3 /= double(n3);
    }
    if (n4 != 0)
    {
        q4 /= double(n4);
    }
    
    /*wall repel*/
    Eigen::Vector3d wallRepel = Eigen::Vector3d::Zero();
    double wall = BOUNDARY - WALL_SIZE;
    double bound = wall - BOID_SIZE - R_4;
    if (boid.x >= bound)
    {
        wallRepel.x() = 1.0 / (wall - boid.x);
    }
    else if (boid.x <= -bound)
    {
        wallRepel.x() = -1.0 / (wall + boid.x);
    }
    if (boid.y >= bound)
    {
        wallRepel.y() = 1.0 / (wall - boid.y);
    }
    else if (boid.y <= -bound)
    {
        wallRepel.y() = -1.0 / (wall + boid.y);
    };
    if (boid.z >= bound)
    {
        wallRepel.z() = 1.0 / (wall - boid.z);
    }
    else if (boid.z <= -bound)
    {
        wallRepel.z() = -1.0 / (wall + boid.z);
    }
    Eigen::Vector3d V = ALPHA_1 * q1.normalized() + ALPHA_2 * q2 - ALPHA_3 * q3 - ALPHA_4 * q4 + ALPHA_5 * boid.vctr.normalized() - REPEL_WALL_WEIGHT * wallRepel;
    Direction dir = Direction(V);
    boid.angleY = dir.angleY;
    boid.angleZ = dir.angleZ;
    boid.speed = BETA * log(V.norm() + 1.0);
    boid.vctr = boid.speed * dir.vector;
    
    if (boid.id == 0)
    {
        //		std::cout << "speed; " << boid.speed << "angleY; " << boid.angleY << "angleZ; " << boid.angleZ << std::endl;
        //		std::cout << "pos; " << boid.x << "; " << boid.y << "; " << boid.z << std::endl;
    }
    return boid;
}

void drawWall()
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    glBegin(GL_QUADS);
    for (int j = 0; j < 6; ++j)
    {
        for (int i = 0; i < 4; ++i)
        {
            glVertex3dv(vertex[face[j][i]]);
        }
    }
    glEnd();
}


//this function needs grids
void createGrids()
{
    double width = 2.0 * BOUNDARY / GRID_NO;
    double left;
    double front;
    double top = -BOUNDARY - width;
    for (int i = 0; i < GRID_NO + 2; ++i)
    {
        left = -BOUNDARY - width;
        for (int j = 0; j < GRID_NO + 2; ++j)
        {
            front = -BOUNDARY - width;
            for (int k = 0; k < GRID_NO + 2; ++k)
            {
                grids[i][j][k] = Grid(top, top + width, left, left + width, front, front + width);
                front += width;
            }
            left += width;
        }
        top += width;
    }
}

//this function needs grids, boids: 非効率かも
void updateGrids()
{
    for (int i = 1; i <= GRID_NO; i++)
    {
        for (int j = 1; j <= GRID_NO; j++)
        {
            for (int k = 1; k < GRID_NO; ++k)
            {
                std::vector<int> indexes = grids[i][j][k].boidIndexes;
                for (auto n : indexes)
                {
                    if (boids[n].grid_x != i || boids[n].grid_y != j || boids[n].grid_z != k)
                    {
                        grids[i][j][k].deleteBoidByIndex(n);
                    }
                }
            }
        }
    }
}

//this function needs grids, boids
void findGrid(int index, double x, double y, double z)
{
    double width = 2.0 * BOUNDARY / GRID_NO;
    int gridx = int(ceil((BOUNDARY + x) / width));
    int gridy = int(ceil((BOUNDARY + y) / width));
    int gridz = int(ceil((BOUNDARY + z) / width));
    grids[gridy][gridx][gridz].addBoidByIndex(index);
    boids[index].grid_x = gridx;
    boids[index].grid_y = gridy;
    boids[index].grid_z = gridz;
}

void whereBlock(int index, double x, double y, double z)
{
    double width = 2.0 * BOUNDARY / GRID_NO;
    int gridx = int(ceil((BOUNDARY + x) / width));
    int gridy = int(ceil((BOUNDARY + y) / width));
    int gridz = int(ceil((BOUNDARY + z) / width));
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            for (int k = -1; k <= 1; ++k)
            {
                grids[gridx + i][gridy + j][gridz + k].addBlockByIndex(index);
            }
        }
    }
}

void removeBlock(int index, double x, double y, double z)
{
    double width = 2.0 * BOUNDARY / GRID_NO;
    int gridx = int(ceil((BOUNDARY + x) / width));
    int gridy = int(ceil((BOUNDARY + y) / width));
    int gridz = int(ceil((BOUNDARY + z) / width));
    
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            for (int k = -1; k <= 1; ++k)
            {
                grids[gridx + i][gridy + j][gridz + k].deleteBlockByIndex(index);
            }
        }
    }
    blocks[index].setDisabled();
}

void removeAllBlocks()
{
    for (int i = 0; i < GRID_NO + 2; ++i)
    {
        for (int j = 0; j < GRID_NO + 2; ++j)
        {
            for (int k = 0; k < GRID_NO + 2; ++k)
            {
                grids[i][j][k].deleteAllBlocks();
            }
        }
    }
    blocks.clear();
}

int findDuplicateBlock(double x, double y, double z)
{
    double width = 2.0 * BOUNDARY / GRID_NO;
    int gridx = int(ceil((BOUNDARY + x) / width));
    int gridy = int(ceil((BOUNDARY + y) / width));
    int gridz = int(ceil((BOUNDARY + z) / width));
    for (auto i : grids[gridx][gridy][gridz].blockIndexes)
    {
        double dist = calcDist(x, y, z, blocks[i].x, blocks[i].y, blocks[i].z);
        if (dist <= 2.0 * BLOCK_SIZE && !blocks[i].disabled)
        {
            return i;
        }
    }
    return -1;
}

/*
 void display()
 {
	static int r = 0; /* 回転角 #1#
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* 光源の位置設定 #1#
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
	/* モデルビュー変換行列の保存 #1#
	glPushMatrix();
	glTranslated(-BOUNDARY, 0.0, 0.0);
	/* 図形の回転 #1#
	glRotated(double(r * 10), 0.0, 1.0, 0.0);
	/* 図形の色 (赤) #1#
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
	/* 図形の描画 #1#
	glutSolidCone(100.0, 100.0, 50, 50);
	/* モデルビュー変換行列の復帰 #1#
	glPopMatrix();
	/* モデルビュー変換行列の保存 #1#
	glPushMatrix();
	glTranslated(BOUNDARY, 0.0, 0.0);
	/* 図形の回転 #1#
	glRotated(double(r * 10), 0.0, 1.0, 0.0);
	/* 図形の色 (赤) #1#
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
	/* 図形の描画 #1#
	glutSolidCone(100.0, 100.0, 50, 50);
	/* モデルビュー変換行列の復帰 #1#
	glPopMatrix();
	/* モデルビュー変換行列の保存 #1#
	glPushMatrix();
	glTranslated(0.0, BOUNDARY, 0.0);
	/* 図形の回転 #1#
	glRotated(double(r * 10), 0.0, 1.0, 0.0);
	/* 図形の色 (赤) #1#
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
	/* 図形の描画 #1#
	glutSolidCone(100.0, 100.0, 50, 50);
	/* モデルビュー変換行列の復帰 #1#
	glPopMatrix();
	/* モデルビュー変換行列の保存 #1#
	glPushMatrix();
	glTranslated(0.0, -BOUNDARY, 0.0);
	/* 図形の回転 #1#
	glRotated(double(r * 10), 0.0, 1.0, 0.0);
	/* 図形の色 (赤) #1#
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
	/* 図形の描画 #1#
	glutSolidCone(100.0, 100.0, 50, 50);
	/* モデルビュー変換行列の復帰 #1#
	glPopMatrix();
	/* モデルビュー変換行列の保存 #1#
	glPushMatrix();
	glTranslated(0.0, 0.0, -BOUNDARY);
	/* 図形の回転 #1#
	glRotated(double(r * 10), 0.0, 1.0, 0.0);
	/* 図形の色 (赤) #1#
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
	/* 図形の描画 #1#
	glutSolidCone(100.0, 100.0, 50, 50);
	/* モデルビュー変換行列の復帰 #1#
	glPopMatrix();
	glutSwapBuffers();
	/* 一周回ったら回転角を 0 に戻す #1#
	if (++r >= 36) r = 0;
 }
 */


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /* 光源の位置設定 */
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
    //	drawWall();
    glPushMatrix();
    glTranslated(BOUNDARY, BOUNDARY, BOUNDARY);
    /* 図形の色 (赤) */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    
    /* 図形の描画 */
    glutSolidCone(50.0, 50.0, 20, 20);
    /* モデルビュー変換行列の復帰 */
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-BOUNDARY, BOUNDARY, BOUNDARY);
    /* 図形の色 (赤) */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    
    /* 図形の描画 */
    glutSolidCone(50.0, 50.0, 20, 20);
    /* モデルビュー変換行列の復帰 */
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(BOUNDARY, -BOUNDARY, BOUNDARY);
    /* 図形の色 (赤) */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    
    /* 図形の描画 */
    glutSolidCone(50.0, 50.0, 20, 20);
    /* モデルビュー変換行列の復帰 */
    glPopMatrix();
    glPushMatrix();
    glTranslated(-BOUNDARY, -BOUNDARY, BOUNDARY);
    /* 図形の色 (赤) */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    
    /* 図形の描画 */
    glutSolidCone(50.0, 50.0, 20, 20);
    /* モデルビュー変換行列の復帰 */
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(BOUNDARY, BOUNDARY, -BOUNDARY);
    /* 図形の色 (赤) */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    
    /* 図形の描画 */
    glutSolidCone(50.0, 50.0, 20, 20);
    /* モデルビュー変換行列の復帰 */
    glPopMatrix();
    glPushMatrix();
    glTranslated(-BOUNDARY, BOUNDARY, -BOUNDARY);
    /* 図形の色 (赤) */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    
    /* 図形の描画 */
    glutSolidCone(50.0, 50.0, 20, 20);
    /* モデルビュー変換行列の復帰 */
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(BOUNDARY, -BOUNDARY, -BOUNDARY);
    /* 図形の色 (赤) */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    
    /* 図形の描画 */
    glutSolidCone(50.0, 50.0, 20, 20);
    /* モデルビュー変換行列の復帰 */
    glPopMatrix();
    glPushMatrix();
    glTranslated(-BOUNDARY, -BOUNDARY, -BOUNDARY);
    /* 図形の色 (赤) */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    
    /* 図形の描画 */
    glutSolidCone(50.0, 50.0, 20, 20);
    /* モデルビュー変換行列の復帰 */
    glPopMatrix();
    drawWall();
    for (auto boid : boids)
    {
        boid.drawBaseBoid();
    }
    for (auto block : blocks)
    {
        if (!block.disabled)
        {
            block.drawBlock();
        }
    }
    //	glutSwapBuffers();
    glFlush();
}

void resize(int w, int h)
{
    //	glViewport(0, 0, w, h);
    //
    //	/* 透視変換行列の設定 */
    //	glMatrixMode(GL_PROJECTION);
    //	glLoadIdentity();
    //	gluPerspective(30.0, double(w) / double(h), -w / WINDOW_SIZE * BOUNDARY, w / WINDOW_SIZE * BOUNDARY);
    //	gluPerspective(30.0, double(w) / double(h), 1.0, 100.0);
    //	glOrtho(-w / WINDOW_SIZE * BOUNDARY, w / WINDOW_SIZE * BOUNDARY, -h / WINDOW_SIZE * BOUNDARY, h / WINDOW_SIZE * BOUNDARY, -h / WINDOW_SIZE * BOUNDARY, h / WINDOW_SIZE * BOUNDARY);
    //
    //	/* モデルビュー変換行列の設定 */
    //	glMatrixMode(GL_MODELVIEW);
    //	glLoadIdentity();
    //	gluLookAt(1.0, 1.0, -w / WINDOW_SIZE * BOUNDARY, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    glViewport(0, 0, w, h);
    
    /* 透視変換行列の設定 */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
    gluPerspective(120.0, double(w) / double(h), 1.0, double(w) / WINDOW_SIZE * BOUNDARY * 4.0);
    //	glOrtho(-w / WINDOW_SIZE * BOUNDARY, w / WINDOW_SIZE * BOUNDARY, -h / WINDOW_SIZE * BOUNDARY, h / WINDOW_SIZE * BOUNDARY, -1.0, 1.0);
    
    /* モデルビュー変換行列の設定 */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //	gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    gluLookAt(0.0, 0.0, w / WINDOW_SIZE * BOUNDARY * 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    //	gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

/*
 void mouse(int button, int state, int x, int y)
 {
	double pos_x = BOUNDARY * (double(x) - WINDOW_SIZE / 2.0) / double(WINDOW_SIZE / 2.0);
	double pos_y = -BOUNDARY * (double(y) - WINDOW_SIZE / 2.0) / double(WINDOW_SIZE / 2.0);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
 int index = findDuplicateBlock(pos_x, pos_y);
 if (index != -1)
 {
 std::cout << "exist" << index << std::endl;
 removeBlock(index, pos_x, pos_y);
 }
 else
 {
 blocks.push_back(Block(pos_x, pos_y, BLOCK_SIZE));
 whereBlock(blocks.size() - 1, blocks[blocks.size() - 1].x, blocks[blocks.size() - 1].y);
 }
	}
	if (button == GLUT_RIGHT_BUTTON)
	{
 if (state == GLUT_DOWN)
 {
 std::cout << "pressing" << std::endl;
 mouseX = pos_x;
 mouseY = pos_y;
 isPress = true;
 }
 else
 {
 isPress = false;
 }
	}
 }
 void motion(int x, int y)
 {
	if (isPress)
	{
 double pos_x = BOUNDARY * (double(x) - WINDOW_SIZE / 2.0) / double(WINDOW_SIZE / 2.0);
 double pos_y = -BOUNDARY * (double(y) - WINDOW_SIZE / 2.0) / double(WINDOW_SIZE / 2.0);
 mouseX = pos_x;
 mouseY = pos_y;
	}
 }
 */

void key(unsigned char key, int x, int y)
{
    if (key == 'r')
    {
        std::cout << "refresh" << std::endl;
        removeAllBlocks();
    }
    if (key == 'b')
    {
        removeAllBlocks();
        for (int i = 0; i < BLOCK_NO; ++i)
        {
            blocks.push_back(Block((double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - BLOCK_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - BLOCK_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - BLOCK_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, BLOCK_SIZE));
            whereBlock(i, blocks[i].x, blocks[i].y, blocks[i].z);
        }
    }
}

void timer(int value)
{
    //	if (time % 10 == 0)
    //	{
    //		cout << time / 10 << endl;
    //	}
    for (int i = 0; i < boids.size(); i++)
    {
        boids[i].updatePosition();
        if (i != 0)
        {
            boids[i].setColor(1.0, 1.0, 1.0);
        }
        findGrid(i, boids[i].x, boids[i].y, boids[i].z);
    }
    updateGrids();
    for (int i = 0; i < boids.size(); i++)
    {
        //boid速度ベクトルの計算部分
        boids[i] = updateSpeedAndAngle(boids[i]);
    }
    glutPostRedisplay();
    tim++;
    glutTimerFunc(FLAME_RATE, timer, tim);
}


void init()
{
    glClearColor(0.0, 0.0, 1.0, 0.5);
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, green);
    glLightfv(GL_LIGHT1, GL_SPECULAR, green);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    //	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    
    glutCreateWindow(argv[0]);
    //	glutMouseFunc(mouse);
    glutKeyboardFunc(key);
    init();
    createGrids();
    
    for (int i = 0; i < BOIDS_NO; i++)
    {
        boids.push_back(BaseBoid((double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - WALL_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - WALL_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - WALL_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) / RAND_MAX) * 2.0 * M_PI, (double(rand()) / RAND_MAX) * 2.0 * M_PI, BOID_SPEED, i));
        findGrid(i, boids[i].x, boids[i].y, boids[i].z);
        if (i == 0)
        {
            boids[i].setColor(1.0, 0.0, 0.0);
        }
    }
    for (int i = 0; i < BLOCK_NO; ++i)
    {
        blocks.push_back(Block((double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - BLOCK_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - BLOCK_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - BLOCK_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, BLOCK_SIZE));
        whereBlock(i, blocks[i].x, blocks[i].y, blocks[i].z);
    }
    updateGrids();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutTimerFunc(FLAME_RATE, timer, tim);
    glutMainLoop();
    return 0;
}
