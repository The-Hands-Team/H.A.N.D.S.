#include <memory>
#include <cstring>
#include "irrlicht/irrTypes.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H


void initGraphics();


struct dirObject
{
public:
    dirObject():isHighlighted(false),type('f'),x(5),y(5),name(L""){  };
    dirObject(char i_type, irr::f32 i_x, irr::f32 i_y, const std::wstring& i_name, bool i_isHighlighted, bool i_isSelected )
        : isHighlighted( i_isHighlighted )
        , isSelected( i_isSelected )
        , type( i_type )
        , x( i_x )
        , y( i_y )
        , name( i_name )
    {
    };
    char getType()
        {
            return type;
        }
        irr::f32 getX()
        {
            return x;
        }
        irr::f32 getY()
        {
            return y;
        }
        std::wstring* getName()
        {
            return &name;
        }
	bool isHighlighted;
    bool isSelected;
private:
    char type;
    irr::f32 x;
    irr::f32 y;
    std::wstring name;
};



void newObjects(dirObject*, size_t);

#endif
