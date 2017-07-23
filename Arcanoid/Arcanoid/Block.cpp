#include <stdexcept>
#include "Block.h"

Block::Block(const double linearSize, const pingpong::Point upperLeftCornerCoordinate, bool isVisible) : linearSize(linearSize),
                                                                                                upperLeftCornerCoordinate(
                                                                                                        upperLeftCornerCoordinate),
                                                                                                isVisible(isVisible) {}

bool Block::isBlockVisible() const {
    return isVisible;
}

void Block::setIsVisible(bool isVisible) {
    Block::isVisible = isVisible;
}

const double Block::getLinearSize() const {
    return linearSize;
}

const pingpong::Point &Block::getUpperLeftCornerCoordinate() const {
    return upperLeftCornerCoordinate;
}
