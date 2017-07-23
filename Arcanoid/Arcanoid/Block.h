#pragma once
#include "Utils.h"

class Block {
private:
    const double linearSize;
    const pingpong::Point upperLeftCornerCoordinate;

    bool isVisible;


public:
    Block(const double linearSize, const pingpong::Point upperLeftCornerCoordinate, bool isVisible);

    bool isBlockVisible() const;

    void setIsVisible(bool isVisible);

    const double getLinearSize() const;

    const pingpong::Point &getUpperLeftCornerCoordinate() const;
};
