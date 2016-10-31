//
// Created by Erik Sevre on 10/19/16.
//

#ifndef TREE_CODE_BH_TREE_NODE_H
#define TREE_CODE_BH_TREE_NODE_H

//
//  C++ STL Includes
//
#include <queue>
#include <stack>
#include <ostream>
#include <cmath>

//  Erik's Custom includes
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
//  Each node has a body object containing information necessary for the physics
//    - my_body contains the physical properties
//
//  Then it has 4 children
//    - Northwest
//    - Northeast
//    - Southeast
//    - Southwest
//    - OUTSIDE (not in any of the 4 nodes above)
//
//  NodeState is an enumeration used to determine if the body is a Leaf or Conglomerate of subnodes
//    - state : NodeState value to hold node type
//

class bh_tree_node {
protected:
    std::shared_ptr<body> my_body;
    std::shared_ptr<bh_tree_node> nw, ne, se, sw, outside;

    region my_region;
    NodeState state;

public:
    // Object constructor
    // Required input:
    //    region : r        -  region that this node represents
    //    body   : my_body  -  body that represents this region
    bh_tree_node(region r,
                 std::shared_ptr<body> my_body,
                 std::shared_ptr<bh_tree_node> nw = nullptr,
                 std::shared_ptr<bh_tree_node> ne = nullptr,
                 std::shared_ptr<bh_tree_node> se = nullptr,
                 std::shared_ptr<bh_tree_node> sw = nullptr)
            : my_region(r), my_body(my_body), nw(nw), ne(ne), se(se), sw(sw) { state = NodeState::LEAF; outside = nullptr; }

    // Object destructor
    // When a node is destroyed, we will clear out subnodes, and destroy the node
    // When we destroy a node, set each node to nullptr
    virtual ~bh_tree_node() {
        this->clear();
        my_body = nullptr;
        nw = nullptr;
        ne = nullptr;
        se = nullptr;
        sw = nullptr;
        outside = nullptr;
    }

    //
    //  Deletes all data in the node, and sub-nodes
    //
    void clear() {
        // if the node is a leaf, clear out the pointers
        if (state == NodeState::LEAF) {
            my_body = nullptr;
            nw = nullptr;
            ne = nullptr;
            se = nullptr;
            sw = nullptr;
            outside = nullptr;
            return;
        }
        // For Conglomerate nodes
        // If the quadrant is not null, then clear it
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
    //  Subregions are created and used to create subnodes based on the quadrant
    //
    //  There are two versions of the fucntion exist so it works with point or body
    // objects.
    //
    region get_subregion_for_point(const point &p) {
        Quadrant q = my_region.get_quadrant(p);
        return my_region.create_subregion(q);
    }
    region get_subregion_for_body(const std::shared_ptr<body> &b) {
        region r = get_subregion_for_point(b->get_position());
        return r;
    }

    // Getters: for retreiving pointers to sub-nodes;
    std::shared_ptr<bh_tree_node> get_nw() { return nw; }
    std::shared_ptr<bh_tree_node> get_ne() { return ne; }
    std::shared_ptr<bh_tree_node> get_se() { return se; }
    std::shared_ptr<bh_tree_node> get_sw() { return sw; }
    std::shared_ptr<bh_tree_node> get_outside() { return outside; }

    // Setters for setting subnodes, these aren't used at this time
    //    void set_nw(std::shared_ptr<bh_tree_node> node) { nw = node; }
    //    void set_ne(std::shared_ptr<bh_tree_node> node) { ne = node; }
    //    void set_se(std::shared_ptr<bh_tree_node> node) { se = node; }
    //    void set_sw(std::shared_ptr<bh_tree_node> node) { sw = node; }

    // getter for retreiving the region
    region get_region() const { return my_region; }

    // getter to get the quadrant for a point
    Quadrant get_quadrant(const point &p) const { return my_region.get_quadrant(p); };

    // Used to determine if the node is a leaf or conglomerate
    // not used at this time
    //
    //  get_state() will get the state for this node
    //  is_leaf() is a quick boolian check to see if the node is a leaf
    NodeState get_state() const { return bh_tree_node::state; }
    bool is_leaf() { return this->state == NodeState::LEAF; }


    // The heart of the program  to add a node
    void add_node(std::shared_ptr<body> &b){
        //
        //  If LEAF
        //
        //  In the case of adding to a leaf, we need to move the body to a subnode
        // before we add the new node.
        //  There are two cases,
        //    - current body and the new body are in the same quadrant
        //    - current body and the new body are in different quadrants
        //
        if (is_leaf()) {
            Quadrant leaf_body_quadrant = get_quadrant(my_body->get_position());
            Quadrant new_body_quadrant = get_quadrant(b->get_position());

            //
            // Add ing a node follows one of two paths, depending on if the two nodes are in
            //  the same quadrant or not. If the two are in the same quadrant, the insertion
            //  method is recursive, and will terminate when they are in separate quadrants.
            //    - create a region
            //    - use that region to create a sub-node with our current body
            //    - set our current sub-node to the appropriate quadrant
            //   -*- If the quadrant is the same
            //         use a recursive call to try inserting the new body
            //         recursion means this will keep dividing until the nodes
            //         are in different quadrants.
            //   -*- If the quadrant is the different
            //         set the new node in its quadrant.
            //    - reset the current body
            //    - set the state of this node to CONGLOMERATE
            //

            //
            //  If the current node is in teh same quadrant as the new body
            //
            if (leaf_body_quadrant == new_body_quadrant) {
                region r_shared = get_subregion_for_body(my_body);
                std::shared_ptr<bh_tree_node> subnode = std::make_shared<bh_tree_node>(r_shared, my_body);
                set_node_in_quadrant(subnode, leaf_body_quadrant);
                subnode->add_node(b);    // recursive call
                my_body = std::make_shared<body>(0, point(0,0), point(0,0)); // reset it
                this->state = NodeState::CONGLOMERATE;
            } else {
                //
                //  If the current node is in a different quadrant than the new body
                //
                region r_old = get_subregion_for_body(my_body);
                region r_new = get_subregion_for_body(b);
                std::shared_ptr<bh_tree_node> node_for_old = std::make_shared<bh_tree_node>(r_old, my_body);
                std::shared_ptr<bh_tree_node> node_for_new = std::make_shared<bh_tree_node>(r_new, b);
                set_node_in_quadrant(node_for_old, leaf_body_quadrant);
                set_node_in_quadrant(node_for_new, new_body_quadrant);
                my_body = std::make_shared<body>(0, point(0,0), point(0,0)); // reset it
                this->state = NodeState::CONGLOMERATE;
            }
        //
        //  If Conglomerate node, we can simply set the body to the correct quadrant
        //
        } else {
            region r = this->get_subregion_for_body(b);
            std::shared_ptr<bh_tree_node> new_node = std::make_shared<bh_tree_node>(r, b);
            Quadrant q = this->get_region().get_quadrant(b->get_position());
            set_node_in_quadrant(new_node, q);
        }
    }

    //
    //  Place node in the selected quadrant
    //
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
        return my_body->get_mass();
    }

    point get_position() {
        return my_body->get_position();
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
        // todo: add colision mechanics here
        double epsilon = 2.0e1;
        if (d < epsilon) {
            return force;
        }

        // Compute the force on the body if this node is a leaf,
        // or the sd ratio for this node and the body are less than our theta
        //
        if (this->state == NodeState::LEAF or sd_ratio <= theta) {
            // Force magnitude and direction
            double force_magnitude = G*b->get_mass() * my_body->get_mass() / (d*d);
            point force_direction = my_body->get_position() - b->get_position();
            force_direction /= d; // normalize the force direction
            force_direction *= force_magnitude;
            return force_direction;
        }
        //
        //   if the s/d condition is not met, try again for sub-nodes
        //     (this will terminate when it reaches a leaf node)
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

    //
    // Updates the masses and positions for body nodes
    //
    //  This calls recursively through the subnodes
    //
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
