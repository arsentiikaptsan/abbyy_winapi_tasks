#include "Playground.h"
#define EPSILON BALL_RADIUS / 10


Playground::Playground() : ball(BALL_RADIUS, pingpong::Point(LAYOUT_LENGTH / 2, PLATE_ORDINATE + BALL_RADIUS), BALL_SPEED, PLATE_FRICTION),
                           plate(PLATE_LENGTH, LAYOUT_LENGTH, PLATE_ACCELERATION),
                           blocksManager(NUM_OF_BLOCKS_X, NUM_OF_BLOCKS_Y, LAYOUT_LENGTH / NUM_OF_BLOCKS_X, pingpong::Point(0, LAYOUT_HEIGHT)),
                           layoutLength(LAYOUT_LENGTH),
                           layoutHeight(LAYOUT_HEIGHT),
                           plateOrdinate(PLATE_ORDINATE),
						   plateWidth(PLATE_WIDTH) {}

void Playground::setToInitialStateAndLaunch() {
    blocksManager.randomlySetBlocksVisible();
    plate.setPlateToMiddleOFLayout();
    ball.setBallToInitialLocationAndLaunch();
}

bool Playground::onTick(double timePassedFormPreviousTick, pingpong::PressedKey pressedKey) {
    ball.onTick(timePassedFormPreviousTick, pressedKey);
    plate.onTick(timePassedFormPreviousTick, pressedKey);

    if (isGameOver()) {
        throw std::runtime_error("Game over");
    }

    if (checkIfBallBumpedIntoPlate()) {
        ball.bounceFromPlate(plate.getSpeed());
    }

    pingpong::ReflectionType reflectionTypeFromBlocks = blocksManager.checkBallBumpedIntoBlocksAndReflectIfSo(ball.getBallLocation(), ball.getVelocity());
    ball.bounceFromObstacle(reflectionTypeFromBlocks);

    ball.bounceFromObstacle(checkIfBallBumpedIntoWall()) ;

    return (reflectionTypeFromBlocks != pingpong::NO_OBSTACLE);
}

pingpong::ReflectionType Playground::checkIfBallBumpedIntoWall() {
    if ((ball.getBallLocation().center.y + ball.getBallLocation().radius) > layoutHeight) {
        return pingpong::FROM_HORIZONTAL_WALL;
    }

    if (((ball.getBallLocation().center.x + ball.getBallLocation().radius) > layoutLength)
            || (ball.getBallLocation().center.x - ball.getBallLocation().radius < 0)) {
        return pingpong::FROM_VERTICAL_WALL;
    }

    return pingpong::NO_OBSTACLE;
}

bool Playground::isGameOver() const {
    return ((ball.getBallLocation().center.y - ball.getBallLocation().radius < plateOrdinate - EPSILON)
                && ((ball.getBallLocation().center.x > plate.getPlateLocation().rightEdge) || (ball.getBallLocation().center.x < plate.getPlateLocation().leftEdge)));
}

bool Playground::checkIfBallBumpedIntoPlate() {
    return ((ball.getBallLocation().center.y - ball.getBallLocation().radius < plateOrdinate - EPSILON) 
		&& (ball.getBallLocation().center.x < plate.getPlateLocation().rightEdge) 
		&& (ball.getBallLocation().center.x > plate.getPlateLocation().leftEdge));
}

const Ball &Playground::getBall() const {
    return ball;
}

const Plate &Playground::getPlate() const {
    return plate;
}

const BlocksManager &Playground::getBlocksManager() const {
    return blocksManager;
}

double Playground::getLayoutLength() const {
	return layoutLength;
}

double Playground::getLayoutHeight() const {
	return layoutHeight;
}

double Playground::getPlateOrdinate() const {
	return plateOrdinate;
}

double Playground::getPlateWidth() const {
	return plateWidth;
}