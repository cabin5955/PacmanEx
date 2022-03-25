//
//  map_data.cpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/2/22.
//

#include "map_data.hpp"
#include "utils.h"
#include "resource_manager.hpp"
#include "global.hpp"
#include "enemy.hpp"

std::string modeloElemento[] = {"","cube.obj","point.obj","special.obj",};

MapData& MapData::GetMapGlobal(){
    static MapData mapaGlobal;
    return mapaGlobal;
}

MapData::MapData(): _pointsRestant(0){}

MapData::~MapData(){}

void MapData::LoadFile(std::string arq){
    MapData &map = GetMapGlobal();

    std::ifstream arquivo(arq);

    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            int elemento;
            arquivo >> elemento;
            if(!arquivo.fail()){
                if(elemento <= MAX_INDIX_ELEMENT_MAP){
                    map._mapa[x][y] = (ElementType) elemento;
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
            }
            else{
                throw std::invalid_argument("Mapa " + arq +" contem valor invalido!");
            }
        }
    }
    map.RegistraMapRenderizable();
    map.GeneratePath();
}

void MapData::RegistraMapRenderizable(){
    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            int indiceElem = (int)(this->_mapa[x][y]);
            if(indiceElem < (int)(sizeof(modeloElemento)/sizeof(modeloElemento[0]))
               && modeloElemento[indiceElem] != "")
            {
                ModelObject* wall = new ModelObject(modeloElemento[indiceElem], BLUE);
                wall->Position.x = x;
                wall->Position.z = y;
                _objetosMap.push_back(wall);
            }
        }
    }
}

void MapData::DeregistrationMapRenderable(){
    for(GameObject* o : _objetosMap){
        delete o;
    }
    _objetosMap.clear();
}

ElementType MapData::GetElementoMap(unsigned int x, unsigned int y){
    if(x >= MAP_WIDTH || y >=MAP_HEIGHT) return ElementType::Empty;
    return GetMapGlobal()._mapa[x][y];
}

void MapData::DrawMap(Shader &meshShader){
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

void MapData::Reload(){
    MapData &mapa = GetMapGlobal();
    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            ElementType elem = mapa._original[x][y];
            mapa._mapa[x][y] = elem;

            if(elem == Point || elem == Especial){
                mapa._pointsRestant++;
                ModelObject* point = new ModelObject(modeloElemento[elem], BLUE);
                point->Position.x = x;
                point->Position.z = y;
                mapa._objetosMap.push_back(point);
            }
        }
    }
}

void MapData::GeneratePath(){
    //Creating the path generator matrix
    int** paths = new int*[MAP_WIDTH*MAP_HEIGHT];
    for(int i = 0; i < MAP_WIDTH*MAP_HEIGHT; i++){
        paths[i] = new int[MAP_WIDTH*MAP_HEIGHT];
    }
    //Inicializando
    for(int i = 0; i < MAP_WIDTH*MAP_HEIGHT; i++){
        for(int j = 0; j < MAP_WIDTH*MAP_HEIGHT; j++){
            paths[i][j] = INT_MAX/2;
            _nexto[i][j] = -1;
        }
    }

    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            int i = x+(y*MAP_WIDTH);
            paths[i][i] = 0;
            _nexto[i][i] = i;

            //Sets the distance between neighbors and the current one to 1
            if(_mapa[x][y] != Wall){
                if(x+1 < MAP_WIDTH && _mapa[x+1][y]!=Wall){
                    int iv = (x+1)+(y*MAP_WIDTH);
                    paths[i][iv] = 1;
                    paths[iv][i] = 1;
                    _nexto[i][iv] = iv;
                    _nexto[iv][i] = i;
                }else if(x == MAP_WIDTH){
                    int iv = 0+(y*MAP_WIDTH);
                    paths[i][iv] = 1;
                    paths[iv][i] = 1;
                    _nexto[i][iv] = iv;
                    _nexto[iv][i] = i;
                }

                if(x-1 >= 0 && _mapa[x-1][y]!=Wall){
                    int iv = (x-1)+(y*MAP_WIDTH);
                    paths[i][iv] = 1;
                    paths[iv][i] = 1;
                    _nexto[i][iv] = iv;
                    _nexto[iv][i] = i;
                }else if(x == 0){
                    int iv = (MAP_WIDTH-1)+(y*MAP_WIDTH);
                    paths[i][iv] = 1;
                    paths[iv][i] = 1;
                    _nexto[i][iv] = iv;
                    _nexto[iv][i] = i;
                }

                if(y+1 < MAP_HEIGHT && _mapa[x][y+1]!=Wall){
                    int iv = x +((y+1)*MAP_WIDTH);
                    paths[i][iv] = 1;
                    paths[iv][i] = 1;
                    _nexto[i][iv] = iv;
                    _nexto[iv][i] = i;
                }else if(y == MAP_HEIGHT){
                    int iv = x+(0*MAP_WIDTH);
                    paths[i][iv] = 1;
                    paths[iv][i] = 1;
                    _nexto[i][iv] = iv;
                    _nexto[iv][i] = i;
                }

                if(y-1 >= 0 && _mapa[x][y-1]!=Wall){
                    int iv = x +((y-1)*MAP_WIDTH);
                    paths[i][iv] = 1;
                    paths[iv][i] = 1;
                    _nexto[i][iv] = iv;
                    _nexto[iv][i] = i;
                }else if(y == 0){
                    int iv = x+((MAP_HEIGHT-1)*MAP_WIDTH);
                    paths[i][iv] = 1;
                    paths[iv][i] = 1;
                    _nexto[i][iv] = iv;
                    _nexto[iv][i] = i;
                }
            }
        }
    }

    for(int k = 0; k < MAP_WIDTH*MAP_HEIGHT; k++){
        for(int i = 0; i < MAP_WIDTH*MAP_HEIGHT; i++){
            for(int j = 0; j < MAP_WIDTH*MAP_HEIGHT; j++){
                if(paths[i][j] > paths[i][k] + paths[k][j]){
                    paths[i][j] = paths[i][k] + paths[k][j];
                    _nexto[i][j] = _nexto[i][k];
                }
            }
        }
    }

    for(int i = 0; i < MAP_WIDTH*MAP_HEIGHT; i++){
        free(paths[i]);
    }
    free(paths);
}

void MapData::GetDirection(unsigned int startX, unsigned int startY,
                           unsigned int goalX, unsigned int goalY,
                           int &stepX, int &stepY)
{
    if(startX >= MAP_WIDTH || startY >= MAP_HEIGHT
       || goalX >= MAP_WIDTH || goalY >=MAP_HEIGHT)
    {
        stepX = 0;
        stepY = 0;
        return;
    }

    int u = startX + (startY*MAP_WIDTH);
    int v = goalX + (goalY*MAP_WIDTH);
    MapData &map = GetMapGlobal();
    if(map._nexto[u][v] >= 0 && (startX != goalX || startY != goalY)){
        stepX = map._nexto[u][v] % MAP_WIDTH;
        stepY = (map._nexto[u][v] - stepX)/MAP_WIDTH;
        printf("uv %d,%d,nexto[u][v] %d,stepx %d ,stepy %d\n",u,v,map._nexto[u][v],stepX,stepY);
        stepX -= startX;
        stepY -= startY;
        printf("startx %d,stepx-=startx %d,starty %d,stepy-=starty %d\n",startX,startY,stepX,stepY);
    }else{
        stepX = 0;
        stepY = 0;
    }
}

glm::vec2 MapData::GetPlayerSpawn(){
    return GetMapGlobal()._playerSpawn;
}

bool MapData::GetEnemySpawn(unsigned id, glm::vec2& pos){
    auto it = GetMapGlobal()._enemySpawn.find(id);
    if(it != GetMapGlobal()._enemySpawn.end()){
        pos = it->second;
        return true;
    }else{
        return false;
    }
}

double tempoInterp = 1;
void MapData::OnUpdate()
{
    tempoInterp += Global::GetFrameTime()*0.0125;
    for(unsigned int i=0; i<_objetosMap.size(); i++)
    {
        ModelObject* objMap = _objetosMap.at(i);

        if(MapData::GetElementoMap(objMap->Position.x, objMap->Position.z) == Wall){
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
            Color finalColor = Lerp(objMap->color, tempColor, tempoInterp);
            objMap->SetColor(finalColor);
        }
        else{
            objMap->SetColor(Lerp(objMap->color, { 255, 211, 0, 255 }, tempoInterp));
        }
    }
}

void MapData::OnMenuUpdate(){
    tempoInterp += Global::GetFrameTime()*0.0125;
    for(unsigned int i=0; i<_objetosMap.size(); i++){
        ModelObject* objMap = _objetosMap.at(i);
        Color col = {(unsigned char)((1+sin(objMap->Position.x/8 + Global::GetTime()*1))*127),
                    (unsigned char)((1+sin(objMap->Position.z/8 + Global::GetTime()*2))*127),
                    (unsigned char)((1+cos(objMap->Position.x/8 + objMap->Position.z/8 +Global::GetTime()*3))*127)};
        
        objMap->SetColor(Lerp(objMap->color, col, tempoInterp));
    }
}

void MapData::OnRestart(){
    tempoInterp = 0;
}
