//
//  game_pacman.hpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/6.
//

#ifndef game_pacman_hpp
#define game_pacman_hpp

#include "glad.h"
#include <vector>

#include "scene.h"
#include <string>

class GamePacman:public IScene
{
public:
    
    // Initialize game state (load all shaders/textures/levels)
    void Init(unsigned int width, unsigned int height);
    void OnEnter();
    void OnExit();
    // GameLoop
    void KeyboardInput(int virtual_key, char pressed);
    void Update(GLfloat dt);
    void Render();
    void Release();
    
    void Restart();
    
    static GamePacman* GetInstance()
    {
        if(s_instance == nullptr)
        {
            s_instance = new GamePacman();
        }
        return s_instance;
    }
    
private:
    GLfloat deltaTime;
    int _dirY;
    int _dirX;
    float _x, _y;
    
    GamePacman(){}
    
    ~GamePacman(){}

    GamePacman(const GamePacman&);
    GamePacman& operator =(const GamePacman&);
    
    void UpdateView();
    bool VerificaPosition();
    
    void SetPacman();
    void SetCamera();
    void SetEnemies();
    
    static GamePacman *s_instance;
};

#endif /* game_pacman_hpp */
