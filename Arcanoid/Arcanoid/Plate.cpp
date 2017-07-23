#include <algorithm>
#include "Plate.h"

Plate::Plate(const double plateLength, const double layoutLength, const double acceleration) : plateLength(plateLength),
                                                                                            layoutLength(layoutLength),
                                                                                            acceleration(acceleration) {
    lastTickPressedKey = pingpong::NO_KEY;
}

double Plate::getSpeed() const {
    return speed;
}

pingpong::PlateLocation Plate::getPlateLocation() const {
    return pingpong::PlateLocation(leftEdgeCoordinate, leftEdgeCoordinate + plateLength);
}

void Plate::setPlateToMiddleOFLayout() {
    leftEdgeCoordinate = layoutLength / 2 - plateLength / 2;
}

void Plate::onTick(double timePassedFormPreviousTick, pingpong::PressedKey pressedKey) {
    switch (pressedKey) {
        case pingpong::OTHER_KEY:
        case pingpong::NO_KEY: {
            speed = 0;
            break;
        }
        case pingpong::KEY_RIGHT: {
            if (lastTickPressedKey != pingpong::KEY_RIGHT) {
                speed = acceleration * timePassedFormPreviousTick;
            } else {
                speed += acceleration * timePassedFormPreviousTick;
            }

            break;
        }
        case pingpong::KEY_LEFT: {
            if (lastTickPressedKey != pingpong::KEY_LEFT) {
                speed = -acceleration * timePassedFormPreviousTick;
            } else {
                speed -= acceleration * timePassedFormPreviousTick;
            }
            break;
        }
    }

    if (speed < 0) {
        leftEdgeCoordinate = std::max(0.0, leftEdgeCoordinate + speed*timePassedFormPreviousTick);
    } else {
        leftEdgeCoordinate = std::min(layoutLength - plateLength, leftEdgeCoordinate + speed*timePassedFormPreviousTick);
    }

    lastTickPressedKey = pressedKey;
}
