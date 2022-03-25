//
//  derived_enemy.cpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/16.
//

#include "derived_enemy.hpp"
#include "global.hpp"
#include "map_data.hpp"

Blinky::Blinky(int x, int y):
    Enemy(x,y, RED){}

void Blinky::SetGoal(int &goalX, int &goalY, int pacX, int pacY,int dirX, int dirY){
    if(_alive){
        if(!_scared){
            //After a while the ghost goes to its corner
            if(_scatter){
                goalX = 1;
                goalY = 1;
                if(_timerScatter > 0){
                    _timerScatter -= 5*Global::GetFrameTime();
                }
            }else{
                //follows pacman
                goalX = pacX;
                goalY = pacY;
            }
        }else{
            //When he is afraid he also goes to his corner
            goalX = 1;
            goalY = 1;
        }
    }else{
        goalX = 10;
        goalY = 10;
    }
}

Pinky::Pinky(int x, int y):
    Enemy(x,y, PINK){}

void Pinky::SetGoal(int &goalX, int &goalY, int pacX, int pacY,int dirX, int dirY){
    if(_alive){
        if(!_scared){
           if(_scatter){
                goalX = MAP_WIDTH -2;
                goalY = 1;
                if(_timerScatter > 0){
                    _timerScatter -= 5*Global::GetFrameTime();
                }
            }
            else{
                //The goal is two positions in front of the pacman
                //right
                if(dirX == 1 && dirY == 0){
                    goalX = pacX + 2;
                    goalY = pacY;
                }
                //left
                else if(dirX == -1 && dirY == 0){
                    goalX = pacX - 2;
                    goalY = pacY;
                }
                //top
                else if(dirX == 0 && dirY == -1){
                    goalX = pacX;
                    goalY = pacY - 2;
                }
                //bottom
                else if(dirX == 0 && dirY == 1){
                    goalX = pacX;
                    goalY = pacY + 2;
                }
            }
        }else{
            goalX = MAP_WIDTH - 2;
            goalY = 1;
        }
    }else{
        goalX = 10;
        goalY = 10;
    }
}
