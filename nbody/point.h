//
// Created by Erik Sevre on 10/19/16.
//

#ifndef TREE_CODE_POINT_H
#define TREE_CODE_POINT_H

#include <vector>
#include <array>
#include <ostream>
#include <cmath>


class point {

private:
    std::array<double, 2> coords;

public:
    point(const std::array<double, 2> &coords) : coords(coords) { }

    point(double x=0.0, double y=0.0) {
        coords[0] = x;
        coords[1] = y;
    }

    point(const point &p) {
        coords[0] = p.coords[0];
        coords[1] = p.coords[1];
    }

    double x() const { return coords[0]; }
    double y() const { return coords[1]; }
    void x(double x) { coords[0] = x; }
    void y(double y) { coords[1] = y; }

    //
    //  Some useful math functions
    //

    //
    //  Compute the lenght of the vector from the origin
    //
    double length() const {
        return std::sqrt( std::pow(coords[0], 2) + std::pow(coords[1], 2) );
    }


    //
    //  Operator overloading
    //
    // assignment operator
    point &operator=(const point &rhs) {
        coords = rhs.coords;
        return *this;
    }

    // check equality
    bool operator==(const point &rhs) const {
        return coords == rhs.coords;
    }
    bool operator!=(const point &rhs) const {
        return !(rhs == *this);
    }


    // subscript access (get and set with subscript) + get for const objects
    double &operator[](int i) {
        return coords[i];
    }
    double operator[](int i) const { return coords[i]; } // const obj accessor

    // multiply by point by a double number
    point &operator*=(const double d) {
        coords[0] *= d;
        coords[1] *= d;
        return *this;
    }
    point operator/(const double d) {
        point p = *this;
        p /= d;
        return p;
    }
    point &operator/=(const double d) {
        coords[0] /= d;
        coords[1] /= d;
        return *this;
    }
    point operator/(const double d) const {
        point p = *this;
        p /= d;
        return p;
    }

    // point based addition/subtraction
    point &operator+=(const point& rpoint) {
        coords[0] += rpoint.coords[0];
        coords[1] += rpoint.coords[1];
        return *this;
    }
    point operator+(const point & rpoint) const {
        point p = *this;
        p += rpoint;
        return p;
    }
    point &operator-=(const point& rpoint) {
        coords[0] -= rpoint.coords[0];
        coords[1] -= rpoint.coords[1];
        return *this;
    }
    point operator-(const point & rpoint) const {
        point p = *this;
        p -= rpoint;
        return p;
    }

    // output stream overload (for easy display)
    friend std::ostream &operator<<(std::ostream &os, const point &point1) {
        os << "( " << point1.coords[0] << ", " << point1.coords[1] << ")";
        return os;
    }

    // distance = sqrt( (a.x - b.x)^2 + (a.y - b.y)^2 )
    friend double distance(const point &a, const point &b) {
        return std::sqrt( std::pow(a.x() - b.x(),2) + std::pow(a.y() - b.y(), 2) );
    }

    friend point operator*(const point &p, const double d) {
        point pt = p;
        pt *= d;
        return pt;
    }

};



#endif //TREE_CODE_POINT_H
