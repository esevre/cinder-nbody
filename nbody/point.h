//
// Created by Erik Sevre on 10/19/16.
//

#ifndef TREE_CODE_POINT_H
#define TREE_CODE_POINT_H

#include <vector>
//#include <array>
#include <ostream>
#include <cmath>


class point {
public:
    double x, y;
    point(double x=0.0, double y=0.0) : x(x), y(y) { }

    point(const point &p) {
        this->x = p.x;
        this->y = p.y;
    }

    //
    //  Compute the lenght of the vector from the origin
    //
    double length() const {
        return std::sqrt( std::pow(x, 2) + std::pow(y, 2) );
    }


    //
    //  Operator overloading
    //
    // assignment operator
    point &operator=(const point &rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    // check equality
    bool operator==(const point &rhs) const {
        return x == rhs.x and y == rhs.y;
    }
    bool operator!=(const point &rhs) const {
        return !(rhs == *this);
    }

    // multiply by point by a double number
    point &operator*=(const double d) {
        x *= d;
        y *= d;
        return *this;
    }
    point operator/(const double d) {
        point p = *this;
        p /= d;
        return p;
    }
    point &operator/=(const double d) {
        x /= d;
        y /= d;
        return *this;
    }
    point operator/(const double d) const {
        point p = *this;
        p /= d;
        return p;
    }

    // point based addition/subtraction
    point &operator+=(const point& rpoint) {
        x += rpoint.x;
        y += rpoint.y;
        return *this;
    }
    point operator+(const point & rpoint) const {
        point p = *this;
        p += rpoint;
        return p;
    }
    point &operator-=(const point& rpoint) {
        x -= rpoint.x;
        y -= rpoint.y;
        return *this;
    }
    point operator-(const point & rpoint) const {
        point p = *this;
        p -= rpoint;
        return p;
    }

    // output stream overload (for easy display)
    friend std::ostream &operator<<(std::ostream &os, const point &point1) {
        os << "( " << point1.x << ", " << point1.y << ")";
        return os;
    }

    // distance = sqrt( (a.x - b.x)^2 + (a.y - b.y)^2 )
    friend double distance(const point &a, const point &b) {
        return std::sqrt( std::pow(a.x - b.x,2) + std::pow(a.y - b.y, 2) );
    }

    friend point operator*(const point &p, const double d) {
        point pt = p;
        pt *= d;
        return pt;
    }

};



#endif //TREE_CODE_POINT_H
