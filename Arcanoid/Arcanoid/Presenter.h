#pragma once

#include "Playground.h"
#include "windows.h"

#define SCALE_FACTOR 2.6

#define BALL_COLOR RGB(0, 255, 0)
#define LAYOUT_COLOR RGB(0, 0, 10)

#define PLATE_COLOR RGB(255, 0, 0)

#define BLOCK_COLOR RGB(128, 0, 0)
#define BLOCK_CONTOUR_COLOR RGB(0, 128, 0)


class PingPongWindow;
class Presenter {
private:
    Playground playground;

    PingPongWindow* window;

    const double scaleFactor;

public:
    Presenter(PingPongWindow* window);

    void onTick();

	void onPaint() const;

    void onInitialShowup();

private:

    void drawBlock(HDC targetDC, const Block& block) const;

    void drawAllBlocks(HDC targetDC) const;

    void drawPlayground(HDC targetDC) const;

	void drawOrUndrawBall(HDC targetDC, bool drawOrUndraw) const;

	void drawOrUndrawPlate(HDC targetDC, bool drawOrUndraw) const;
};
