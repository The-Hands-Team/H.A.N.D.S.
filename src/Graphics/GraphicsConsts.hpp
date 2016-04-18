#ifndef GRAPHICS_CONSTS
#define GRAPHICS CONSTS
namespace GraphicsConsts
{
    const int CELL_WIDTH = 5;
    const int OBJ_WIDTH = CELL_WIDTH * 0.75;
    const int GRID_WIDTH = 5;
    const int GRID_HEIGHT = GRID_WIDTH;
    const int GRID_DEPTH = GRID_HEIGHT;
    const int VIEW_WIDTH = CELL_WIDTH * (GRID_WIDTH + 1);
    const int VIEW_HEIGHT = CELL_WIDTH * (GRID_HEIGHT + 1);
    const int VIEW_DEPTH = CELL_WIDTH * (GRID_DEPTH + 1);
    const int CAM_HEIGHT = CELL_WIDTH * GRID_WIDTH * 0.5;
}
#endif
