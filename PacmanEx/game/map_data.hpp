//
//  map_data.hpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/2/22.
//

#ifndef map_data_hpp
#define map_data_hpp

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "glad.h"
#include <glm/glm.hpp>
#include "model_object.hpp"

#define MAP_HEIGHT 27
#define MAP_WIDTH 21

#define MAX_INDIX_ELEMENT_MAP ((int)ElementType::Fruit)
#define INDIX_PACMAN 5
#define INDIX_FANTASMAS 6

enum ElementType{Empty, Wall, Point, Especial, Fruit};

class MapData {
    private:
    ElementType _mapa[MAP_WIDTH][MAP_HEIGHT];
    ElementType _original[MAP_WIDTH][MAP_HEIGHT];

    int _pointsRestant;
    
    glm::vec2 _playerSpawn;
    std::map<unsigned, glm::vec2> _enemySpawn;

    int _nexto[MAP_WIDTH*MAP_HEIGHT][MAP_WIDTH*MAP_HEIGHT];
    
    void RegistraMapRenderizable();
    void DeregistrationMapRenderable();
    void GeneratePath();
    
    public:
    std::vector<ModelObject*> _objetosMap;
    MapData();
    ~MapData();
    
    static ElementType GetElementoMap(unsigned int x, unsigned int y);
    void DrawMap(Shader &meshShader);
    void FindingPath(glm::vec3 start, glm::vec3 end);

    static void LoadFile(std::string arq);
    static void Reload();
    static MapData& GetMapGlobal();
    
    static glm::vec2 GetPlayerSpawn();
    static bool GetEnemySpawn(unsigned id, glm::vec2& pos);

    void OnUpdate() ;
    void OnMenuUpdate() ;
    void OnRestart() ;

    static void GetDirection(unsigned int startX, unsigned int startY, unsigned int goalX, unsigned int goalY, int &stepX, int &stepY);

};

#endif /* map_data_hpp */
