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
int mouseState = 0; //0 is not pressed, 1 is distractor, 2 is attractor
double mouseX = 0.0;
double mouseY = 0.0;
double mouseZ = 0.0;
GLfloat light0pos[] = {0.0, 0.0, BOUNDARY, 1.0};
GLfloat light1pos[] = {0.0, 0.0, BOUNDARY, 1.0};

GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
GLfloat red[] = {0.8, 0.2, 0.2, 1.0};
GLfloat blue[] = {0.0, 0.0, 1.0, 0.6};
GLfloat white[] = {0.5, 0.5, 0.5, 0.4};

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
    std::vector<int> indexes = grids[grid_x][grid_y][grid_z].boidIndexes;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            for (int k = -1; k <= 1; ++k)
            {
                indexes.insert(indexes.end(), grids[grid_x + i][grid_y + j][grid_z + k].boidIndexes.begin(), grids[grid_x + i][grid_y + j][grid_z + k].boidIndexes.end());
            }
        }
    }
    auto result = remove(indexes.begin(), indexes.end(), id);
    auto result2 = unique(indexes.begin(), result);
    indexes.erase(result2, indexes.end());
    return indexes;
}

std::vector<BaseBoid> boids;
std::vector<std::pair<int, int>> boidConnections;

void addConnections(std::pair<int, int> newConnection)
{
    boidConnections.push_back(newConnection);
    std::sort(boidConnections.begin(), boidConnections.end());
    auto result = std::unique(boidConnections.begin(), boidConnections.end());
    boidConnections.erase(result, boidConnections.end());
}

void removeAllConnections()
{
    boidConnections.clear();
}

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
        //		std::cout << "boid " << boid.id << " can see boid " << boids[i].id << std::endl;
        double dist = calcDist(boid.x, boid.y, boid.z, boids[i].x, boids[i].y, boids[i].z);
        
        if (boid.isVisible(boids[i].x, boids[i].y, boids[i].z, _viewAngle))
        {
            /*boidが見える範囲内にいる*/
//            if (boid.id == 0)
//            {
//                boids[i].setColor(0.0, 1.0, 1.0);
//            }
            if (dist - boid.size - boids[i].size < R_1)
            {
                int first;
                int second;
                if (boid.id < boids[i].id)
                {
                    first = boid.id;
                    second = boids[i].id;
                }
                else
                {
                    first = boids[i].id;
                    second = boid.id;
                }
                std::pair<int, int> connection = std::make_pair(first, second);
                addConnections(connection);
                /*rule1*/
                n1++;
                q1 += boids[i].vctr.normalized();
//                if (boid.id == 0)
//                {
//                    boids[i].setColor(1.0, 1.0, 0.0);
//                }
            }
            if (dist - boid.size - boids[i].size < R_2)
            {
                /*rule2*/
                n2++;
                q2 += Eigen::Vector3d(boids[i].x - boid.x, boids[i].y - boid.y, boids[i].z - boid.z) / dist / dist * R_2;
//                if (boid.id == 0)
//                {
//                    boids[i].setColor(1.0, 1.0, 0.0);
//                }
            }
            if (dist - boid.size - boids[i].size < R_3)
            {
                /*rule3*/
                n3++;
                q3 += Eigen::Vector3d(boids[i].x - boid.x, boids[i].y - boid.y, boids[i].z - boid.z) / dist / dist * R_3;
//                if (boid.id == 0)
//                {
//                    boids[i].setColor(0.0, 1.0, 0.0);
//                }
            }
        }
    }
    /*loop ends here*/
    for (int n : grids[boid.grid_x][boid.grid_y][boid.grid_z].blockIndexes)
    {
        double dist = calcDist(boid.x, boid.y, boid.z, blocks[n].x, blocks[n].y, blocks[n].z);
        if (dist - BLOCK_SIZE - boid.size <= R_4 && !blocks[n].disabled)
        {
            /*rule4*/
            n4++;
            q4 += Eigen::Vector3d(blocks[n].x - boid.x, blocks[n].y - boid.y, blocks[n].z - boid.z) / dist / dist * R_4;
        }
    } if (mouseState != 0)
    {
        double dist = calcDist(boid.x, boid.y, boid.z, mouseX, mouseY, mouseZ);
        if (mouseState == 1)
        {
            if (dist - MOUSE_SIZE - boid.size <= R_4)
            {
                /*rule4*/
                n4++;
                q4 += MOUSE_DISTRACTION_FORCE * Eigen::Vector3d(mouseX - boid.x, mouseY - boid.y, mouseZ - boid.z) / dist / dist * R_4;
            }
        }
        if (mouseState == 2)
        {
            if (true)
            {
                /*rule2*/
                n2++;
                q2 += MOUSE_ATTRACTION_FORCE * Eigen::Vector3d(mouseX - boid.x, mouseY - boid.y, mouseZ - boid.z) / dist / dist * R_2;
            }
            if (dist - MOUSE_SIZE - boid.size < R_3)
            {
                /*rule3*/
                n3++;
                q3 += MOUSE_ATTRACTION_FORCE * Eigen::Vector3d(mouseX - boid.x, mouseY - boid.y, mouseZ - boid.z) / dist / dist * R_3;
            }
        }
    }
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
    return boid;
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
                grids[i][j][k] = Grid(left, left + width, top, top + width,  front, front + width);
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
            for (int k = 1; k <= GRID_NO; ++k)
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
    grids[gridx][gridy][gridz].addBoidByIndex(index);
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

void drawWall()
{
    double bound = BOUNDARY;
    GLfloat color[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    glBegin(GL_POLYGON);
    glColor3d(1.0, 0.0, 0.0);
    glVertex3d(bound, bound, bound);
    glVertex3d(bound, -bound, bound);
    glVertex3d(bound, -bound, -bound);
    glVertex3d(bound, bound, -bound);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3d(0.0, 1.0, 0.0);
    glVertex3d(-bound, bound, bound);
    glVertex3d(-bound, bound, -bound);
    glVertex3d(-bound, -bound, -bound);
    glVertex3d(-bound, -bound, bound);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3d(0.0, 0.0, 1.0);
    glVertex3d(bound, bound, bound);
    glVertex3d(bound, bound, -bound);
    glVertex3d(-bound, bound, -bound);
    glVertex3d(-bound, bound, bound);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3d(1.0, 1.0, 0.0);
    glVertex3d(bound, -bound, bound);
    glVertex3d(-bound, -bound, bound);
    glVertex3d(-bound, -bound, -bound);
    glVertex3d(bound, -bound, -bound);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3d(1.0, 0.0, 1.0);
    glVertex3d(bound, bound, -bound);
    glVertex3d(bound, -bound, -bound);
    glVertex3d(-bound, -bound, -bound);
    glVertex3d(-bound, bound, -bound);
    glEnd();
}

void drawConnections()
{
    glColor3d(1.0, 1.0, 1.0);
    GLfloat color[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    for (auto pair: boidConnections)
    {
        glBegin(GL_LINES);
        glVertex3d(boids[pair.first].x, boids[pair.first].y, boids[pair.first].z);
        glVertex3d(boids[pair.second].x, boids[pair.second].y, boids[pair.second].z);
        glEnd();
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //	glClear(GL_COLOR_BUFFER_BIT);
    
    //	/* 光源の位置設定 */
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
    
    drawWall();
    drawConnections();
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
    if (mouseState!=0)
    {
        int slice = 10;
        double r;
        double g;
        double b;
        if (mouseState==1) {
            r = 1.0;
            g = 0.0;
            b = 0.0;
        }else{
            r = 0.0;
            g = 1.0;
            b = 0.0;
        }
        GLfloat col[] = {GLfloat(r),GLfloat(g),GLfloat(b),1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, col);
        glColor3d(r, g, b);
        glPushMatrix();
        glTranslated(mouseX,mouseY, mouseZ);
        double angle = double(tim % 36)*10.0;
        glRotated(angle, 0.0, 1.0, 0.0);
        glutSolidTeapot(MOUSE_SIZE);
        glPopMatrix();
    }
    glFlush();
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    /* 透視変換行列の設定 */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120.0, double(w) / double(h), 0.0001, double(w) / WINDOW_SIZE * BOUNDARY * 4.0);
    
    /* モデルビュー変換行列の設定 */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, double(w) / WINDOW_SIZE * BOUNDARY * 1.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


 void mouse(int button, int state, int x, int y)
 {
// double pos_x = BOUNDARY * (double(x) - WINDOW_SIZE / 2.0) / double(WINDOW_SIZE / 2.0);
// double pos_y = -BOUNDARY * (double(y) - WINDOW_SIZE / 2.0) / double(WINDOW_SIZE / 2.0);
 if (state == GLUT_DOWN)
 {
 if (button == GLUT_LEFT_BUTTON)
 {
 std::cout << "distractor" << std::endl;
// mouseX = pos_x;
// mouseY = pos_y;
 mouseState = 1;
 }
 if (button == GLUT_RIGHT_BUTTON)
 {
 std::cout << "attractor" << std::endl;
// mouseX = pos_x;
// mouseY = pos_y;
 mouseState = 2;
 }
 }
 else
 {
 mouseState = 0;
 }
 }
 

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
    //	if (tim % 10 == 0)
    //	{
    //		cout << tim / 10 << endl;
    //	}
    removeAllConnections();
    for (int i = 0; i < boids.size(); i++)
    {
        boids[i].updatePosition();
//        if (i != 0)
//        {
//            boids[i].setColor(1.0, 1.0, 1.0);
//        }
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
    glClearColor(0.0, 0.0, 0.0, 0.5);
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, blue);
    glLightfv(GL_LIGHT0, GL_SPECULAR, blue);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    //	glutInitDisplayMode(GLUT_RGBA);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    
    glutCreateWindow(argv[0]);
    glutMouseFunc(mouse);
    glutKeyboardFunc(key);
    init();
    createGrids();
    
    for (int i = 0; i < BOIDS_NO; i++)
    {
        boids.push_back(BaseBoid((double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - WALL_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - WALL_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - WALL_SIZE - BOID_SIZE) * 2.0 / RAND_MAX, (double(rand()) / RAND_MAX) * 2.0 * M_PI, (double(rand()) / RAND_MAX) * 2.0 * M_PI, BOID_SPEED, BOID_SIZE, i));
        findGrid(i, boids[i].x, boids[i].y, boids[i].z);
//        if (i == 0)
//        {
//            boids[i].setColor(1.0, 0.0, 0.0);
//        }
    }
    for (int i = 0; i < BLOCK_NO; ++i)
    {
        blocks.push_back(Block((double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - WALL_SIZE - BLOCK_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - WALL_SIZE - BLOCK_SIZE) * 2.0 / RAND_MAX, (double(rand()) - RAND_MAX / 2.0) * (BOUNDARY - WALL_SIZE - BLOCK_SIZE) * 2.0 / RAND_MAX, BLOCK_SIZE));
        whereBlock(i, blocks[i].x, blocks[i].y, blocks[i].z);
    }
    
    updateGrids();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutTimerFunc(FLAME_RATE, timer, tim);
    glutMainLoop();
    return 0;
}
