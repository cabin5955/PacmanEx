//
//  gameover_pacman.hpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/22.
//

#ifndef gameover_pacman_hpp
#define gameover_pacman_hpp

#include "glad.h"
#include <vector>

#include "scene.h"
#include <string>

class GameOverPacman:public IScene
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
    
    static GameOverPacman* GetInstance()
    {
        if(s_instance == nullptr)
        {
            s_instance = new GameOverPacman();
        }
        return s_instance;
    }
    GLchar                 preferPath[1024];
    
private:
    const GLchar *FullPath(const GLchar *fileName);
    GLchar  fullPath[1024];
    GLfloat deltaTime;
    
    GameOverPacman(){}
    
    ~GameOverPacman(){}

    GameOverPacman(const GameOverPacman&);
    GameOverPacman& operator =(const GameOverPacman&);
    
    static GameOverPacman *s_instance;
};

#endif /* gameover_pacman_hpp */
