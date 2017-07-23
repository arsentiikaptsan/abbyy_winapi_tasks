#include <random>
#include "BlocksManager.h"

BlocksManager::BlocksManager(int blocksNumX, int blocksNumY, const double blockSize,
                             const pingpong::Point upperLeftCornerCoordinate) {
    for (int i = 0; i < blocksNumY; i++) {
        blocks.push_back(std::vector<Block>());
        for (int j = 0; j < blocksNumX; j++) {
            blocks[i].push_back(Block(blockSize, pingpong::Point(upperLeftCornerCoordinate.x + j*blockSize,
                                                                 upperLeftCornerCoordinate.y - i*blockSize), false));
        }
    }
}

void BlocksManager::randomlySetBlocksVisible() {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0, 1);
    for (int i = 0; i < blocks.size(); i++) {
        for (int j = 0; j < blocks[0].size(); j++) {
            blocks[i][j].setIsVisible(distribution(generator) < DENSITY_OF_VISIBLE_BLOCKS);
        }
    }
}

const std::vector< std::vector<Block> > &BlocksManager::getBlocks() const {
	return blocks;
}

pingpong::ReflectionType BlocksManager::checkBallBumpedIntoBlocksAndReflectIfSo(pingpong::Circle ballLocation,
                                                                                pingpong::Velocity ballVelocity) {
    int inBlockY, inBlockX;
    double blockSize = blocks[0][0].getLinearSize();
    pingpong::Point upperLeftEdgeOfBlocks = blocks[0][0].getUpperLeftCornerCoordinate();
    int numOfBlocksY = blocks.size();
	int numOfBlocksX = 0;
	if (numOfBlocksY > 0) {
		numOfBlocksX = blocks[0].size();
	}
    inBlockX = (int)((ballLocation.center.x - upperLeftEdgeOfBlocks.x) / blockSize);
    inBlockY = (int)((upperLeftEdgeOfBlocks.y - ballLocation.center.y) / blockSize);

	if (((inBlockX < numOfBlocksX) && (inBlockY < numOfBlocksY)) && blocks[inBlockY][inBlockX].isBlockVisible()) {
        blocks[inBlockY][inBlockX].setIsVisible(false);
        return pingpong::DIAGONAL;
    }

    bool bumpedIntoNearestXBlock = false, bumpedIntoNearestYBlock = false, bumpedIntoNearestDiagonalBlock = false;
    int nearestXBlock, nearestYBlock;

    if (ballVelocity.vX > 0) {
        nearestXBlock = inBlockX + 1;
    } else {
        nearestXBlock = inBlockX - 1;
    }

    if (ballVelocity.vY > 0) {
        nearestYBlock = inBlockY - 1;
    } else {
        nearestYBlock = inBlockY + 1;
    }

    //TODO implement case with single diagonal block
    if ((0 <= nearestXBlock) && (nearestXBlock < numOfBlocksX)) {
		bumpedIntoNearestXBlock = false;
		if (inBlockY < numOfBlocksY) {
			bumpedIntoNearestXBlock = blocks[inBlockY][nearestXBlock].isBlockVisible();
		}
		if (bumpedIntoNearestXBlock) {
			if (nearestXBlock > inBlockX) {
				bumpedIntoNearestXBlock &= (ballLocation.center.x + ballLocation.radius > blocks[inBlockY][nearestXBlock].getUpperLeftCornerCoordinate().x);
			} else {
				bumpedIntoNearestXBlock &= (ballLocation.center.x - ballLocation.radius < blocks[inBlockY][nearestXBlock].getUpperLeftCornerCoordinate().x + blockSize);
			}
		}
    }

    if ((0 <= nearestYBlock) && (nearestYBlock < numOfBlocksY)) {
        bumpedIntoNearestYBlock = blocks[nearestYBlock][inBlockX].isBlockVisible();
        if (nearestYBlock < inBlockY) {
            bumpedIntoNearestYBlock &= (ballLocation.center.y + ballLocation.radius < blocks[nearestYBlock][inBlockX].getUpperLeftCornerCoordinate().y);
        } else {
            bumpedIntoNearestYBlock &= (ballLocation.center.y - ballLocation.radius > blocks[nearestYBlock][inBlockX].getUpperLeftCornerCoordinate().y - blockSize);
        }
    }

    if (bumpedIntoNearestXBlock && bumpedIntoNearestYBlock) {
		blocks[nearestYBlock][inBlockX].setIsVisible(false);
		blocks[inBlockY][nearestXBlock].setIsVisible(false);
		return pingpong::DIAGONAL;
    }

    if (bumpedIntoNearestXBlock) {
		blocks[inBlockY][nearestXBlock].setIsVisible(false);
        return pingpong::FROM_VERTICAL_WALL;
    }

    if (bumpedIntoNearestYBlock) {
		blocks[nearestYBlock][inBlockX].setIsVisible(false);
        return pingpong::FROM_HORIZONTAL_WALL;
    }

    return pingpong::NO_OBSTACLE;
}
