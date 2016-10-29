//
// Created by Erik Sevre on 10/25/16.
//

#ifndef BASICAPP_NBODY_CINDER_H
#define BASICAPP_NBODY_CINDER_H

#include <vector>

#include "bh_tree.h"
#include "cinder/gl/gl.h"


// convert between cinder and my custom point types
ci::vec2 point_to_vec2(const point p) {
    return ci::vec2(p.x(), p.y());
}
point vec2_to_point(const ci::vec2 v2) {
    return point(v2.x, v2.y);
}

//
//  Scale functions
//
//   Used to convert between point on screen, and point in computational space
//
ci::ivec2 scale_point_to_screen(const point &p, const region &r, const ci::ivec2 screen) {
    double x_offset = r.get_min_corner().x();
    double y_offset = r.get_min_corner().y();

    double x_scale = screen.x / r.width();
    double y_scale = screen.y / r.height();

    double scale = x_scale > y_scale ? y_scale : x_scale;

    double x = (p.x() - x_offset)*scale;
    double y = (p.y() - y_offset)*scale;

    ci::ivec2 pt((int)x, (int)y);
    return pt;
}
point scale_vec2_to_point(const ci::vec2 &v, const region &r, const ci::vec2 screen) {
    double x_offset = r.get_min_corner().x();
    double y_offset = r.get_min_corner().y();

    double x_scale = screen.x / r.width();
    double y_scale = screen.y / r.height();

    double scale = x_scale > y_scale ? y_scale : x_scale;

    double ptx = v.x / scale + x_offset;
    double pty = v.y / scale + y_offset;

    point pt(ptx, pty);
    return pt;
}


// todo: figure out how to remove bodies, following code isn't working
void pluck_outside_bodies(std::vector<std::shared_ptr<body>> &bodies, const region &r) {
    // use remove if to remove all elements outside the tree
    auto ptr_begin = bodies.begin();
    auto ptr_end = bodies.end();

    std::vector<int> erase_list;

    int index = 0;
    for (auto ptr = ptr_begin; ptr != ptr_end; ++ptr) {
        point pos = ptr->get()->get_position();
        if (!r.is_in(pos)) {
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

    // todo: uncomment code below once it is fixed
    //pluck_outside_bodies(bodies, tree.get_global_region());


    for (int i = 0; i < bodies.size(); ++i) {
        tree.insert_body(bodies[i]);
    }
    tree.update();

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


// todo: modify the function below to add bodies that are drawn on by user
void add_body_to_bodies(std::vector<std::shared_ptr<body>> &bodies, ci::vec2 screen, ci::vec2 pos, region disp_region) {
    point pt = scale_vec2_to_point(pos, disp_region, screen);
    double mass = 5000;
    point vel(0,0);
    std::shared_ptr<body> body_ptr = std::make_shared<body>(mass, pt, vel);
    bodies.push_back(body_ptr);
}



#endif //BASICAPP_NBODY_CINDER_H
