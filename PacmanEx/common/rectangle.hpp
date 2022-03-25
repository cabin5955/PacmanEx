//
//  rectangle.hpp
//  BreakoutEx
//
//  Created by chen caibin on 2022/3/4.
//

#ifndef rectangle_hpp
#define rectangle_hpp

#include "glad.h"
#include <glm/glm.hpp>

#include "texture.hpp"
#include "color_renderer.hpp"
#include "touch_delegate_protocol.h"
#include "color.h"

class Rectangle
{
public:
    Rectangle(glm::vec2 pos, glm::vec2 size, Color color);
    ~Rectangle();

    glm::vec2   Position, Size;
    Color color;
    
    void Draw(ColorRenderer &renderer);
};

#endif /* rectangle_hpp */
