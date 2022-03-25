//
//  gameover_pacman.cpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/22.
//

#include "gameover_pacman.hpp"
#include "resource_manager.hpp"
#include "sprite_renderer.hpp"
#include "text_renderer.hpp"
#include "director.hpp"
#include "button.hpp"
#include "global.hpp"
#include "game_pacman.hpp"

GameOverPacman* GameOverPacman::s_instance = nullptr;
extern SpriteRenderer      *Renderer;
extern TextRenderer        *Text;

Button              *pacmanReplayButton;
GLfloat textWidth = 0.0f;
GLfloat showTimer = 0.0f;

void GameOver_ReplayBtnOnClick() {
    Director::GetInstance()->PopScene();
    GamePacman::GetInstance()->Restart();
    printf("click btn replay \n");
}

void GameOverPacman::Init(unsigned int width, unsigned int height)
{
    this->Width = width;
    this->Height = height;
    
    float scal = 2.0f;
    
    pacmanReplayButton = new Button(this,glm::vec2(this->Width/2-32*scal,this->Height/2),
                                      glm::vec2(64*scal,64*scal),
                                      ResourceManager::GetTexture("ui_btn_replay"),
                                      GameOver_ReplayBtnOnClick);
}

void GameOverPacman::OnEnter(){
    textWidth = Text->GetTextWidth("Game Over", 2.0f);
}

void GameOverPacman::OnExit(){
    
}

void GameOverPacman::KeyboardInput(int virtual_key, char pressed)
{
    
}

void GameOverPacman::Update(GLfloat dt)
{
    if(showTimer < 0.5f)showTimer+=Global::GetFrameTime();
}

void GameOverPacman::Render()
{
    if(showTimer>=0.5f)
    {
        glDisable(GL_DEPTH_TEST);
        Text->RenderText("Game Over", (this->Width-textWidth)/2, this->Height / 2.0f-150, 2.0f);
        pacmanReplayButton->Draw(*Renderer);
    }
}

void GameOverPacman::Release()
{
    
}
