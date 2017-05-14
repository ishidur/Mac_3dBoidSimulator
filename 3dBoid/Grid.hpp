//
//  Grid.hpp
//  boid
//
//  Created by Ryota Ishidu on 2017/05/04.
//  Copyright © 2017年 Ryota Ishidu. All rights reserved.
//

#ifndef Grid_hpp
#define Grid_hpp

#include <stdio.h>
#include <vector>


class Grid
{
public:
    double left;
    double right;
    double top;
    double bottom;
    std::vector<int> boidIndexes;
    std::vector<int> blockIndexes;
    Grid(double _top = 0.0, double _bottom = 0.0, double _left = 0.0, double _right = 0.0);
    void addBoidByIndex(int index);
    void deleteBoidByIndex(int index);
    bool findBoidByIndex(int index);
    void addBlockByIndex(int index);
    void deleteBlockByIndex(int index);
    void deleteAllBoids();
    void deleteAllBlocks();
};
#endif /* Grid_hpp */
