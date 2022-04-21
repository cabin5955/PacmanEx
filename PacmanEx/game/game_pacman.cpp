//
//  game_pacman.cpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/6.
//

#include "game_pacman.hpp"
#include "resource_manager.hpp"
#include "sprite_renderer.hpp"
#include "text_renderer.hpp"
#include "color_renderer.hpp"
#include "director.hpp"
#include "button.hpp"
#include "global.hpp"
#include "camera.h"
#include "mesh_object.hpp"
#include "mesh.hpp"
#include "objloader.hpp"
#include "player.hpp"
#include "model_cache.hpp"
#include "utils.h"
#include "enemy.hpp"
#include "derived_enemy.hpp"
#include "astar_map.hpp"
#include "gameover_pacman.hpp"

#define ROTATION_TIME 0.2f

GamePacman* GamePacman::s_instance = nullptr;
SpriteRenderer      *Renderer;
SpriteRenderer      *uiRenderer;
TextRenderer        *Text;
ColorRenderer       *colorRenderer;

//Camera camera3d(glm::vec3(0.0f, 8.0f, 10.0f),glm::vec3(0.0f, 1.0f, 0.0f), -90.0f,-30.0f);
// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 15.0f,  10.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 cameraOffset = {0,0,0};
glm::vec3 cameraTarget = {0,0,0};

float cameraZOOM      =  60.0f;

Shader meshShader;
Shader lightingShader;
Player *pacman;
ModelObject *ground;

Button *LeftButton;
Button *RightButton;
Button *UpButton;
Button *DownButton;
Button *PacmanJumpButton;

void LeftBtnOnPressed()
{
    pacman->RotationAngle = -90;
}

void RightBtnOnPressed()
{
    pacman->RotationAngle = 90;
}

void UpBtnOnPressed()
{
    pacman->RotationAngle = 180;
}

void DownBtnOnPressed()
{
    pacman->RotationAngle = 0;
}

void PacmanJumpBtnOnPressed()
{
    if (pacman->canJump)
    {
        pacman->speed = PLAYER_JUMP_SPD;
        pacman->Position.y += pacman->speed * Global::GetFrameTime();
        pacman->speed += -G * Global::GetFrameTime();
        pacman->canJump = false;
    }
    else
    {
        pacman->isWalk = false;
    }
}

void SetCameraOffset(glm::vec3 offset){
    cameraOffset = offset;
}

void CameraFollow(glm::vec3 position, float speed){
    glm::vec3 newPos =  {position.x + cameraOffset.x,
                                position.y + cameraOffset.y,
                                position.z + cameraOffset.z};
    cameraPos = Lerp(cameraPos, newPos, Global::GetFrameTime() * speed);
    cameraTarget = Lerp(cameraTarget, position, Global::GetFrameTime() * speed);
}

bool GamePacman::VerificaPosition(){

    unsigned int ix = roundf(_x + _dirX*0.5);
    unsigned int iy = roundf(_y + _dirY*0.5);
    std::vector<Enemy*> enemies = Enemy::GetEnemies();
    for(unsigned int i = 0; i < enemies.size(); ++i)
    {
        float enemy_x = enemies[i]->GetX();
        float enemy_y = enemies[i]->GetY();

        if(abs(enemy_x - _x)<0.5 && abs(enemy_y - _y)<0.5){
            if(enemies[i]->IsScared()){
                enemies[i]->Die();
                //_pontuacao += 50;
            }else if(enemies[i]->GetAlive()){
                pacman->Die();
                Enemy::IsOver(true);
                Director::GetInstance()->PushScene(GameOverPacman::GetInstance());
                return 0;
            }
        }
    }
    
    if(AStarMap::GetElementoMap(ix,iy) == ElementType::Point) {
        //_pontuacao++;
        AStarMap::RemoveElementoMap(ix,iy);
    }
    
    if(AStarMap::GetElementoMap(ix,iy) == ElementType::Especial){
        for (unsigned int i = 0; i < enemies.size(); ++i){
            if(enemies[i]->GetAlive()){
                enemies[i]->GetScared(true);
            }
        }
        //_pontuacao+= VALOR_FRUTA;
        AStarMap::RemoveElementoMap(ix,iy);
    }
    
    if(AStarMap::GetElementoMap(ix,iy) == ElementType::Wall) {
        return true;
    }
    else{
        return false;
    }
}

void GamePacman::SetPacman(){
    glm::vec2 spawn = AStarMap::GetPlayerSpawn();
    pacman->Position = {spawn.x, 0, spawn.y};
    pacman->isAlive = true;
    pacman->idle_ani.SetTimer(0);
    pacman->model = ModelCache::LoadModel(pacman->idle_ani.GetCurrentFrame());
    _x = pacman->Position.x;
    _y = pacman->Position.z;
    _dirY = 0;
    _dirX = 0;
    Player::_xToEnemy = round(_x);
    Player::_yToEnemy = round(_y);
    Player::_timerScared = 0.0;
    Player::_scaredTime = 6;
}

void GamePacman::SetCamera(){
    SetCameraOffset({0.0f, 18.5f, 7.2f});
    //SetCameraOffset({0.0f, 1.0f, 0.0f});
    glm::vec3 newPos =  {
        pacman->Position.x + cameraOffset.x,
        pacman->Position.y + cameraOffset.y,
        pacman->Position.z + cameraOffset.z
    };
    cameraPos = newPos;
    cameraTarget = pacman->Position;
    UpdateView();
}

void GamePacman::SetEnemies(){
    glm::vec2 enemyPos;
    Enemy::RemoveEnemies();
    if(AStarMap::GetEnemySpawn(0, enemyPos))
        Enemy::AddEnemy(new Blinky(enemyPos.x,enemyPos.y));
    
    if(AStarMap::GetEnemySpawn(1, enemyPos))
        Enemy::AddEnemy(new Pinky(enemyPos.x,enemyPos.y));
    
    Enemy::IsOver(false);
}

void GamePacman::Restart(){
    char des[1024] = {0};
    AStarMap::LoadFile(Global::ResFullPath(des,"mapa1.txt"));
    AStarMap::GetMapGlobal().OnRestart();
    SetPacman();
    SetCamera();
    SetEnemies();
}

void GamePacman::Init(unsigned int width, unsigned int height)
{
    this->Width = width;
    this->Height = height;
    
    char des[1024] = {0};
    
    // 配置着色器
    glm::mat4 projection2d = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
        static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    
    Shader spriteShader = ResourceManager::GetShader("sprite");
    spriteShader.use();
    spriteShader.setInt("image", 0);
    spriteShader.setMat4("projection", projection2d);
    
    Shader textShader = ResourceManager::GetShader("text");
    Text = new TextRenderer(textShader, this->Width, this->Height);
    Text->Load(Global::ResFullPath(des,"default.ttf"),48);
    
    Shader colorShader = ResourceManager::GetShader("color");
    colorShader.use();
    colorShader.setMat4("view", glm::mat4(1.0));
    colorShader.setMat4("projection", projection2d);
    
    Shader uiShader = ResourceManager::GetShader("ui");
    uiShader.use();
    uiShader.setMat4("projection", projection2d);
    
    // 设置专用于渲染的控制
    Renderer = new SpriteRenderer(spriteShader);
    colorRenderer = new ColorRenderer(colorShader);
    uiRenderer = new SpriteRenderer(uiShader);
    
    float scal = 1.0f;
    int btn_size = 64;
    int x_offset = btn_size/2;
    LeftButton = new Button(this,glm::vec2(this->Width/2-btn_size*scal-x_offset,this->Height-192*scal),
                              glm::vec2(btn_size*scal,btn_size*scal),
                              180.0f*DEG2RAD,
                              ResourceManager::GetTexture("arrow"),
                              LeftBtnOnPressed,0);
    
    RightButton = new Button(this,glm::vec2(this->Width/2+btn_size*scal-x_offset,this->Height-192*scal),
                              glm::vec2(btn_size*scal,btn_size*scal),
                              0,
                              ResourceManager::GetTexture("arrow"),
                              RightBtnOnPressed,0);
    
    UpButton = new Button(this,glm::vec2(this->Width/2-x_offset,this->Height-256*scal),
                              glm::vec2(btn_size*scal,btn_size*scal),
                              -90.0f*DEG2RAD,
                              ResourceManager::GetTexture("arrow"),
                              UpBtnOnPressed,0);
    
    DownButton = new Button(this,glm::vec2(this->Width/2-x_offset,this->Height-128*scal),
                              glm::vec2(btn_size*scal,btn_size*scal),
                              90.0f*DEG2RAD,
                              ResourceManager::GetTexture("arrow"),
                              DownBtnOnPressed,0);
    
    meshShader = ResourceManager::GetShader("mesh");
    meshShader.use();
    //meshShader.setVec3("viewPos", camera3d.Position);
    meshShader.setVec3("viewPos", cameraPos);
    meshShader.setVec3("light.position", glm::vec3(0.0f,0.0f,3.0f));
    meshShader.setVec3("light.ambient",  0.8f, 0.8f, 0.8f);
    meshShader.setVec3("light.diffuse",  0.3f, 0.3f, 0.3f); // 将光照调暗了一些以搭配场景
    meshShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    meshShader.setFloat("material.shininess", 64.0f);
    meshShader.setInt("material.diffuse", 0);
    
    pacman = new Player(8, 0.0f, 20);
    ground = new ModelObject("cube_ogl.obj", WHITE);
    
    // don't forget to enable shader before setting uniforms
    //meshShader.setMat4("view", camera3d.GetViewMatrix());
    glm::mat4 view = glm::lookAt(cameraPos, pacman->Position, cameraUp);
    meshShader.setMat4("view", view);
    //glm::mat4 projection = glm::perspective(glm::radians(camera3d.Zoom), this->Width / this->Height, 0.1f, 100.0f);
    glm::mat4 projection = glm::perspective(glm::radians(cameraZOOM),
                                                 (float)this->Width / (float)this->Height, 0.1f, 100.0f);
    meshShader.setMat4("projection", projection);
    
    lightingShader = ResourceManager::GetShader("mulight");
    lightingShader.use();
    lightingShader.setVec3("viewPos", cameraPos);
    lightingShader.setMat4("view", view);
    lightingShader.setMat4("projection", projection);
    lightingShader.setVec3("dirLight.position", glm::vec3(0.0f,3.0f,0.0f));
    lightingShader.setVec3("dirLight.ambient",  0.4f, 0.4f, 0.4f);
    lightingShader.setVec3("dirLight.diffuse",  0.2f, 0.2f, 0.2f); // 将光照调暗了一些以搭配场景
    lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("material.shininess", 64.0f);
    lightingShader.setInt("material.diffuse", 0);
    
    Color red = PINK;
    glm::vec3 light0Color = {red.r/255.0f,red.g/255.0f,red.b/255.0f};
    glm::vec3 diffuseColor =light0Color;
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.5f);
    lightingShader.setVec3("pointLights[0].ambient", ambientColor);
    lightingShader.setVec3("pointLights[0].diffuse", diffuseColor);
    lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    lightingShader.setFloat("pointLights[0].linear", 0.22f);
    lightingShader.setFloat("pointLights[0].quadratic", 0.20f);
    
    Color pink = PINK;
    light0Color = {pink.r/255.0f,pink.g/255.0f,pink.b/255.0f};
    diffuseColor =light0Color;
    ambientColor = diffuseColor * glm::vec3(0.5f);
    lightingShader.setVec3("pointLights[1].ambient", ambientColor);
    lightingShader.setVec3("pointLights[1].diffuse", diffuseColor);
    lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    lightingShader.setFloat("pointLights[1].linear", 0.22f);
    lightingShader.setFloat("pointLights[1].quadratic", 0.20f);
    
    AStarMap::LoadFile(Global::ResFullPath(des,"mapa1.txt"));
}

void GamePacman::OnEnter(){
    SetPacman();
    SetEnemies();
    SetCamera();
}

void GamePacman::OnExit(){
    
}

void GamePacman::KeyboardInput(int virtual_key, char pressed)
{
    
}

void GamePacman::UpdateView()
{
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    meshShader.use();
    meshShader.setMat4("view", view);
    
    lightingShader.use();
    lightingShader.setMat4("view", view);
    std::vector<Enemy*> enemies = Enemy::GetEnemies();
    lightingShader.setVec3("pointLights[0].position", {enemies[0]->GetX(),0.5f,enemies[0]->GetY()});
    lightingShader.setVec3("pointLights[1].position", {enemies[1]->GetX(),0.5f,enemies[1]->GetY()});
}

void GamePacman::Update(GLfloat dt)
{
    if (LeftButton->mouseState==MOUSE_PRESSED && pacman->IsAlive())
    {
        int mx = ceilf(_x-1);
        int my = roundf(_y);
        if(AStarMap::GetElementoMap(mx,my) != ElementType::Wall)
        {
            pacman->isWalk = true;
            _y = roundf(_y);
            _dirY = 0;
            _dirX = -1;
        }
    }
    else if (RightButton->mouseState==MOUSE_PRESSED && pacman->IsAlive())
    {
        int mx = floorf(_x+1);
        int my = roundf(_y);
        if(AStarMap::GetElementoMap(mx,my) != ElementType::Wall)
        {
            pacman->isWalk = true;
            _y = roundf(_y);
            _dirY = 0;
            _dirX = 1;
        }
    }
    else if (UpButton->mouseState==MOUSE_PRESSED && pacman->IsAlive())
    {
        int mx = roundf(_x);
        int my = ceilf(_y-1);
        if(AStarMap::GetElementoMap(mx,my) != ElementType::Wall)
        {
            pacman->isWalk = true;
            _x = roundf(_x);
            _dirY = -1;
            _dirX = 0;
        }
    }
    else if (DownButton->mouseState==MOUSE_PRESSED && pacman->IsAlive())
    {
        int mx = roundf(_x);
        int my = floorf(_y+1);
        if(AStarMap::GetElementoMap(mx,my) != ElementType::Wall)
        {
            pacman->isWalk = true;
            _x = roundf(_x);
            _dirY = 1;
            _dirX = 0;
        }
    }
    else
    {
        pacman->isWalk = false;
    }
    
    if(pacman->IsAlive()){
        bool colide = VerificaPosition();
        if(pacman->isWalk)
        {
            if(!colide && (_dirX != 0 || _dirY != 0))
            {
                _x = fModulus(_x + _dirX*3.5*Global::GetFrameTime(), MAP_WIDTH);
                _y = fModulus(_y + _dirY*3.5*Global::GetFrameTime(), MAP_HEIGHT);
                pacman->Position.x = _x;
                pacman->Position.z = _y;
            }
        }
    }
    
    Player::_xToEnemy = round(_x);
    Player::_yToEnemy = round(_y);
    Player::_dirxToEnemy = _dirX;
    Player::_diryToEnemy = _dirY;
    
    pacman->Update();
    CameraFollow(pacman->Position, 1);
    UpdateView();
    
    if(Enemy::IsScared()){
         if(Player::_timerScared < Player::_scaredTime){
             Player::_timerScared += Global::GetFrameTime();
        }
        else{
            Player::_timerScared = 0;
            Enemy::GetScared(false);
        }
    }
    Enemy::UpdateEnemies();
    AStarMap::GetMapGlobal().OnUpdate();
}

void GamePacman::Render()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    MeshRenderer::DrawMesh(meshShader,
                           *ground->model,
                           ResourceManager::GetTexture("mask"),
                           {0,-0.1,0},
                           glm::vec3(100.0f,0.1f,100.0f),
                           pacman->RotationAngle*DEG2RAD,
                           glm::vec3(0, 1.0f, 0),
                           DARKGRAY);
    
    AStarMap::GetMapGlobal().DrawMap(meshShader);
    
    MeshRenderer::DrawMesh(meshShader,
                           *pacman->model,
                           ResourceManager::GetTexture("mask"),
                           pacman->Position,
                           glm::vec3(1.0f,1.0f,1.0f),
                           pacman->RotationAngle*DEG2RAD,
                           glm::vec3(0, 1.0f, 0));
    
    Enemy::DrawEnemies(meshShader);

    LeftButton->Draw(*uiRenderer);
    RightButton->Draw(*uiRenderer);
    UpButton->Draw(*uiRenderer);
    DownButton->Draw(*uiRenderer);
}

void GamePacman::Release()
{
    
}
