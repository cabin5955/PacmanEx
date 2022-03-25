//
//  rectangle.cpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/4.
//

#include "rectangle.hpp"

Rectangle::Rectangle(glm::vec2 pos, glm::vec2 size, Color c)
:Position(pos),Size(size),color(c)
{
    
}

Rectangle::~Rectangle()
{
    
}

void Rectangle::Draw(ColorRenderer &renderer){
    renderer.DrawColor(color, Position, Size);
}

