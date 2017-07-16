//
//  parameter.h
//  boid
//
//  Created by Ryota Ishidu on 2017/05/04.
//  Copyright © 2017年 Ryota Ishidu. All rights reserved.
//

#ifndef parameter_h
#define parameter_h
/*parameters*/
#define GRID_NO 8 //number of grid; boid can see around 1 grid
#define BOID_SIZE 10.0 //size of boid
#define WALL_SIZE 5.0 //size of wall
#define BLOCK_SIZE 10.0 //size of block
#define MOUSE_SIZE BLOCK_SIZE //size of mouse
#define BOID_SPEED 3.0 //initial boid speed
#define BOIDS_NO 50 //number of boids
#define BLOCK_NO 0 //number of blocks
#define FLAME_RATE 100 //rerender after this FLAME_RATE milliseconds
#define WINDOW_SIZE 600 //window size
#define BOUNDARY 200.0 //area boundary
#define REPEL_WALL_WEIGHT 30.0 //repel force weight
#define MOUSE_DISTRACTION_FORCE 2.0 //repel force weight
#define MOUSE_ATTRACTION_FORCE 6.0 //repel force weight
#define ALPHA_1 0.5 //rule1 weight
#define ALPHA_2 1.1 //rule2 weight
#define ALPHA_3 2.0 //rule3 weight
#define ALPHA_4 5.0 //rule4 weight
#define ALPHA_5 2.0 //rule5 weight
#define R_1 50.0 //rule1 range
#define R_2 50.0 //rule2 range
#define R_3 30.0 //rule3 range
#define R_4 40.0 //rule4 range
#define THETA_1 240.0 //rule1 view angle
#define THETA_2 240.0 //rule2 view angle
#define THETA_3 240.0 //rule3 view angle
#define THETA_4 240.0 //rule4 view angle
#define BETA 100.0 //boid speed factor

#endif /* parameter_h */
