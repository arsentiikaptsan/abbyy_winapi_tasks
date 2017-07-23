#include "Ball.h"
#include <stdexcept>

Ball::Ball(const double radius, const pingpong::Point &initialLocationOfCenter, const double ballSpeed,
           const double plateFrictionCoeff) : radius(radius),
                                              initialLocationOfCenter(
                                                                                          initialLocationOfCenter),
                                              ballSpeed(ballSpeed), plateFriction(plateFrictionCoeff) {}

const pingpong::Point &Ball::getInitialLocationOfCenter() const {
    return initialLocationOfCenter;
}

pingpong::Circle Ball::getBallLocation() const {
    return pingpong::Circle(center, radius);
}

void Ball::setBallToInitialLocationAndLaunch() {
    center = initialLocationOfCenter;
    velocity = pingpong::Velocity(0, ballSpeed);
}

const pingpong::Velocity &Ball::getVelocity() const {
    return velocity;
}

void Ball::bounceFromObstacle(pingpong::ReflectionType wallSide) {
    switch (wallSide) {
        case pingpong::FROM_HORIZONTAL_WALL : {
            velocity.vY = - velocity.vY;
            break;
        }
        case pingpong::FROM_VERTICAL_WALL : {
            velocity.vX = - velocity.vX;
            break;
        }
        case pingpong::DIAGONAL : {
            velocity.vX = - velocity.vX;
            velocity.vY = - velocity.vY;
            break;
        }
    }
}

void Ball::bounceFromPlate(double plateSpeedX) {
    if (velocity.vY > 0) {
        throw std::logic_error("When colliding with plate ball must move down");
    }

    velocity.vY = - velocity.vY;
    double relativeSpeed = -velocity.vX + plateSpeedX;
    velocity.vX += relativeSpeed * plateFriction;
}

void Ball::onTick(double timePassedFormPreviousTick, pingpong::PressedKey pressedKey) {
    center.x += velocity.vX * timePassedFormPreviousTick;
    center.y += velocity.vY * timePassedFormPreviousTick;
}
