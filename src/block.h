#ifndef BLOCK_H
#define BLOCK_H

enum BlockID: unsigned char {
    Block_Air,
    Block_Dirt,
    Block_DirtWall,
    Block_Ground,
    Block_PathStraight1,
    Block_PathStraight2,
    Block_PathCurved1,
    Block_PathCurved2,
    Block_PathCurved3,
    Block_PathCurved4,
    Block_Intersection,
    Block_Level,
    Block_Bridge1,
    Block_Bridge2,
    Block_Water,

    Block_Count,
    Block_Start = Block_Dirt
};

void draw_block(BlockID id);

#endif