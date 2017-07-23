#include "Presenter.h"
#include "PingPongWindow.hpp"

Presenter::Presenter(PingPongWindow* window_) : scaleFactor(SCALE_FACTOR), playground(), window(window_) { }


void Presenter::onTick() {
	/*HDC targetDC = window->startDrawing();
    drawOrUndrawBall(targetDC, false);
	drawOrUndrawPlate(targetDC, false);*/
	try {
		playground.onTick(window->getTickDuration(), window->getPressedKey());
	} catch (const std::runtime_error& exc) {
		window->onGameOver();
	}

    /*if (isBlocksChanged) {
        drawAllBlocks(targetDC);
    }

    drawOrUndrawBall(targetDC, true);
	drawOrUndrawPlate(targetDC, true);

	drawPlayground(targetDC);

    window->endDrawing();*/
}

void Presenter::onInitialShowup() {
    //HDC targetDC = window->startDrawing();
	playground.setToInitialStateAndLaunch();
	/*drawPlayground(targetDC);
    window->endDrawing();*/
}

void Presenter::drawOrUndrawBall(HDC targetDC, bool drawOrUndraw) const {
	HGDIOBJ oldPen, oldBrush;
	HBRUSH innBrush;
	HPEN pen;
	if (drawOrUndraw) {
		pen = CreatePen(PS_SOLID, 1, BALL_COLOR);
		innBrush = CreateSolidBrush(BALL_COLOR);
	} else {
		pen = CreatePen(PS_SOLID, 1, LAYOUT_COLOR);
		innBrush = CreateSolidBrush(LAYOUT_COLOR);
	}
	oldPen = SelectObject(targetDC, pen);
	oldBrush = SelectObject(targetDC, innBrush);
	pingpong::Circle ballLocation = playground.getBall().getBallLocation();
	Ellipse(targetDC, scaleFactor * (ballLocation.center.x - ballLocation.radius), 
		scaleFactor * (playground.getLayoutHeight() - ballLocation.center.y + ballLocation.radius), 
		scaleFactor * (ballLocation.center.x + ballLocation.radius), 
		scaleFactor * (playground.getLayoutHeight() - ballLocation.center.y - ballLocation.radius));
	SelectObject(targetDC, oldPen);
	SelectObject(targetDC, oldBrush);
	DeleteObject(innBrush);
	DeleteObject(pen);
}

void Presenter::drawOrUndrawPlate(HDC targetDC, bool drawOrUndraw) const {
	HGDIOBJ oldPen, oldBrush;
	HBRUSH innBrush;
	HPEN pen;
	if (drawOrUndraw) {
		pen = CreatePen(PS_SOLID, 1, PLATE_COLOR);
		innBrush = CreateSolidBrush(PLATE_COLOR);
	} else {
		pen = CreatePen(PS_SOLID, 1, LAYOUT_COLOR);
		innBrush = CreateSolidBrush(LAYOUT_COLOR);
	}
	oldPen = SelectObject(targetDC, pen);
	oldBrush = SelectObject(targetDC, innBrush);
	pingpong::PlateLocation plateLocation = playground.getPlate().getPlateLocation();
	Rectangle(targetDC, 
		scaleFactor * plateLocation.leftEdge, 
		scaleFactor * (playground.getLayoutHeight() - playground.getPlateOrdinate()),
		scaleFactor * plateLocation.rightEdge,
		scaleFactor * (playground.getLayoutHeight() - playground.getPlateOrdinate() + playground.getPlateWidth()));
	SelectObject(targetDC, oldPen);
	SelectObject(targetDC, oldBrush);
	DeleteObject(innBrush);
	DeleteObject(pen);
}

void Presenter::drawBlock(HDC targetDC, const Block& block) const {
	HGDIOBJ oldPen, oldBrush;
	HBRUSH innBrush;
	HPEN pen;
	if (block.isBlockVisible()) {
		pen = CreatePen(PS_SOLID, 1, BLOCK_CONTOUR_COLOR);
		innBrush = CreateSolidBrush(BLOCK_COLOR);
	} else {
		pen = CreatePen(PS_SOLID, 1, LAYOUT_COLOR);
		innBrush = CreateSolidBrush(LAYOUT_COLOR);
	}
	oldPen = SelectObject(targetDC, pen);
	oldBrush = SelectObject(targetDC, innBrush);
	pingpong::Point upperLeftEdge = block.getUpperLeftCornerCoordinate();
	double blockSize = block.getLinearSize();
	Rectangle(targetDC, 
		scaleFactor * upperLeftEdge.x, 
		scaleFactor * (playground.getLayoutHeight() - upperLeftEdge.y),
		scaleFactor * (upperLeftEdge.x + blockSize),
		scaleFactor * (playground.getLayoutHeight() - upperLeftEdge.y + blockSize));
	SelectObject(targetDC, oldPen);
	SelectObject(targetDC, oldBrush);
	DeleteObject(innBrush);
	DeleteObject(pen);
}

void Presenter::drawAllBlocks(HDC targetDC) const {
	int sizeY = playground.getBlocksManager().getBlocks().size();
	int sizeX = 0;
	if (sizeY > 0) {
		sizeX = playground.getBlocksManager().getBlocks()[0].size();
	}

	for (int i = 0; i < sizeY; i++) {
		for (int j = 0; j < sizeX; j++) {
			drawBlock(targetDC, playground.getBlocksManager().getBlocks()[i][j]);
		}
	}
}

void Presenter::drawPlayground(HDC targetDC) const {
	RECT clientRect; // get Rect from Window class
	HGDIOBJ oldBrush;
	HBRUSH innBrush = CreateSolidBrush(LAYOUT_COLOR);
	oldBrush = SelectObject(targetDC, innBrush);
	FillRect(targetDC, &clientRect, innBrush);
	SelectObject(targetDC, oldBrush);
	DeleteObject(innBrush);

	drawAllBlocks(targetDC);
	drawOrUndrawBall(targetDC, true);
	drawOrUndrawPlate(targetDC, true);
}

void Presenter::onPaint() const {
	HDC targetDC = window->startDrawing();
	drawPlayground(targetDC);
    window->endDrawing();
}