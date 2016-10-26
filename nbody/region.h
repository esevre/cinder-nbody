//
// Created by Erik Sevre on 10/19/16.
//

#ifndef TREE_CODE_REGION_H
#define TREE_CODE_REGION_H

#include <iostream> // remove this when std removed

#include <ostream>
#include "point.h"

enum Quadrant { NW, NE, SE, SW, OUTSIDE };  // used for finding quadrants

class region {

protected:
    point min_corner, max_corner, center;

public:

    //
    // todo: add constructor code to ensure that the min_corner is actually min
    //       this will allow for a consistent region for any two oposite corners given
    region(const point &min_corner, const point &max_corner) : min_corner(min_corner), max_corner(max_corner)
    {
        center = min_corner + (max_corner - min_corner) / 2.0;
    }
    region(double xmin, double ymin, double xmax, double ymax) : min_corner(point(xmin,ymin)), max_corner(point(xmax, ymax))
    {
        center = min_corner + (max_corner - min_corner) / 2.0;
    }

    region(const region &r) {
        min_corner = r.min_corner;
        max_corner = r.max_corner;
        center = r.center;
    }

    // getters and setters
    point get_min_corner() const {
        return min_corner;
    }
    point get_max_corner() const {
        return max_corner;
    }

    //
    //  Operator Overloads
    //
    region &operator=(const region &rhs) {
        min_corner = rhs.min_corner;
        max_corner = rhs.max_corner;
        center = rhs.center;
        return *this;
    }


    point get_center() const { return center; }

    //
    //  Check if a point is in our region (boundaries are included in the region)
    //
    bool is_in(point p) const {
        bool x_in_range = min_corner.x() <= p.x() and p.x() <= max_corner.x();
        bool y_in_range = min_corner.y() <= p.y() and p.y() <= max_corner.y();
        return x_in_range and y_in_range;
    }

    //
    // The following four functions return true if the point is in the quadrant
    //
    bool is_nw(const point &p) const {
        if (!is_in(p)) return false; // if p is outside the cell, it is not in the quadrant
        bool north = p.y() >= center.y();
        bool west = p.x() <= center.x();
        return north and west;
    }
    bool is_ne(const point &p) const {
        if (!is_in(p)) return false; // if p is outside the cell, it is not in the quadrant
        bool north = p.y() >= center.y();
        bool east = p.x() >= center.x();
        return north and east;
    }
    bool is_sw(const point &p) const {
        if (!is_in(p)) return false; // if p is outside the cell, it is not in the quadrant
        bool south = p.y() <= center.y();
        bool west = p.x() <= center.x();
        return south and west;
    }
    bool is_se(const point &p) const {
        if (!is_in(p)) return false; // if p is outside the cell, it is not in the quadrant
        bool south = p.y() <= center.y();
        bool east = p.x() >= center.x();
        return south and east;
    }

    //
    //  Get the quadrant for a given point
    //
    //  Since border points will return true for both, the order that we call our tests
    //  will determine what quadrant a point belongs to. Basically, how to break ties.
    //
    //  In cases of ties, presidence goes as follows:
    //          NW --> NE --> SW --> SE
    //  basically clockwise starting in the NW quadrant
    //
    Quadrant get_quadrant(const point &p) const {
        if (is_nw(p)) return Quadrant::NW;
        if (is_ne(p)) return Quadrant::NE;
        if (is_sw(p)) return Quadrant::SW;
        if (is_se(p)) return Quadrant::SE;
        return Quadrant::OUTSIDE; // this should never get hit, but just in case
    }

    //
    //  For our nbody, we will compute the width and height of our regions
    //
    double width() const { return max_corner.x() - min_corner.x(); }
    double height() const { return max_corner.y() - min_corner.y(); }

    //
    //  create a subregion
    //
    //  This will be useful when we need to create new nodes of our trees
    //  todo: add tests for create_subregion
    //
    // creating a pointer r and returning by reference saves an object destruction
    region &create_subregion(Quadrant q) const {
        if (q == Quadrant::NW) {
            point new_min(min_corner.x(), center.y());
            point new_max(center.x(), max_corner.y());
            region *r = new region(new_min, new_max);
            return *r;
        }
        if (q == Quadrant::NE) {
            // northeast quadrant is easy to define with center and max corner
            region *r = new region(center, max_corner);
            return *r;
        }
        if (q == Quadrant::SE) {
            point new_min(center.x(), min_corner.y());
            point new_max(max_corner.x(), center.y());
            region *r = new region(new_min, new_max);
            return *r;
        }
        if (q == Quadrant::SW) {
            // Southwest is easy to define with center and max corner
            region *r = new region(min_corner, center);
            return *r;
        }
        std::cout << "this should never happen (in create_subregion)" << std::endl;

        region *r = new region(0,0,0,0);
        return *r; // this should never happen
    }

    region create_subregion(const point &p) const {
        Quadrant q = get_quadrant(p);
        region r = create_subregion(q);
        return r;
    }

    friend std::ostream &operator<<(std::ostream &os, const region &region1) {
        os << "min_corner: " << region1.min_corner << " max_corner: " << region1.max_corner;
        return os;
    }
};


//
//  todo: Move testing code below to another testing header file
//  (moving code should improve performance)
//
#include <random>
//
//  Test that we are creating the center properly
//
void swap(double &a, double &b) {
    double t = a;
    a = b;
    b = t;
}
double generate_random_in_range(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    // std::uniform_int_distribution<> create_random_number(-100, 100); // int number
    std::uniform_real_distribution<> create_random_number(min, max);
    return create_random_number(gen);
}
bool test_center_generated_single_test(bool verbose=true) {
    // generate random ranges
    double xmin = generate_random_in_range(-1000, 1000);
    double xmax = generate_random_in_range(-1000, 1000);
    double ymin = generate_random_in_range(-1000, 1000);
    double ymax = generate_random_in_range(-1000, 1000);
    // check that they are formatted properly xmin < xmax (since random this might not happen)
    if (xmax < xmin ) { swap(xmin, xmax); }
    if (ymax < ymin ) { swap(ymin, ymax); }

    // create the region to test
    region r(xmin, ymin, xmax, ymax);

    // figure out the width and midpoint
    double x_width = (xmax - xmin)/2.0;
    double y_width = (ymax - ymin)/2.0;
    double xmid = xmin + x_width;
    double ymid = ymin + y_width;

    // define a point to test:
    double west = generate_random_in_range(xmin, xmid);  // in range (xmin, xmid)
    double south = generate_random_in_range(ymin, ymid); // in range (ymin, ymid)
    double east  = generate_random_in_range(xmid, xmax);  // in range (xmid, xmax)
    double north = generate_random_in_range(ymid, ymax); // in range (ymid, ymax)

    // create points
    point nw(west, north);
    point ne(east, north);
    point se(east, south);
    point sw(west, south);

    bool test_success = true; // try to find a bad case below

    //
    //  Test for true positive results
    //
    if (!r.is_nw(nw)) {
        test_success = false;
        std::cout << "mistake with NW!" << std::endl;
    }
    if (!r.is_ne(ne)) {
        test_success = false;
        std::cout << "mistake with NE!" << std::endl;
    }
    if (!r.is_sw(sw)) {
        test_success = false;
        std::cout << "mistake with SW!" << std::endl;
    }
    if (!r.is_se(se)) {
        test_success = false;
        std::cout << "mistake with SE!" << std::endl;
    }
    //
    // test for false positive results
    //  (note this may be flawed for edge cases)
    //
    // todo: add tests for false positives (I won't worry about this for now)
    // todo: add tests for function get_quadrant

    return test_success;
}
bool test_center_generated_correctly(bool verbose=true) {
    bool test_success = true;
    for (int i = 0; i < 1000; ++i) {
        if (!test_center_generated_single_test(verbose)){
            test_success = false;
        }
    }
    std::cout << std::endl << "Test completed" << std::endl;
    return test_success;
}

//
//  test that our region correctly identifies quadrants
//
// todo: replace cout with ostream and return a tuple (bool, ostream) or just ostream
//
bool test_quadrants(bool verbose=true){
    // Test NW
    point min(0, 0);
    point max(10, 10);
    point nw01(1, 6);
    point nw02(5, 5);
    point ne01(6, 8);
    point ne02(9, 9);
    point se01(6, 1);
    point se02(9, 4);
    point sw01(2, 2);
    point sw02(1,4.5);

    region r(min, max);

    //    ****    ****    ****    ****    ****    ****    ****
    //                    Testing NW
    //    ****    ****    ****    ****    ****    ****    ****
    if (r.is_nw(nw01)){
        std::cout << "nw test 1 passed" << std::endl;
    }
    if (r.is_nw(nw02)){
        std::cout << "nw test 2 passed" << std::endl;
    }

    //    ****    ****    ****    ****    ****    ****    ****
    //                    Testing NE
    //    ****    ****    ****    ****    ****    ****    ****
    if (r.is_ne(ne01)){
        std::cout << "ne test 1 passed" << std::endl;
    }
    if (r.is_ne(ne02)){
        std::cout << "ne test 2 passed" << std::endl;
    }

    //    ****    ****    ****    ****    ****    ****    ****
    //                    Testing SE
    //    ****    ****    ****    ****    ****    ****    ****
    if (r.is_se(se01)){
        std::cout << "se test 1 passed" << std::endl;
    }
    if (r.is_se(se02)){
        std::cout << "se test 2 passed" << std::endl;
    }


    //    ****    ****    ****    ****    ****    ****    ****
    //                    Testing SW
    //    ****    ****    ****    ****    ****    ****    ****
    if (r.is_sw(sw01)){
        std::cout << "sw test 1 passed" << std::endl;
    }
    if (r.is_sw(sw02)){
        std::cout << "sw test 2 passed" << std::endl;
    }




    return true;
}


#endif //TREE_CODE_REGION_H
