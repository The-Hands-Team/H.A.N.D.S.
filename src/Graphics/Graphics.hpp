#include <memory>
#include <cstring>
#include "irrlicht/irrTypes.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H


void initGraphics();


struct dirObject
{
public:
    dirObject():isSelected(false),type('f'),x(5),y(5){name = new wchar_t[0];};
    dirObject(char i_type, irr::f32 i_x, irr::f32 i_y, const wchar_t* i_name, bool i_isSelected )
    {
        size_t len = std::wcslen( i_name ) + 1;
        isSelected = i_isSelected;
        type = i_type;
        x = i_x;
        y = i_y;
        name = new wchar_t[len];
        std::wcsncpy(name, i_name, len);
    }
    ~dirObject() {delete[] name;};
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
    wchar_t *name;
};



void newObjects(dirObject*, size_t);

#endif
