#pragma once
#include "Ball.h"
#include "Plate.h"
#include "BlocksManager.h"

#define LAYOUT_LENGTH 100.0
#define LAYOUT_HEIGHT 200.0

#define PLATE_ORDINATE 10.0

#define PLATE_LENGTH 20.0
#define PLATE_FRICTION 0.4
#define PLATE_ACCELERATION 250.0
#define PLATE_WIDTH 2

#define BALL_RADIUS 5.0
#define BALL_SPEED 80.0 //in 1 second

#define NUM_OF_BLOCKS_X 10
#define NUM_OF_BLOCKS_Y 10


class Playground {
private:
    Ball ball;
    Plate plate;
    BlocksManager blocksManager;

    const double layoutLength;
    const double layoutHeight;

    const double plateOrdinate;
	const double plateWidth;

public:
    Playground();

    void setToInitialStateAndLaunch();

    pingpong::ReflectionType checkIfBallBumpedIntoWall();

    bool checkIfBallBumpedIntoPlate();

    bool isGameOver() const;

    bool onTick(double timePassedFormPreviousTick, pingpong::PressedKey pressedKey);

	const Ball &getBall() const;

    const Plate &getPlate() const;

    const BlocksManager &getBlocksManager() const;

	double getLayoutLength() const;

	double getLayoutHeight() const;

	double getPlateOrdinate() const;

	double getPlateWidth() const;
};
