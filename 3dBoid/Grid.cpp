//
//  Grid.cpp
//  boid
//
//  Created by Ryota Ishidu on 2017/05/04.
//  Copyright © 2017年 Ryota Ishidu. All rights reserved.
//

#include "Grid.hpp"
#include <vector>
#include <algorithm>
Grid::Grid(double _top, double _bottom, double _left, double _right, double _front, double _back)
{
    left = _left;
    right = _right;
    top = _top;
    bottom = _bottom;
    front = _front;
    back = _back;
}
void Grid::addBoidByIndex(int index)
{
    boidIndexes.push_back(index);
    sort(boidIndexes.begin(), boidIndexes.end());
}
void Grid::deleteBoidByIndex(int index)
{
    if (find(boidIndexes.begin(), boidIndexes.end(), index) != boidIndexes.end())
    {
        remove(boidIndexes.begin(), boidIndexes.end(), index);
        boidIndexes.pop_back();
    }
}
bool Grid::findBoidByIndex(int index)
{
    if (find(boidIndexes.begin(), boidIndexes.end(), index) != boidIndexes.end())
    {
        return true;
    }
    return false;
}
void Grid::addBlockByIndex(int index)
{
    blockIndexes.push_back(index);
    sort(blockIndexes.begin(), blockIndexes.end());
}
void Grid::deleteBlockByIndex(int index)
{
    if (find(blockIndexes.begin(), blockIndexes.end(), index) != blockIndexes.end())
    {
        remove(blockIndexes.begin(), blockIndexes.end(), index);
        blockIndexes.pop_back();
    }
}
void Grid::deleteAllBlocks()
{
    blockIndexes.clear();
}
