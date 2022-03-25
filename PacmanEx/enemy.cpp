//
//  enemy.cpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/15.
//

#include "enemy.hpp"
#include "model_cache.hpp"
#include "player.hpp"
#include "global.hpp"
#include "resource_manager.hpp"

#define MAPA_VALIDO 0
#define MAPA_INVALIDO 1
#define POSICAO_INIMIGO 2
#define ITEM_PONTO 3
#define ITEM_FRUTA 4
#define VALOR_FRUTA 50

std::vector<Enemy*> Enemy::_enemies;
bool Enemy::_isOver = false;
bool Enemy::_scared = false;

Enemy::Enemy(int x, int y, Color color) : _model("ghost.obj",color){
    _spawnX = x;
    _spawnY = y;
    _x = x;
    _y = y;
    _model.Position = {(float)x, 0, (float)y};
    _dirY = 0;
    _dirX = 0;
    
    _scatter = false;
    _alive = true;

    _color = color;
    _velocity = GHOST_SPEED_DEFAULT;

    _timerMovement = 0.0;
    _timerScatter = 0.0;
    
    _goalX = 1.0;
    _goalY = 1.0;
    _lastGoalX = Player::GetX();
    _lastGoalY = Player::GetY();
    
    GeneratePath(_lastGoalX,_lastGoalY);
    if(_path.size()>2){
        _dirX = _path[1]->x - _path[0]->x;
        _dirY = _path[1]->y - _path[0]->y;
    }
}

void Enemy::GeneratePath(int goalX,int goalY){
    _path.clear();
    AStarMap map = AStarMap::GetMapGlobal();
    std::vector<NodeItem*> list = map.FindingPath({_x,0,_y},{goalX,0,goalY});
    _path.assign(list.begin(), list.end());
}

void Enemy::AddEnemy(Enemy* enemy){
    _enemies.push_back(enemy);
}

void Enemy::RemoveEnemies(){
    _enemies.clear();
}

std::vector<Enemy*> &Enemy::GetEnemies()
{
    return _enemies;
}

void Enemy::GetScared(bool state){
    _scared = state;
}

void Enemy::DrawEnemies(Shader &meshShader){
    std::vector<Enemy*>& enemies = _enemies;
    for(Enemy* e : enemies){
        ModelObject enemy = e->GetModel();
        MeshRenderer::DrawMesh(meshShader,*enemy.model,
                               ResourceManager::GetTexture("mask"),
                               enemy.Position,
                               enemy.Size,
                               enemy.RotationAngle*DEG2RAD,
                               enemy.RotationAxis,
                               enemy.color);
    }
}

void Enemy::UpdateEnemies(){
    std::vector<Enemy*>& enemies = _enemies;
    for(Enemy* e : enemies){
        e->OnUpdateAStarMap();
    }
}

void Enemy::Die(){
    _alive = false;
    _model.SetColor(WHITE);
    _model.model = ModelCache::LoadModel("ghost_dead.obj");
}

ModelObject& Enemy::GetModel(){
    return _model;
}

int Enemy::FindPathNode(int x,int y)
{
    for (int i = 0; i < _path.size(); i++) {
        if(_path[i]->x == x && _path[i]->y == y){
            return i;
        }
    }
    return -1;
}

NodeItem *Enemy::FindNeibourhoodInPath(int x,int y){
    for (int i = 0; i < _path.size(); i++) {
        if(_path[i]->x+1 == x && _path[i]->y == y){
            return _path[i];
        }
        if(_path[i]->x-1 == x && _path[i]->y == y){
            return _path[i];
        }
        if(_path[i]->x == x && _path[i]->y+1 == y){
            return _path[i];
        }
        if(_path[i]->x == x && _path[i]->y-1 == y){
            return _path[i];
        }
    }
    return NULL;
}

void Enemy::OnUpdateAStarMap(){
    if(!_isOver){
        int px = Player::GetX();
        int py = Player::GetY();
        int player_dirX =  Player::GetDirX();
        int player_dirY =  Player::GetDirY();
        
        _timerScatter += Global::GetFrameTime();
        if(_timerScatter > SCATTER_TIME) {
            _scatter = true;
        }
        if(_timerScatter <= 0){
            _scatter = false;
        }

        if(GetAlive() && IsScared()){
            _model.SetColor(PURPLE);
            _timerScatter = 0;
        }
        else if(GetAlive() && !IsScared()){
            _model.SetColor(_color);
        }
        
        int newGoalX,newGoalY;
        SetGoal(newGoalX, newGoalY, px,py, player_dirX, player_dirY);
        
        _velocity = _scared && _alive ? GHOST_SPEED_SCARED*0.8 : GHOST_SPEED_DEFAULT*0.8;
        _timerMovement += Global::GetFrameTime()*_velocity;
        
        if(abs(_x-Modulus(_x+_dirX, MAP_WIDTH)) != MAP_WIDTH-1){
            _model.Position.x = Lerp(_x, _x+_dirX, _timerMovement);
        }else{
            _model.Position.x = _x+_dirX;
            _timerMovement = 1;
        }

        if(abs(_y-Modulus(_y+_dirY, MAP_HEIGHT)) != MAP_HEIGHT-1){
            _model.Position.z = Lerp(_y, _y+_dirY, _timerMovement);
        }else{
            _model.Position.z = _y+_dirY;
            _timerMovement = 1;
        }

        if(_timerMovement >= 1){
            _timerMovement = 0;
            _x = Modulus(_x+_dirX, MAP_WIDTH);
            _y = Modulus(_y+_dirY, MAP_HEIGHT);
            _model.Position.x = _x;
            _model.Position.z = _y;
            
            int nodeIndex = FindPathNode(_x, _y);
            if(nodeIndex >=0 && nodeIndex < _path.size()-1){
                _dirX = _path[nodeIndex+1]->x - _path[nodeIndex]->x;
                _dirY = _path[nodeIndex+1]->y - _path[nodeIndex]->y;
            }
            else{
                _dirX = 0;
                _dirY = 0;
            }
                
            if(!_alive && _x == _goalX && _y == _goalY){
                _alive = true;
                _model.SetColor(_color);
                _model.model = ModelCache::LoadModel("ghost.obj");
            }
            
            if(newGoalX!=_lastGoalX || newGoalY!=_lastGoalY){
                _lastGoalX = newGoalX;
                _lastGoalY = newGoalY;
                GeneratePath(newGoalX,newGoalY);
                if(_path.size()>2){
                    _dirX = _path[0]->x - _x;
                    _dirY = _path[0]->y - _y;
                }
            }
        }
        float lookAngle = _dirX < 0 ? -90 :
                          _dirX > 0 ? 90 :
                          _dirY < 0 ? 180 :
                          _dirY > 0 ? 0 : _model.RotationAngle;
        
        _model.RotationAngle = Lerp(_model.RotationAngle, lookAngle, Global::GetFrameTime()*5);
    }
}

void Enemy::OnUpdate(){
    if(!_isOver){
        int player_x = Player::GetX();
        int player_y = Player::GetY();
        int player_dirX =  Player::GetDirX();
        int player_dirY =  Player::GetDirY();

        _timerScatter += Global::GetFrameTime();
        if(_timerScatter > SCATTER_TIME) {
            _scatter = true;
        }
        if(_timerScatter <= 0){
            _scatter = false;
        }

        if(GetAlive() && IsScared()){
            _model.SetColor(PURPLE);
            _timerScatter = 0;
        }
        else if(GetAlive() && !IsScared()){
            _model.SetColor(_color);
        }
        
        int newGoalX,newGoalY;
        SetGoal(newGoalX, newGoalY, player_x,player_y, player_dirX, player_dirY);

        if(newGoalX >= 0 && newGoalX < MAP_WIDTH &&
        newGoalY >= 0 && newGoalY < MAP_HEIGHT &&
        MapData::GetElementoMap(newGoalX, newGoalY) != ElementType::Wall){
            _goalX = newGoalX;
            _goalY = newGoalY;
        }

        _velocity = _scared && _alive ? GHOST_SPEED_SCARED : GHOST_SPEED_DEFAULT*0.8f;
        _timerMovement += Global::GetFrameTime()*_velocity;
        
        if(abs(_x-Modulus(_x+_dirX, MAP_WIDTH)) != MAP_WIDTH-1){
            _model.Position.x = Lerp(_x, _x+_dirX, _timerMovement);
        }else{
            _model.Position.x = _x+_dirX;
            _timerMovement = 1;
        }

        if(abs(_y-Modulus(_y+_dirY, MAP_HEIGHT)) != MAP_HEIGHT-1){
            _model.Position.z = Lerp(_y, _y+_dirY, _timerMovement);
        }else{
            _model.Position.z = _y+_dirY;
            _timerMovement = 1;
        }

        if(_timerMovement >= 1){
            _timerMovement = 0;
            _x = Modulus(_x+_dirX, MAP_WIDTH);
            _y = Modulus(_y+_dirY, MAP_HEIGHT);
            _model.Position.x = _x;
            _model.Position.z = _y;
            MapData::GetDirection(_x, _y, _goalX,_goalY, _dirX, _dirY);
            
            if(!_alive && _x == _goalX && _y == _goalY){
                _alive = true;
                _model.SetColor(_color);
                _model.model = ModelCache::LoadModel("ghost.obj");
            }
        }

        float lookAngle = _dirX < 0 ? -90 :
                          _dirX > 0 ? 90 :
                          _dirY < 0 ? 180 :
                          _dirY > 0 ? 0 : _model.RotationAngle;
        
        _model.RotationAngle = Lerp(_model.RotationAngle, lookAngle, Global::GetFrameTime()*5);
    }
}

void Enemy::OnRestart(){
    _model.Position = {(float)_spawnX, 0, (float)_spawnY};
    _model.SetColor(_color);
    _dirX = 0;
    _dirY = 0;
    _x = _spawnX;
    _y = _spawnY;
    _scatter = false;
    _scared = false;
    _alive = true;
}

bool Enemy::IsScared(){
    return _scared;
}

void Enemy::IsOver(bool state){
    _isOver = state;
}

Color Enemy::GetColor(){
    return _scared && _alive? (Color)PURPLE : (_alive? _color : (Color)WHITE);
}

float Enemy::GetX(){
    return _model.Position.x;
}

float Enemy::GetY(){
    return _model.Position.z;
}

bool Enemy::GetAlive(){
    return _alive;
}
