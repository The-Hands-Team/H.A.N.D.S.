#include <memory>
#include <cstring>
#include "irrlicht/irrTypes.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H


void initGraphics();


struct dirObject
{
public:
    dirObject():type('f'),x(5),y(5),isSelected(false){for(int i=0;i<20;i++)name[i]=0;};
    dirObject(char i_type, irr::f32 i_x, irr::f32 i_y, const wchar_t* i_name, bool i_isSelected )
    {
        type = i_type;
        x = i_x;
        y = i_y;
        std::wcscpy(name,i_name);
	isSelected = i_isSelected;
    }
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
        wchar_t* getName()
        {
            return name;
        }
	bool isSelected;
private:
    char type;
    irr::f32 x;
    irr::f32 y;
    wchar_t name[20];
};



void newObjects(dirObject*, size_t);

#endif
