#pragma once
#include "Utils.h"

class Ball : public pingpong::DynamicObject {
private:
    const double radius;
    const pingpong::Point initialLocationOfCenter;
    const double ballSpeed;
    const double plateFriction;

    pingpong::Velocity velocity;
    pingpong::Point center;


public:
    Ball(const double radius, const pingpong::Point &initialLocationOfCenter, const double ballSpeed,
             const double plateFriction);

    const pingpong::Point &getInitialLocationOfCenter() const;

    pingpong::Circle getBallLocation() const;

    const pingpong::Velocity &getVelocity() const;

    void setBallToInitialLocationAndLaunch();

    void bounceFromObstacle(pingpong::ReflectionType wallSide);

    void bounceFromPlate(double plateSpeedX);

	void onTick(double timePassedFormPreviousTick, pingpong::PressedKey pressedKey) override;
};
