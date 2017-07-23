#pragma once
#include "Utils.h"

class Plate : public pingpong::DynamicObject {
private:
    const double plateLength;
    const double layoutLength;
    const double acceleration;

    double leftEdgeCoordinate;
    double speed;

    pingpong::PressedKey lastTickPressedKey;


public:
    Plate(const double plateLength, const double layoutLength, const double acceleration);

    double getSpeed() const;

    pingpong::PlateLocation getPlateLocation() const;

    void setPlateToMiddleOFLayout();

    void onTick(double timePassedFormPreviousTick, pingpong::PressedKey pressedKey) override;
};
