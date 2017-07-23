#pragma once

namespace pingpong {
    struct Point {
        double x, y;

        Point(double x_, double y_) : x(x_), y(y_) {}

        Point() : x(0), y(0) {}
    };

    struct Circle {
        Point center;
        double radius;

        Circle(const Point &center, double R) : center(center), radius(R) {}
    };

    struct Velocity {
        double vX, vY;

        Velocity(double vX, double vY) : vX(vX), vY(vY) {}

        Velocity() : vX(0), vY(0) {}
    };

    enum ReflectionType { FROM_HORIZONTAL_WALL, FROM_VERTICAL_WALL, DIAGONAL, NO_OBSTACLE };

    enum PressedKey { KEY_RIGHT, KEY_LEFT, OTHER_KEY, NO_KEY };

    class DynamicObject {
    public:
        virtual void onTick(double timePassedFormPreviousTick, PressedKey pressedKey) = 0;
    };

    struct PlateLocation {
        double leftEdge, rightEdge;

        PlateLocation(double leftEdge, double rightEdge) : leftEdge(leftEdge), rightEdge(rightEdge) {}
    };
}
