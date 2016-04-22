#include <string>
#include "irrlicht/irrTypes.h"
#include "irrlicht/IVideoDriver.h"
#include "irrlicht/ISceneNode.h"


#ifndef DIROBJECT_HPP
#define DIROBJECT_HPP
class DirObject
{
public:
    DirObject();
    DirObject(char i_type, irr::f32 i_x, irr::f32 i_y, const std::wstring& i_name, bool i_isHighlighted, bool i_isSelected );
    ~DirObject();
    char getType();
    irr::f32 getX();
    irr::f32 getY();
    std::wstring getName();
    bool isHighlighted;
    bool isSelected;
    void setNode(irr::scene::ISceneNode*);
    irr::scene::ISceneNode* getNode();
    void setIsHighlighted(bool, irr::video::IVideoDriver*);
private:
    char type;
    irr::f32 x;
    irr::f32 y;
    std::wstring name;
    irr::scene::ISceneNode* node;
};

#endif //DIROBJECT_HPP
