//
//  player.cpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/2/20.
//

#include "player.hpp"
#include "global.hpp"
#include "model_cache.hpp"
#include "astar_map.hpp"

int Player::_xToEnemy = 0;
int Player::_yToEnemy = 0;
int Player::_dirxToEnemy = 0;
int Player::_diryToEnemy = 0;
int Player::_scaredTime = 0;
float Player::_timerScared = 0.0f;

Player::Player(float x, float y,float z) :
walk_ani("player_walk", 6, true),
idle_ani("player_idle", 1, true),
die_ani("player_die", 18, false)
{
    walk_ani.Preload();
    idle_ani.Preload();
    die_ani.Preload();

    model = ModelCache::LoadModel("player_walk_0.obj");
    Position = {x, y, z};
    RotationAxis = {0,1,0};
    
    isWalk = true;
    isAlive = true;
    timerAnimation = 0.0;
}

void Player::Update()
{
    float delta = Global::GetFrameTime();
    // Update
    //----------------------------------------------------------------------------------
    timerAnimation += delta * 9 /*Frames por segundo*/;
    
    if (Position.y>1.0 )
    {
        Position.y += speed*delta;
        speed += -G*delta;
        canJump = false;
        
        if(Position.y < 1.0f)
        {
            Position.y = 1.0f;
        }
    }
    else {
        canJump = true;
    }
    
    if(!isAlive)
    {
        die_ani.SetTimer(timerAnimation);
        model = ModelCache::LoadModel(die_ani.GetCurrentFrame());
    }
    else if(isWalk)
    {
        walk_ani.SetTimer(timerAnimation);
        model = ModelCache::LoadModel(walk_ani.GetCurrentFrame());
    }
    else
    {
        idle_ani.SetTimer(timerAnimation);
        model = ModelCache::LoadModel(idle_ani.GetCurrentFrame());
    }
}

int Player::GetX(){
    return _xToEnemy;
}
int Player::GetY(){
    return _yToEnemy;
}

int Player::GetDirX(){
    return _dirxToEnemy;
}
int Player::GetDirY(){
    return _diryToEnemy;
}

bool Player::IsAlive(){
    return isAlive;
}

void Player::Die(){
    isAlive = false;
//    if(_vidas>0)
//        _vidas--;
//
    timerAnimation = 0;
}
