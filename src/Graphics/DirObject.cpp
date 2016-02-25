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
    std::wstring* DirObject::getName()
    {
        return &name;
    }
