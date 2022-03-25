//
//  enemy.hpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/15.
//

#ifndef enemy_hpp
#define enemy_hpp

#include <vector>
#include <list>
#include <stdexcept>
#include <cmath>

#include "map_data.hpp"
#include "utils.h"
#include "model_object.hpp"
#include "color.h"
#include "astar_map.hpp"

#define SCATTER_TIME 20
#define GHOST_SPEED_DEFAULT 3
#define GHOST_SPEED_SCARED 1.25

class Enemy{
protected:
    ModelObject _model;
    int _spawnX;
    int _spawnY;
    int _x, _y;
    int _dirY;
    int _dirX;

    bool _scatter;
    static bool _scared;
    bool _alive;
    static bool _isOver;
    
    float _velocity;
    Color _color;
    
    static std::vector<Enemy*> _enemies;
    
    float _timerMovement;
    float _timerScatter;
    
    int _goalX, _goalY;
    int _lastGoalX, _lastGoalY;
    std::vector<NodeItem*> _path;
    int FindPathNode(int x,int y);
    NodeItem *FindNeibourhoodInPath(int x,int y);
    void GeneratePath(int goalX,int goalY);

public:
    Enemy(int _x, int _y, Color _color);
    virtual void SetGoal(int &goalX, int &goalY, int pacX, int pacY,int dirX, int dirY) = 0;
    static void AddEnemy(Enemy* _enemy);
    static void RemoveEnemies();
    static void IsOver(bool state);
    static std::vector<Enemy*> &GetEnemies();
    static void GetScared(bool state);
    static bool IsScared();
    static void DrawEnemies(Shader &meshader);
    static void UpdateEnemies();
    
    void Die();
    Color GetColor();
    float GetX();
    float GetY();
    bool GetAlive();
    ModelObject& GetModel();

    void OnUpdate();
    void OnRestart();
    void OnUpdateAStarMap();
};

#endif /* enemy_hpp */
