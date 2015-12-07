#include <memory>
#include <cwchar>
#include <cstring>
#ifndef GRAPHICS_H
#define GRAPHICS_H


void initGraphics();


struct dirObject
{
public:
	dirObject(){};
	dirObject(char i_type, int i_x, int i_y, const wchar_t* i_name )
	{
		type = i_type;
		x = i_x;
		y = i_y;
		std::wcscpy(name,i_name);
		isSelected = false;
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
	void select()
	{
		isSelected = true;
	}
	void deselect()
	{
		isSelected = false;
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
