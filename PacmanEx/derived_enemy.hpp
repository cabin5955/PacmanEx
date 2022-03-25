//
//  derived_enemy.hpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/16.
//

#ifndef derived_enemy_hpp
#define derived_enemy_hpp

#include "enemy.hpp"

class Blinky : public Enemy{
public:
    Blinky(int x, int y);
    virtual void SetGoal(int &goalX, int &goalY, int pacX, int pacY,int dirX, int dirY);
};

class Pinky : public Enemy{
public:
    Pinky(int x, int y);
    virtual void SetGoal(int &goalX, int &goal_y, int pacX, int pac_y,int dirX, int dirY);
};

#endif /* derived_enemy_hpp */
