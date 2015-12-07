#include <memory>
#include <cwchar>
#include <cstring>

#ifndef GRAPHICS_H
#define GRAPHICS_H


void initGraphics();


struct dirObject
{
public:
    dirObject():type('f'),x(5),y(5),isSelected(false){for(int i=0;i<20;i++)name[i]=0;};
    dirObject(char i_type, int i_x, int i_y, const wchar_t* i_name, bool i_isSelected )
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
        int getX()
        {
            return x;
        }
        int getY()
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
    int x;
    int y;
    wchar_t name[20];
};



void newObjects(dirObject*, size_t);

#endif
