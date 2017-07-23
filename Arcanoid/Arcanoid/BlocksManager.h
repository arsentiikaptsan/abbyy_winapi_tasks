#pragma once
#include <vector>
#include "Block.h"

#define DENSITY_OF_VISIBLE_BLOCKS 0.3

class BlocksManager {
private:
    std::vector< std::vector<Block> > blocks;


public:
    BlocksManager(int blocksNumX, int blocksNumY, const double blockSize, const pingpong::Point upperLeftCornerCoordinate);

    void randomlySetBlocksVisible();

	const std::vector< std::vector<Block> > &getBlocks() const;

    pingpong::ReflectionType checkBallBumpedIntoBlocksAndReflectIfSo(pingpong::Circle ballLocation,
                                                                     pingpong::Velocity ballVelocity);
};
