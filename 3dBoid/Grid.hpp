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
    double front;
    double back;
    std::vector<int> boidIndexes;
    std::vector<int> blockIndexes;
    
    /**
     * \brief
     * \param top
     * \param bottom
     * \param left
     * \param right
     */
    Grid(double top = 0.0, double bottom = 0.0, double left = 0.0, double right = 0.0, double front = 0.0, double back = 0.0);
    /**
     * \brief
     * \param index
     */
    void addBoidByIndex(int index);
    /**
     * \brief
     * \param index
     */
    void deleteBoidByIndex(int index);
    /**
     * \brief
     * \param index
     * \return
     */
    bool findBoidByIndex(int index);
    /**
     * \brief
     * \param index
     */
    void addBlockByIndex(int index);
    /**
     * \brief
     * \param index
     */
    void deleteBlockByIndex(int index);
    /**
     * \brief
     */
    void deleteAllBlocks();
};

#endif /* Grid_hpp */
