//
//  astar_map.hpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/19.
//

#ifndef astar_map_hpp
#define astar_map_hpp

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
#include "map_data.hpp"

class NodeItem{
public:
    bool isWall;
    glm::vec3 pos;
    
    // node index
     int x, y;

    // length from the starting point
     int gCost;
    
    // length to target point
     int hCost;

    // total paths length
     int fCost() {
         return gCost + hCost;
    }
    
    NodeItem *parent;
    
    NodeItem()
    {
        this->isWall = false;
        this->pos = {0,0,0};
        this->x = 0;
        this->y = 0;
    }
};

class AStarMap {
    private:
    ElementType _map[MAP_WIDTH][MAP_HEIGHT];
    ElementType _original[MAP_WIDTH][MAP_HEIGHT];
    NodeItem ***_grid;

    int _pointsRestant;
    
    glm::vec2 _playerSpawn;
    std::map<unsigned, glm::vec2> _enemySpawn;
    
    void RegistraMapRenderizable();
    void DeregistrationMapRenderable();
    
    NodeItem* GetItem(glm::vec3 position);
    std::vector<NodeItem*> GetNeibourhood(NodeItem *node);
    std::vector<NodeItem*> GeneratePath(NodeItem *startNode, NodeItem *endNode);
    int GetDistanceNodes(NodeItem *a, NodeItem *b);
    
    public:
    std::vector<ModelObject*> _objetosMap;
    AStarMap();
    ~AStarMap();
    
    static ElementType GetElementoMap(unsigned int x, unsigned int y);
    static void RemoveElementoMap(unsigned int x, unsigned int y);
    void DrawMap(Shader &meshShader);
    std::vector<NodeItem*> FindingPath(glm::vec3 start, glm::vec3 end);

    static void LoadFile(std::string arq);
    static void Reload();
    static AStarMap& GetMapGlobal();
    
    static glm::vec2 GetPlayerSpawn();
    static bool GetEnemySpawn(unsigned id, glm::vec2& pos);

    void OnUpdate() ;
    void OnMenuUpdate() ;
    void OnRestart() ;

};

#endif /* astar_map_hpp */
