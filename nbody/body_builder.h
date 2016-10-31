//
// Created by Erik Sevre on 10/21/16.
//
//  The goal is to create some functions to help populate the galaxy with moving bodies.
//  The idea is to add bodies that will be in motion in a way that would result in circular (or eliptic)
//  orbits around a central point
//
//
//
#ifndef TREE_CODE_BODY_BUILDER_H
#define TREE_CODE_BODY_BUILDER_H


#include <iostream>

#include <vector>
#include <cmath>

#include "body.h"
#include "point.h"  // redundant, but included for clarity that points are used here

//
//  Simple body test
//
//  Looks at the orbit of four bodies around a large Central mass
//
//
//  We need the input to be a vector of shared pointers
void body_test_1(std::vector<std::shared_ptr<body>> &bodies) {
    double G = 6.674e-14;

    bodies.clear(); // empty the list

    double big_mass = 200;
    std::shared_ptr<body> central_body = std::make_shared<body>(big_mass, point(0,0), point(0,0));
    double mass = 100;
    double distance = 500;


    double velocity = std::sqrt( (mass + big_mass) / distance);
    //double velocity = .75;

    point direction_east(1, 0);
    point direction_west(-1, 0);
    point direction_north(0, 1);
    point direction_south(0, -1);

    //
    //  place bodies traveling clockwise around the center
    //

    // first body will be located east, traveling south
    point b1_loc = direction_east; b1_loc *= distance;
    point b1_vel = direction_south; b1_vel *= velocity;
    std::shared_ptr<body> b1 = std::make_shared<body>(mass, b1_loc, b1_vel);
    // second body will be located south, traveling west
    point b2_loc = direction_south; b2_loc *= distance;
    point b2_vel = direction_west; b2_vel *= velocity;
    std::shared_ptr<body> b2 = std::make_shared<body>(mass, b2_loc, b2_vel);
    // third body will be located west, traveling north
    point b3_loc = direction_west; b3_loc *= distance;
    point b3_vel = direction_north; b3_vel *= velocity;
    std::shared_ptr<body> b3 = std::make_shared<body>(mass, b3_loc, b3_vel);
    // fourth body will be located north, traveling east
    point b4_loc = direction_north; b4_loc *= distance;
    point b4_vel = direction_east; b4_vel *= velocity;
    std::shared_ptr<body> b4 = std::make_shared<body>(mass, b4_loc, b4_vel);

    //
    //  Add bodies to list
    //
    bodies.push_back(central_body);
    bodies.push_back(b1);
    bodies.push_back(b2);
    bodies.push_back(b3);
    bodies.push_back(b4);
}

#include <random>


//    ****    ****    ****    ****    ****    ****    ****    ****    ****    ****    ****    ****
//
//    many_body_test with collapsing galaxy
//
//    ****    ****    ****    ****    ****    ****    ****    ****    ****    ****    ****    ****
//
//  Very cool with current settings!
//
//  get the number of bodies as an argument passed
//    ****    ****    ****    ****    ****    ****    ****    ****    ****    ****    ****    ****
void many_bodies_test(std::vector<std::shared_ptr<body>> &bodies, int num_bodies = 500) {
    double G = 6.674e-11;
    double pi = acos(-1);


    double big_mass = 10000000;
    double mass = 10000;


    // Clear the list before populating it
    bodies.clear(); // empty the list

    // create and add central mass (black hole)
    std::shared_ptr<body> bdy = std::make_shared<body>(big_mass, point(0,0), point(0,0));
    bodies.push_back(bdy);



    for (int i = 0; i < num_bodies; ++i) {
        double radius = generate_random_in_range(400, 1500);     // random radius between 5000 and 10000
        double theta = generate_random_in_range(0, 2*pi);        // random between 0 and 2*pi
        double phi = theta + pi / 4;

        double velocity = 0.99*std::sqrt(G*(big_mass + mass) / radius);


        double x = radius * cos(theta);
        double y = radius * sin(theta);

        double vx = velocity * cos(phi);
        double vy = velocity * sin(phi);

        point pos(x, y);
        point vel(vx, vy);
        std::shared_ptr<body> b = std::make_shared<body>(mass, pos, vel);

        bodies.push_back(b);
    }


}

enum Rotation{ CLOCKWISE, COUNTERCLOCKWISE };
void add_galaxy_to_body_list(std::vector<std::shared_ptr<body>> &bodies, point center,
                             double min_radius = 500, double max_radius = 1000,
                             int num_bodies = 500, Rotation rotation = Rotation::CLOCKWISE)
{
    double G = 6.674e-11;
    double pi = acos(-1);

    double big_mass = 10000000;
    double mass = 10000;


    // create and add central mass (black hole)
    std::shared_ptr<body> bdy = std::make_shared<body>(big_mass, center, point(0,0));
    bodies.push_back(bdy);



    for (int i = 0; i < num_bodies; ++i) {
        double radius = generate_random_in_range(min_radius, max_radius); // random radius between 5000 and 10000
        double theta = generate_random_in_range(0, 2*pi);  // random between 0 and 2*pi
        double phi = theta; //+ pi / 4;

        switch (rotation){
            case Rotation::CLOCKWISE:
                phi += pi/4.0;
                break;
            case Rotation::COUNTERCLOCKWISE:
                phi -= pi/4.0;
                break;
        }

        double velocity = 0.75*std::sqrt(G*(big_mass + mass) / radius);


        double x = radius * cos(theta) + center.x;
        double y = radius * sin(theta) + center.y;

        double vx = velocity * cos(phi);
        double vy = velocity * sin(phi);

        point pos(x, y);
        point vel(vx, vy);
        std::shared_ptr<body> b = std::make_shared<body>(mass, pos, vel);

        bodies.push_back(b);
    }


}

//
//  Create a galaxy in the upper right, and lower left quadrants
//
void create_two_galaxies(std::vector<std::shared_ptr<body>> &bodies, const region &r, int num_bodies=500) {

    point global_center = r.get_center();
//    double width = r.width();

//    point upper_right_center = (global_center + r.get_max_corner()) / 2.0;
//    point lower_left_center = (global_center - r.get_min_corner()) / 2.0;
//
//    double min_radius = r.get_center().x() / 8;
//    double max_radius = r.get_center().x() / 4;

    point upper_right_center(800, 800);
    point lower_left_center(-800, -800);

    double min_radius = 200;
    double max_radius = 800;

    bodies.clear(); // clear our list of bodies

    // try with one region for now
    add_galaxy_to_body_list(bodies, upper_right_center, min_radius, max_radius, num_bodies/2, Rotation::CLOCKWISE);
    add_galaxy_to_body_list(bodies, lower_left_center, min_radius, max_radius, num_bodies/2, Rotation::COUNTERCLOCKWISE);


}

#endif //TREE_CODE_BODY_BUILDER_H
