//
//  scene_template.cpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/1/6.
//

#include "scene_template.hpp"
#include "resource_manager.hpp"
#include "sprite_renderer.hpp"
#include "text_renderer.hpp"
#include "director.hpp"
#include "button.hpp"
#include "global.hpp"
#include "camera.h"
#include "mesh_object.hpp"
#include "mesh.hpp"
#include "player.hpp"
#include "model_cache.hpp"
#include "utils.h"

SceneTemplate* SceneTemplate::s_instance = nullptr;

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float cubeVertices[] = {
    // positions          // texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
float planeVertices[] = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};
// cube VAO
unsigned int cubeVAO, cubeVBO;
// plane VAO
unsigned int planeVAO, planeVBO;
int cubeTexture;
int floorTexture;

Shader depthShader;

Camera camera_template(glm::vec3(0.0f, 8.0f, 10.0f),glm::vec3(0.0f, 1.0f, 0.0f), -90.0f,-30.0f);

void SceneTemplate::Init(unsigned int width, unsigned int height)
{
    this->Width = width;
    this->Height = height;
    char des[1024] = {0};
    
    // 加载着色器
    char vs[1024] = {0}, fs[1024] = {0};
    memset(vs, 0, sizeof(vs));memset(fs, 0, sizeof(fs));
    ResourceManager::LoadShader(Global::ResFullPath(vs,"depth.vs"), Global::ResFullPath(fs,"depth.fs"), nullptr, "depth");
    
    ResourceManager::LoadTexture(Global::ResFullPath(des, "marble.jpg"), GL_FALSE, "marble");
    ResourceManager::LoadTexture(Global::ResFullPath(des, "metal.png"), GL_TRUE, "metal");
    
    depthShader = ResourceManager::GetShader("depth");
    depthShader.use();
    depthShader.setInt("sprite", 0);
    Texture2D cubeTex = ResourceManager::GetTexture("marble");
    Texture2D floorTex = ResourceManager::GetTexture("metal");
    cubeTexture = cubeTex.ID;
    floorTexture = floorTex.ID;
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

void SceneTemplate::OnEnter(){
    
}

void SceneTemplate::OnExit(){
    
}

void SceneTemplate::KeyboardInput(int virtual_key, char pressed)
{
    
}

void SceneTemplate::Update(GLfloat dt)
{
    
}

void SceneTemplate::Render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    depthShader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera_template.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera_template.Zoom),
                                            (float)this->Width / (float)this->Height, 0.1f, 100.0f);
    depthShader.setMat4("view", view);
    depthShader.setMat4("projection", projection);
    
    // cubes
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model = glm::translate(model, glm::vec3(-1.0f, 2.0f, -1.0f));
    depthShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 2.0f, 0.0f));
    depthShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    depthShader.setMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
}

void SceneTemplate::Release()
{
    
}
