//
//  player.hpp
//
//  Created by chen caibin on 2022/2/20.
//

#ifndef player_hpp
#define player_hpp

#include "animation.hpp"
#include "game_object.hpp"
#include "model.hpp"

#define G 16
#define PLAYER_JUMP_SPD 8.0f

class Player:public GameObject {
public:
    
    bool isWalk;
    bool isAlive = true;
    float timerAnimation;
    float speed;
    bool canJump;
    
    Animation walk_ani;
    Animation idle_ani;
    Animation die_ani;
    
    Model *model;
    
    Player(float x, float y,float z);
    void Update();
    bool IsAlive();
    void Die();
    
    static int _xToEnemy;
    static int _yToEnemy;
    static int _dirxToEnemy;
    static int _diryToEnemy;
    static float _timerScared;
    static int _scaredTime;
    
    static int GetX();
    static int GetY();
    static int GetDirX();
    static int GetDirY();
};

#endif /* player_hpp */
