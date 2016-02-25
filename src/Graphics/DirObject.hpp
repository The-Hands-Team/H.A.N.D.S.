#include <string>
#include "irrlicht/irrTypes.h"


#ifndef DIROBJECT_HPP
#define DIROBJECT_HPP
class DirObject
{
public:
    DirObject();
    DirObject(char i_type, irr::f32 i_x, irr::f32 i_y, const std::wstring& i_name, bool i_isHighlighted, bool i_isSelected );
    char getType();
        irr::f32 getX();
        irr::f32 getY();
        std::wstring* getName();
	bool isHighlighted;
    bool isSelected;
private:
    char type;
    irr::f32 x;
    irr::f32 y;
    std::wstring name;
};

#endif //DIROBJECT_HPP
