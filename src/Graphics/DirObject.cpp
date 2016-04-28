#include <iostream>
#include "DirObject.hpp"

DirObject::DirObject()
    :isHighlighted(false)
    , type('f')
    , x(5)
    , y(5)
    , name(L"")
    {
    }

DirObject::DirObject( char i_type
                    , irr::f32 i_x
                    , irr::f32 i_y
                    , const std::wstring& i_name
                    , bool i_isHighlighted
                    , bool i_isSelected )
    : isHighlighted( i_isHighlighted )
    , isSelected( i_isSelected )
    , type( i_type )
    , x( i_x )
    , y( i_y )
    , name( i_name )
    {
        node = nullptr;
        text_node = nullptr;
    }

DirObject::~DirObject()
{
    if (nullptr != node)
    {
        node->remove();
    }
}

char DirObject::getType()
    {
        return type;
    }
    irr::f32 DirObject::getX()
    {
        return x;
    }
    irr::f32 DirObject::getY()
    {
        return y;
    }
    std::wstring DirObject::getName()
    {
        return name;
    }
void DirObject::setNode(irr::scene::IMeshSceneNode* node_p)
{
    node = node_p;
}

void DirObject::setNodeText(irr::scene::IBillboardTextSceneNode* node_t)
{
    text_node = node_t;
}

irr::scene::ISceneNode* DirObject::getNode()
{
    return node;
}

irr::scene::IBillboardTextSceneNode* DirObject::getNodeText()
{
    return text_node;
}

void DirObject::setIsHighlighted(bool h, irr::video::IVideoDriver* driver)
{
    isHighlighted = h;
    if(isHighlighted)
    {
        node->getSceneManager()->getMeshManipulator()->setVertexColors(node->getMesh(), irr::video::SColor(255,0,255,0));
    }
    else
    {
        node->getSceneManager()->getMeshManipulator()->setVertexColors(node->getMesh(), irr::video::SColor(255,0,0,255));
    }
}

void DirObject::setTranslucent(bool b)
{
    if(b)
    {
       node->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
    }
    else
    {
       node->setMaterialType(irr::video::EMT_SOLID);
    }
    //text_node->setVisible(b);
} 
