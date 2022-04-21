//
//  astar_map.cpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/19.
//

#include "astar_map.hpp"

#include "utils.h"
#include "resource_manager.hpp"
#include "global.hpp"
#include "enemy.hpp"

std::string modeloElemento_astar_astar[] = {"","cube.obj","point.obj","special.obj",};

AStarMap& AStarMap::GetMapGlobal(){
    static AStarMap mapGlobal;
    return mapGlobal;
}

AStarMap::AStarMap(): _pointsRestant(0){
    _grid = new NodeItem**[MAP_WIDTH];
    for(int i = 0; i < MAP_WIDTH; ++i){
        _grid[i] = new NodeItem*[MAP_HEIGHT];

        for(int j = 0; j < MAP_HEIGHT; ++j){
            _grid[i][j] = new NodeItem();
        }
    }
}

AStarMap::~AStarMap(){}

void AStarMap::LoadFile(std::string arq){
    AStarMap &map = GetMapGlobal();

    std::ifstream arquivo(arq);

    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            int elemento;
            arquivo >> elemento;
            if(!arquivo.fail()){
                if(elemento <= MAX_INDIX_ELEMENT_MAP){
                    map._map[x][y] = (ElementType) elemento;
                    map._original[x][y] = (ElementType) elemento;

                    if(elemento == Point || elemento == Especial){
                        map._pointsRestant++;
                    }
                }
                else{
                    if(elemento == INDIX_PACMAN){
                        map._playerSpawn = {(float)x, (float)y};
                    }
                    else if(elemento >= INDIX_FANTASMAS){
                        map._enemySpawn[elemento - INDIX_FANTASMAS] = {(float)x, (float)y};
                    }
                }
                map._grid[x][y]->isWall = (elemento == ElementType::Wall)?true:false;
                map._grid[x][y]->x = x;
                map._grid[x][y]->y = y;
                map._grid[x][y]->pos = {x,0,y};
            }
            else{
                throw std::invalid_argument("Mapa " + arq +" contem valor invalido!");
            }
        }
    }
    map.DeregistrationMapRenderable();
    map.RegistraMapRenderizable();
}

// 根据坐标获得一个节点
NodeItem *AStarMap::GetItem(glm::vec3 position) {
    int x = round(position.x);
    int y = round(position.z);
    x = Clamp (x, 0, MAP_WIDTH - 1);
    y = Clamp (y, 0, MAP_HEIGHT - 1);
    return _grid [x][y];
}

// 取得周围的节点
std::vector<NodeItem*> AStarMap::GetNeibourhood(NodeItem *node) {
    std::vector<NodeItem*> list;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            // 如果是自己，则跳过
            if(i == 0 && j == 0)
                continue;
            if(i==-1 && j==-1)
                continue;
            if(i==1 && j==-1)
                continue;
            if(i==-1 && j==1)
                continue;
            if(i==1 && j==1)
                continue;
            int x = node->x + i;
            int y = node->y + j;
            // 判断是否越界，如果没有，加到列表中
            if (x < MAP_WIDTH && x >= 0 && y < MAP_HEIGHT && y >= 0)
                list.push_back(_grid[x][y]);
        }
    }
    return list;
}

// A*寻路
std::vector<NodeItem*> AStarMap::FindingPath(glm::vec3 startPos, glm::vec3 endPos) {
    NodeItem *startNode = GetItem (startPos);
    NodeItem *endNode = GetItem (endPos);

    std::vector<NodeItem*> openSet;
    std::set<NodeItem*> closeSet;
    openSet.push_back (startNode);

    while (openSet.size() > 0)
    {
        NodeItem *curNode = openSet[0];
        for (int i = 0; i < openSet.size(); i++){
            if (openSet[i]->fCost() <= curNode->fCost() &&
                openSet[i]->hCost < curNode->hCost)
            {
                curNode = openSet[i];
            }
        }
        std::vector<NodeItem*>::iterator it;
        for(it=openSet.begin();it!=openSet.end();it++){
            if(*it == curNode)
            {
                openSet.erase(it);
                break;
            }
        }
        closeSet.insert(curNode);
        //printf("insert close set %d,%d,size %d\n",curNode->x,curNode->y,(int)closeSet.size());

        // 找到的目标节点
        if (curNode == endNode) {
            return GeneratePath (startNode, endNode);
        }

        std::vector<NodeItem*> list =  GetNeibourhood(curNode);
        // 判断周围节点，选择一个最优的节点
        for(int i = 0;i < list.size();i++) {
            NodeItem *item = list[i];
            // 如果是墙或者已经在关闭列表中
            if (item->isWall || closeSet.find(item) != closeSet.end() )
                continue;
            // 计算当前相领节点现开始节点距离
            int addDist = GetDistanceNodes (curNode, item);
            int newCost = curNode->gCost + addDist;
            // 如果距离更小，或者原来不在开始列表中
            bool isFindInOpenSet = false;
            for (int i = 0; i < openSet.size(); i++){
                if(item == openSet[i]){
                    isFindInOpenSet = true;
                    break;
                }
            }
            if (newCost < item->gCost || !isFindInOpenSet) {
                // 更新与开始节点的距离
                item->gCost = newCost;
                // 更新与终点的距离
                item->hCost = GetDistanceNodes (item, endNode);
                // 更新父节点为当前选定的节点
                item->parent = curNode;
                // 如果节点是新加入的，将它加入打开列表中
                if (!isFindInOpenSet) {
                    openSet.push_back(item);
                }
                //printf("item %d,%d gcost %d,hcost %d,is find in open set %d\n",item->x,item->y,item->gCost,item->hCost,isFindInOpenSet);
            }
        }
    }
    return GeneratePath (startNode, NULL);
}

// 生成路径
std::vector<NodeItem*> AStarMap::GeneratePath(NodeItem *startNode, NodeItem *endNode) {
    std::vector<NodeItem*> path;
    if (endNode != NULL) {
        NodeItem *temp = endNode;
        while (temp != startNode) {
            path.push_back(temp);
            temp = temp->parent;
        }
        // 反转路径
        std::reverse(path.begin(), path.end());
    }    
    // 更新路径
    //grid.updatePath(path);
    return path;
}

// 获取两个节点之间的距离
int AStarMap::GetDistanceNodes(NodeItem *a, NodeItem *b) {
    int cntX = abs (a->x - b->x);
    int cntY = abs (a->y - b->y);
    // 判断到底是那个轴相差的距离更远
    if (cntX > cntY) {
        return 14 * cntY + 10 * (cntX - cntY);
    } else {
        return 14 * cntX + 10 * (cntY - cntX);
    }
}

void AStarMap::RegistraMapRenderizable(){
    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            int indiceElem = (int)(this->_map[x][y]);
            if(indiceElem < (int)(sizeof(modeloElemento_astar_astar)/sizeof(modeloElemento_astar_astar[0]))
               && modeloElemento_astar_astar[indiceElem] != "")
            {
                ModelObject* wall = new ModelObject(modeloElemento_astar_astar[indiceElem], BLUE);
                wall->Position.x = x;
                wall->Position.z = y;
                _objetosMap.push_back(wall);
            }
        }
    }
}

void AStarMap::DeregistrationMapRenderable(){
    for(GameObject* o : _objetosMap){
        delete o;
    }
    _objetosMap.clear();
}

ElementType AStarMap::GetElementoMap(unsigned int x, unsigned int y){
    if(x >= MAP_WIDTH || y >=MAP_HEIGHT) return ElementType::Empty;
    return GetMapGlobal()._map[x][y];
}

void AStarMap::RemoveElementoMap(unsigned int x, unsigned int y){
    if(x >= MAP_WIDTH || y >=MAP_HEIGHT) return ;

    AStarMap& map = GetMapGlobal();
    //if(map._map[x][y] == Point || map._map[x][y] == Especial)
    //    map._pontosRestantes--;

    map._map[x][y] = ElementType::Empty;
    for(unsigned int i=0; i < map._objetosMap.size(); i++){
        ModelObject* objMapa = map._objetosMap.at(i);
        if(objMapa->Position.x == x && objMapa->Position.z == y){
            delete objMapa;
            map._objetosMap.erase(map._objetosMap.begin()+i);
            break;
        }
    }
}


void AStarMap::DrawMap(Shader &meshShader){
    for(unsigned int i=0; i<_objetosMap.size(); i++){
        ModelObject* objMap = _objetosMap.at(i);
        MeshRenderer::DrawMesh(meshShader, *objMap->model,
                               ResourceManager::GetTexture("mask"),
                               objMap->Position,
                               objMap->Size,
                               objMap->RotationAngle*DEG2RAD,
                               objMap->RotationAxis,
                               objMap->color);
    }
}

void AStarMap::Reload(){
    AStarMap &map = GetMapGlobal();
    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            ElementType elem = map._original[x][y];
            map._map[x][y] = elem;

            if(elem == Point || elem == Especial){
                map._pointsRestant++;
                ModelObject* point = new ModelObject(modeloElemento_astar_astar[elem], BLUE);
                point->Position.x = x;
                point->Position.z = y;
                map._objetosMap.push_back(point);
            }
        }
    }
}

glm::vec2 AStarMap::GetPlayerSpawn(){
    return GetMapGlobal()._playerSpawn;
}

bool AStarMap::GetEnemySpawn(unsigned id, glm::vec2& pos){
    auto it = GetMapGlobal()._enemySpawn.find(id);
    if(it != GetMapGlobal()._enemySpawn.end()){
        pos = it->second;
        return true;
    }else{
        return false;
    }
}

double tempoInterp_astar = 1;
void AStarMap::OnUpdate()
{
    tempoInterp_astar += Global::GetFrameTime()*0.0125;
    return;
    for(unsigned int i=0; i<_objetosMap.size(); i++)
    {
        ModelObject* objMap = _objetosMap.at(i);

        if(AStarMap::GetElementoMap(objMap->Position.x, objMap->Position.z) == Wall){
            float val = 0;
            int lightR = 0;
            int lightG = 0;
            int lightB = 0;

            std::vector<Enemy*>& enemies = Enemy::GetEnemies();
            for(Enemy* e : enemies){
                glm::vec3 pos = {(float)e->GetX(), 0, (float)e->GetY()};
                float distVal = Smoothstep(0.4,0.8,1.0 - Distance(objMap->Position, pos)/4.0);

                Color corEnemy = e->GetColor();
                lightR += corEnemy.r*distVal;
                lightG += corEnemy.g*distVal;
                lightB += corEnemy.b*distVal;
                val += distVal;
            }

            val = Clamp(val, 0, 1);
            lightR = Clamp(lightR, 0, 255);
            lightG = Clamp(lightG, 0, 255);
            lightB = Clamp(lightB, 0, 255);
            Color light = (Color){(unsigned char)lightR, (unsigned char)lightG, (unsigned char)lightB, 255};

            Color tempColor = Lerp(BLUE, light, val);
            Color finalColor = Lerp(objMap->color, tempColor, tempoInterp_astar);
            objMap->SetColor(finalColor);
        }
        else{
            objMap->SetColor(Lerp(objMap->color, { 255, 211, 0, 255 }, tempoInterp_astar));
        }
    }
}

void AStarMap::OnMenuUpdate(){
    tempoInterp_astar += Global::GetFrameTime()*0.0125;
    for(unsigned int i=0; i<_objetosMap.size(); i++){
        ModelObject* objMap = _objetosMap.at(i);
        Color col = {(unsigned char)((1+sin(objMap->Position.x/8 + Global::GetTime()*1))*127),
                    (unsigned char)((1+sin(objMap->Position.z/8 + Global::GetTime()*2))*127),
                    (unsigned char)((1+cos(objMap->Position.x/8 + objMap->Position.z/8 +Global::GetTime()*3))*127)};
        
        objMap->SetColor(Lerp(objMap->color, col, tempoInterp_astar));
    }
}

void AStarMap::OnRestart(){
    tempoInterp_astar = 0;
}
