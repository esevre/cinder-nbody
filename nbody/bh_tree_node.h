//
// Created by Erik Sevre on 10/19/16.
//

#ifndef TREE_CODE_BH_TREE_NODE_H
#define TREE_CODE_BH_TREE_NODE_H

#include <queue>
#include <stack>
#include <ostream>
#include <cmath>

//
//  To know what region this cell occupies
//
#include "region.h"
#include "body.h"

//
//  Each node has a state
//
//    LEAF         : node with no subnodes
//    CONGLOMERATE : node is, from a physics standpoint, an average of all its children
//
enum NodeState {
    LEAF, CONGLOMERATE };


//
//  Barnes Hut Tree Node Class
//
//  Each node has it's own data
//    - This data is representative of the 4 children nodes
//
//  Then it has 4 children
//    - Northwest
//    - Northeast
//    - Southeast
//    - Southwest
//
//
//


class bh_tree_node {
protected:
    //double mass;
    std::shared_ptr<body> my_body;
    std::shared_ptr<bh_tree_node> nw, ne, se, sw, outside;

    region my_region;
    NodeState state;

public:
    // create populated node
    bh_tree_node(region r,
                 std::shared_ptr<body> my_body,
                 std::shared_ptr<bh_tree_node> nw = nullptr,
                 std::shared_ptr<bh_tree_node> ne = nullptr,
                 std::shared_ptr<bh_tree_node> se = nullptr,
                 std::shared_ptr<bh_tree_node> sw = nullptr)
            : my_region(r), my_body(my_body), nw(nw), ne(ne), se(se), sw(sw) { state = NodeState::LEAF; outside = nullptr; }

    virtual ~bh_tree_node() {
        this->clear();
        my_body = nullptr;
        nw = nullptr;
        ne = nullptr;
        se = nullptr;
        sw = nullptr;
        outside = nullptr;
    }

    void clear() {
        if (state == NodeState::LEAF) {
            my_body = nullptr;
            nw = nullptr;
            ne = nullptr;
            se = nullptr;
            sw = nullptr;
            outside = nullptr;
            return;
        }
        if (nw != nullptr) {
            nw->clear();
            nw = nullptr;
        }
        if (ne != nullptr) {
            ne->clear();
            ne = nullptr;
        }
        if (se != nullptr) {
            se->clear();
            se = nullptr;
        }
        if (sw != nullptr) {
            sw->clear();
            sw = nullptr;
        }
        if (outside != nullptr) {
            outside->clear();
            outside = nullptr;
        }
        my_body = nullptr;
        return;
    }

    //
    //  Currently a point object is destroyed here, I want to prevent that ...
    //
    region get_subregion_for_point(const point &p) {
        Quadrant q = my_region.get_quadrant(p);
        return my_region.create_subregion(q);
    }
    region get_subregion_for_body(const std::shared_ptr<body> &b) {
        region r = get_subregion_for_point(b->get_position());
        return r;
    }

    std::shared_ptr<bh_tree_node> get_nw() { return nw; }
    std::shared_ptr<bh_tree_node> get_ne() { return ne; }
    std::shared_ptr<bh_tree_node> get_se() { return se; }
    std::shared_ptr<bh_tree_node> get_sw() { return sw; }
    std::shared_ptr<bh_tree_node> get_outside() { return outside; }

    // Delete these?
    //    void set_nw(std::shared_ptr<bh_tree_node> node) { nw = node; }
    //    void set_ne(std::shared_ptr<bh_tree_node> node) { ne = node; }
    //    void set_se(std::shared_ptr<bh_tree_node> node) { se = node; }
    //    void set_sw(std::shared_ptr<bh_tree_node> node) { sw = node; }

    region get_region() const { return my_region; }

    Quadrant get_quadrant(const point &p) const { return my_region.get_quadrant(p); };
    //Quadrant get_quadrant(const body &b) const { return my_region.get_quadrant(b.get_position()); }

    NodeState get_state() const { return bh_tree_node::state; }

    bool is_leaf() { return this->state == NodeState::LEAF; }


    void add_node(std::shared_ptr<body> &b){
        //
        //  If LEAF
        //
        if (is_leaf()) {
            // todo: update this code to change the my_body and reset it since it is being moved to a subnode
            Quadrant leaf_body_quadrant = get_quadrant(my_body->get_position());
            Quadrant new_body_quadrant = get_quadrant(b->get_position());

            //
            // if the current node and is in the same quadrant as the new body
            //
            if (leaf_body_quadrant == new_body_quadrant) {
                region r_shared = get_subregion_for_body(my_body);
                std::shared_ptr<bh_tree_node> subnode = std::make_shared<bh_tree_node>(r_shared, my_body);
                set_node_in_quadrant(subnode, leaf_body_quadrant);
                subnode->add_node(b);
                my_body = std::make_shared<body>(0, point(0,0), point(0,0)); // reset it
                this->state = NodeState::CONGLOMERATE;
            } else {
                region r_old = get_subregion_for_body(my_body);
                region r_new = get_subregion_for_body(b);
                std::shared_ptr<bh_tree_node> node_for_old = std::make_shared<bh_tree_node>(r_old, my_body);
                std::shared_ptr<bh_tree_node> node_for_new = std::make_shared<bh_tree_node>(r_new, b);
                set_node_in_quadrant(node_for_old, leaf_body_quadrant);
                set_node_in_quadrant(node_for_new, new_body_quadrant);
                my_body = std::make_shared<body>(0, point(0,0), point(0,0)); // reset it
                this->state = NodeState::CONGLOMERATE;
            }

        } else {
            region r = this->get_subregion_for_body(b);
            std::shared_ptr<bh_tree_node> new_node = std::make_shared<bh_tree_node>(r, b);
            Quadrant q = this->get_region().get_quadrant(b->get_position());
            set_node_in_quadrant(new_node, q);
        }
    }

    void set_node_in_quadrant(const std::shared_ptr<bh_tree_node> &new_node, const Quadrant &q) {
        switch (q) {
            case NW:
                nw = new_node;
                break;
            case NE:
                ne = new_node;
                break;
            case SE:
                se = new_node;
                break;
            case SW:
                sw = new_node;
                break;
            case OUTSIDE:
                // todo: handle points outside of region
                outside = new_node;
                break;
        }
    }

    double get_mass() {
        if (state == NodeState::LEAF) {
            return my_body->get_mass();
        }

        double mass = 0.0;
        if (nw != nullptr) {
            mass += nw->my_body->get_mass();
        }
        if (ne != nullptr) {
            mass += ne->my_body->get_mass();
        }
        if (sw != nullptr) {
            mass += sw->my_body->get_mass();
        }
        if (se != nullptr) {
            mass += se->my_body->get_mass();
        }
        return mass;
    }

    point get_position() {
        if (state == NodeState::LEAF) {
            return my_body->get_position();
        }
        point position(0, 0);
        if (nw != nullptr) {
            point p = nw->my_body->get_position();
            p *= nw->get_mass();
            position += p;
        }
        if (ne != nullptr) {
            point p = ne->my_body->get_position();
            p *= ne->get_mass();
            position += p;
        }
        if (sw != nullptr) {
            point p = sw->my_body->get_position();
            p *= sw->get_mass();
            position += p;
        }
        if (se != nullptr) {
            point p = se->my_body->get_position();
            p *= se->get_mass();
            position += p;
        }
        position /= this->get_mass();
        return position;
    }

    // compute s/d < 0.5
    // s = width of region, d is distance
    point compute_force(std::shared_ptr<body> &b) {
        point force(0,0);

        // is far away?
        double G = 6.674e-11;
        double width = my_region.width();
        double height = my_region.height();
        double s = width > height ? width : height; // s is the max of width and height
        double d = distance(b->get_position(), this->my_body->get_position());
        double sd_ratio = s/d;
        double theta = 0.5;

        // if points are too close, they are considered to be the same point
        // later add code to handle collision physics
        double epsilon = 1.0e2;
        if (d < epsilon) {
            return force;
        }

        // force computation for leaf nodes
        if (this->state == NodeState::LEAF) {
            // get magnitude of force:
            double force_magnitude = G*b->get_mass() * my_body->get_mass() / (d*d);
            point force_direction = my_body->get_position() - b->get_position();
            force_direction /= d; // normalize the force direction
            force_direction *= force_magnitude;
            return force_direction;
        }

        // if s/d < theta the region is far enough away to compute force
        if (sd_ratio <= theta) {
            // Force magnitude and direction
            double force_magnitude = G*b->get_mass() * my_body->get_mass() / (d*d);
            point force_direction = my_body->get_position() - b->get_position();
            force_direction /= d; // normalize the force direction
            force_direction *= force_magnitude;
            return force_direction;
        }
        //
        //   if the s/d condition is not met, try again for sub-nodes
        //
        if (nw != nullptr) {
            force += nw->compute_force(b);
        }
        if (ne != nullptr) {
            force += ne->compute_force(b);
        }
        if (se != nullptr) {
            force += se->compute_force(b);
        }
        if (sw != nullptr) {
            force += sw->compute_force(b);
        }

        return force;
    }

    void update_body() {
        if (state == NodeState::LEAF) {
            return; // for leafs, do nothing
        }
        double mass = 0.0;
        point velocity(0,0);
        point position(0,0);

        if (nw != nullptr) {
            nw->update_body();

            double nw_mass = nw->get_mass();
            point nw_position = nw->get_position();

            mass += nw_mass;
            position += (nw_position *= nw_mass);
        }
        if (ne != nullptr) {
            ne->update_body();

            double ne_mass = ne->get_mass();
            point ne_position = ne->get_position();

            mass += ne->get_mass();
            position += (ne_position *= ne_mass);
        }
        if (sw != nullptr) {
            sw->update_body();

            double sw_mass = sw->get_mass();
            point sw_position = sw->get_position();

            mass += sw->get_mass();
            position += (sw_position *= sw_mass);
        }
        if (se != nullptr) {
            se->update_body();

            double se_mass = se->get_mass();
            point se_position = se->get_position();

            mass += se->get_mass();
            position += (se_position *= se_mass);
        }
        my_body->set_mass(mass);
        position /= mass;
        my_body->set_position(position);

    }

    std::ostream &to_stream(std::ostream &os, std::string buffer="") {
        os << std::endl << buffer << "Body: " << *my_body << " : " << my_region << std::endl;

        os << buffer << "nw: ";
        if (nw == nullptr) {
            os << "nullptr" << std::endl;
        } else {
            nw->to_stream(os, buffer + "  ");
        }
        os << buffer << "ne: ";
        if (ne == nullptr) {
            os << "nullptr" << std::endl;
        } else {
            ne->to_stream(os, buffer + "  ");
        }

        os << buffer << "se: ";
        if (se == nullptr) {
            os << "nullptr" << std::endl;
        } else {
            se->to_stream(os, buffer + "  ");
        }

        os << buffer << "sw: ";
        if (sw == nullptr) {
            os << "nullptr" << std::endl;
        } else {
            sw->to_stream(os, buffer + "  ");
        }

        return os;

    }

    // todo: fix the stream so it indents better for inner nodes
    friend std::ostream &operator<<(std::ostream &os, const bh_tree_node &node) {
        os << "\n" << "mass: " << *(node.my_body) << " : " << node.my_region << std::endl;
        os << "nw: ";
        if (node.nw == nullptr) {
            os << "nullptr" << std::endl;
        } else {
            os << *(node.nw);
        }
        os << "ne: ";
        if (node.ne == nullptr) {
            os << "nullptr" << std::endl;
        } else {
            os << *(node.ne);
        }

        os << "se: ";
        if (node.se == nullptr) {
            os << "nullptr" << std::endl;
        } else {
            os << *(node.se);
        }

        os << "sw: ";
        if (node.sw == nullptr) {
            os << "nullptr" << std::endl;
        } else {
            os << *(node.sw);
        }

        return os;
    }

    //friend class bh_tree;
};




#endif //TREE_CODE_BH_TREE_NODE_H
