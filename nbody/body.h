//
// Created by Erik Sevre on 10/20/16.
//

#ifndef TREE_CODE_BODY_H
#define TREE_CODE_BODY_H


#include <ostream>
#include "point.h"

//
//  Simple object to hold all the information for a body
//
class body {
protected:
    double m_mass;  // in the future I may want to consider radius and other factors
    point m_velocity, m_position, m_last_position;

public:
    body(double mass, const point &position, const point &velocity)
            : m_mass(mass), m_position(position), m_velocity(velocity), m_last_position(point(0,0)) { }
    body() : m_mass(0), m_position(point(0,0)), m_velocity(point(0,0)), m_last_position(point(0,0)) { }


    //
    //  Getters and Setters for Mass, Position, Velocity, and Momentum (getter only)
    //
    //  Changes to Mass and Velocity will update Momentum
    //  return *this to allow funciton chaining
    //
    double get_mass() const { return m_mass; }
    body& set_mass(double m) { m_mass = m; return *this; }

    point get_position() const { return m_position; }
    body& set_position(const point &pos) { m_position = pos; return *this; }

    point get_last_position() const { return m_last_position; }

    point get_velocity() const { return m_velocity; }
    body& set_velocity(const point &vel) { m_velocity = vel; return *this; }

    point get_momentum() const {
        point momentum = m_velocity;
        momentum *= m_mass;
        return momentum;
    }

    void update_based_on_force_dt(point force, double dt) {
        // F = ma  --> F/m = a
        point accelleration = force / m_mass;
        m_last_position = m_position;
        m_velocity += (accelleration * dt);
        m_position += (m_velocity * dt);
    }

    friend std::ostream &operator<<(std::ostream &os, const body &body1) {
        os << "mass: " << body1.m_mass
           << " position: " << body1.m_position
           << " velocity: " << body1.m_velocity;
        return os;
    }


};


#endif //TREE_CODE_BODY_H
