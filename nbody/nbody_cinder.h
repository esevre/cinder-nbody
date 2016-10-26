//
// Created by Erik Sevre on 10/25/16.
//

#ifndef BASICAPP_NBODY_CINDER_H
#define BASICAPP_NBODY_CINDER_H

#include <vector>

#include "bh_tree.h"
#include "cinder/gl/gl.h"

// Add this line of code for cinder support
ci::vec2 point_to_vec2(const point p) {
    return ci::vec2(p.x(), p.y());
}

//
//  Scale point to fit on screen
//
ci::ivec2 scale_point_to_screen(const point &p, const region &r, const ci::ivec2 screen) {
    double x_offset = r.get_min_corner().x();
    double y_offset = r.get_min_corner().y();

    double x_scale = screen.x / r.width();
    double y_scale = screen.y / r.height();

    double x = (p.x() - x_offset)*x_scale;
    double y = (p.y() - y_offset)*y_scale;

    ci::ivec2 pt((int)x, (int)y);
    return pt;
}



void pluck_outside_bodies(std::vector<std::shared_ptr<body>> &bodies, const bh_tree &tree) {
    // use remove if to remove all elements outside the tree
    auto ptr_begin = bodies.begin();
    auto ptr_end = bodies.end();

    std::vector<int> erase_list;

    int index = 0;
    for (auto ptr = ptr_begin; ptr != ptr_end; ++ptr) {
        std::shared_ptr<body> b = *ptr;
        if (tree.is_outside( b )) {
            std::cout << "something needs to be removed" << std::endl;
            erase_list.push_back(index);
        }
        ++index;
    }

    for (int i = erase_list.size()-1; i >= 0; --i) {
        bodies.erase(bodies.begin() + erase_list[i]);
    }

    //ptr_end = std::remove_if(ptr_begin, ptr_end, fnc);

}

std::vector<point> compute_forces(std::vector<std::shared_ptr<body>> &bodies, const region r){
    bh_tree tree(r);

    pluck_outside_bodies(bodies, tree);

    for (int i = 0; i < bodies.size(); ++i) {
        tree.insert_body(bodies[i]);
    }

    std::vector<point> forces;
    for (auto &b : bodies) {
        point force = tree.compute_force(b);
        forces.push_back(force);
    }
    return forces;
}


void update_bodies_with_forces(std::vector<std::shared_ptr<body>> &bodies, const std::vector<point> &forces) {
    double dt = 10000.0;
    if (bodies.size() != forces.size()) {
        std::cout << "error in updating bodies with forces, sizes don't match" << std::endl;
    }
    for (int i = 0; i < bodies.size(); ++i) {
        bodies[i]->update_based_on_force_dt(forces[i], dt);
    }
}





#endif //BASICAPP_NBODY_CINDER_H
