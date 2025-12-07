#pragma once
#define PHYS_H
#include <vector>
#include <glm/gtc/quaternion.hpp>
static const int STATE_SIZE = 13;
#include <glm/glm.hpp>

struct RigidBody{
    float mass;
    float invMass;
    glm::mat3 I_body;        // inertia tensor in body space
    glm::mat3 I_body_inv;    // inverse inertia tensor in body space
    glm::vec3 scale;

    // State
    glm::vec3 x;             // position
    glm::quat q;             // orientation
    glm::vec3 P;             // linear momentum
    glm::vec3 L;             // angular momentum

    // Derived
    glm::vec3 v;             // linear velocity
    glm::vec3 w;             // angular velocity (world)

    glm::mat3 R;             // orientation matrix
    glm::mat3 I_inv;         // world-space inverse inertia

    // Accumulators
    glm::vec3 force;
    glm::vec3 torque;

    glm::vec3 inputForce  = glm::vec3(0.0f);
    glm::vec3 inputTorque = glm::vec3(0.0f);
};


namespace Phys {

// Convert 1 rigid body → array
void State_to_Array(RigidBody* rb, double* y);

// Convert array → 1 rigid body
void Array_to_State(RigidBody* rb, const double* y);

// Convert array → bodies
void Array_to_Bodies(const std::vector<double>& y,
                     RigidBody Bodies[],
                     int n_bodies);

// Convert bodies → array
void Bodies_to_Array(std::vector<double>& y,
                     int n_bodies,
                     RigidBody Bodies[]);

// Compute forces + torques for a body
void Compute_Force(double t, RigidBody* rb);

// Convert 1 rigid body's derivative to array
void ddt_State_to_Array(RigidBody* rb, double* ydot);

// dy/dt for whole system: output ydot
void dydt(double t,
          const std::vector<double>& y,
          std::vector<double>& ydot,
          int NBODIES,
          RigidBody* rb);

// RK4 integrator
void ode(const std::vector<double>& y0,
         std::vector<double>& yEnd,
         int len,
         double t0,
         double t1,
         int NBODIES,
         RigidBody* rb,
         double deltaTime);

} // namespace Phys
