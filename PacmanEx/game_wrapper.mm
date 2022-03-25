//
//  game_wrapper.m
//  BreakOutES
//
//  Created by chen caibin on 2021/10/29.
//

#import <Foundation/Foundation.h>
#include "game_wrapper.h"
#include <string.h>
#include "director.hpp"
#include "touch_dispatcher.hpp"
#include "color_renderer.hpp"
#include "game_pacman.hpp"
#include "global.hpp"
#include "gameover_pacman.hpp"

@interface GameWrapper (){
    NSString *preferPath;
}
@end

extern ColorRenderer       *colorRenderer;

@implementation GameWrapper

- (void)InitWidth:(GLfloat) width Height:(GLfloat) height{
    
    preferPath = [NSString stringWithFormat:@"%@/",[[NSBundle mainBundle] bundlePath]];
    const char* fileName = [preferPath cStringUsingEncoding:1];
    
    memset(Global::RES_PATH, 0, sizeof(Global::RES_PATH));
    strcpy(Global::RES_PATH,fileName);
    
    GamePacman::GetInstance()->Init(width, height);
    GameOverPacman::GetInstance()->Init(width, height);
    
    Director::GetInstance()->SetRootScene(GamePacman::GetInstance());
}

- (void)ProcessInput:(GLfloat) dt{
    
}

- (void)KeyboardInputWhithKey:(int) key Pressed:(char)pressed{
    Director::GetInstance()->GetTopScene()->KeyboardInput(key,pressed);
}

- (void)TouchBeganPosX:(double)x PosY:(double)y{
    TouchDispatcher *dispatcher = TouchDispatcher::get_instance();
    dispatcher->touchesBegan(x, y);
}

- (void)TouchEndedPosX:(double)x PosY:(double)y{
    TouchDispatcher *dispatcher = TouchDispatcher::get_instance();
    dispatcher->touchesEnded(x, y);
}

- (void)TouchMoveOffsetX:(double)x OffsetY:(double)y
{
    TouchDispatcher *dispatcher = TouchDispatcher::get_instance();
    dispatcher->touchesMovedOffset(x, y);
}

- (void)TouchMovedPosX:(double)x PosY:(double)y{
    TouchDispatcher *dispatcher = TouchDispatcher::get_instance();
    dispatcher->touchesMoved(x, y);
}

- (void)Update:(GLfloat)dt{
    Global::UpdateFrameTime();
    std::vector<IScene*> scenes = Director::GetInstance()->GetAllScenes();
    for(int i = 0; i < scenes.size();i++)
    {
        scenes[i]->Update(dt);
    }
}

- (void)Render{
    std::vector<IScene*> scenes = Director::GetInstance()->GetAllScenes();
    for(int i = 0; i < scenes.size();i++)
    {
        if(i>0){
            colorRenderer->DrawColor({0,0,0,192}, glm::vec2(0, 0), glm::vec2(1536, 2048));
        }
        scenes[i]->Render();
    }
}

- (void)Realease
{
    std::vector<IScene*> scenes = Director::GetInstance()->GetAllScenes();
    for(int i = 0; i < scenes.size();i++)
    {
        scenes[i]->Release();
    }
}

@end
