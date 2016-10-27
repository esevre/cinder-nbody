//
// Created by Erik Sevre on 10/19/16.
//

#ifndef TREE_CODE_BH_TREE_H
#define TREE_CODE_BH_TREE_H

#include <ostream>
#include "bh_tree_node.h"
#include "body.h"

class bh_tree {
private:
    //double grid_xmax, grid_ymax, grid_xmin, grid_ymin;
    region global_region;
protected:
    std::shared_ptr<bh_tree_node> root;
public:
    bh_tree() : global_region(region(0,0,0,0)), root(nullptr) { }
    bh_tree(region g_region) : global_region(g_region) {
        root = nullptr;
    }
    ~bh_tree() { clear(); }
    void clear() {
        //clear(root);
        if (root != nullptr) { root->clear(); }
        root = nullptr;
    }
    bool is_empty() const {
        return root == nullptr;
    }

    void set_region(region r) { global_region = r; }
    //
    //
    //  todo: Add test function to check that this is inserting correctly
    //  todo: Add test function to ensure that leaves contain ACTUAL data, while Conglomerates contain an average of leaves
    //
    //
    // This is a tree of shared pointers, so the added body must be a shared pointer
    void insert_body(std::shared_ptr<body> &b) {
        //    ****    ****    ****    ****    ****    ****    ****
        //    CREATE ROOT NODE (if it doesn't exist)
        //    ****    ****    ****    ****    ****    ****    ****
        if (root == nullptr) {
            root = std::make_shared<bh_tree_node>(global_region, b);
            return; // If the first node was root, we add it to the root and stop
        }

        if (!root->get_region().is_in(b->get_position())) {
            // don't add this body
        }
        //
        //  Most of the time the root will exist so we can do our normal code to add a node
        //
        std::shared_ptr<bh_tree_node> current_node = root, prev = nullptr;
        //
        //  Find our way to the leaf we belong in
        //
        while (current_node != nullptr) { // find a place to insert the node
            prev = current_node;
            Quadrant q = current_node->get_quadrant(b->get_position());
            switch (q) {
                case Quadrant::NW:
                    current_node = current_node->get_nw();
                    break;
                case Quadrant::NE:
                    current_node = current_node->get_ne();
                    break;
                case Quadrant::SE:
                    current_node = current_node->get_se();
                    break;
                case Quadrant::SW:
                    current_node = current_node->get_sw();
                    break;
                default:
                    current_node = current_node->get_outside();
                    std::cout << "this shouldn't happen (insert_body)" << std::endl;
            }
        }
        prev->add_node(b);
        root->update_body();
    }

    bool is_outside(const std::shared_ptr<body> &b) const {
        return !global_region.is_in(b->get_position());
    }

    bool is_outside(const point pos) const {
        return !global_region.is_in(pos);
    }

    region get_global_region() const { return global_region; }

    point compute_force(std::shared_ptr<body> &b) {
        if (root == nullptr) {
            // no bodies for force computation
            return point(0, 0);
        }

        return root->compute_force(b);
    }

    friend std::ostream &operator<<(std::ostream &os, const bh_tree &tree) {
        os << "root: " << tree.global_region << " : ";
        if (tree.root == nullptr) {
            os << "nullptr" << std::endl;
        } else {
            tree.root->to_stream(os, "  ");
        }
        return os;
    }


};




#endif //TREE_CODE_BH_TREE_H
