//
//  Direction.hpp
//  boid
//
//  Created by Ryota Ishidu on 2017/05/04.
//  Copyright © 2017年 Ryota Ishidu. All rights reserved.
//

#ifndef Direction_hpp
#define Direction_hpp

#include <stdio.h>
#include "Eigen/Core"

class Direction
{
public:
    double angle;
    double x;
    double y;
    Eigen::Vector2d vector;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    /**
     * \brief
     * \param _angle
     */
    Direction(double _angle);
    /**
     * \brief
     * \param _x
     * \param _y
     */
    Direction(double _x, double _y);
    /**
     * \brief
     * \param point
     */
    Direction(Eigen::Vector2d& point);
};

#endif /* Direction_hpp */
