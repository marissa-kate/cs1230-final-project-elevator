#include "rigidbody.h"

namespace Phys{
void State_to_Array(RigidBody* rb, double* y)
{
    // position
    *y++ = rb->x.x;
    *y++ = rb->x.y;
    *y++ = rb->x.z;

    // orientation quaternion
    *y++ = rb->q.w;
    *y++ = rb->q.x;
    *y++ = rb->q.y;
    *y++ = rb->q.z;

    // linear momentum
    *y++ = rb->P.x;
    *y++ = rb->P.y;
    *y++ = rb->P.z;

    // angular momentum
    *y++ = rb->L.x;
    *y++ = rb->L.y;
    *y++ = rb->L.z;
}


void Array_to_State(RigidBody* rb, double* y)
{
    rb->x = glm::vec3(y[0], y[1], y[2]);

    rb->q = glm::quat(y[3], y[4], y[5], y[6]);
    rb->q = glm::normalize(rb->q);

    rb->P = glm::vec3(y[7], y[8], y[9]);
    rb->L = glm::vec3(y[10], y[11], y[12]);

    rb->v = rb->P * rb->invMass;

    // Step 4 (always needed):
    rb->R = glm::mat3_cast(rb->q);
    rb->I_inv = rb->R * rb->I_body_inv * glm::transpose(rb->R);

    rb->w = rb->I_inv * rb->L;
}


void Array_to_Bodies(double y[ ], RigidBody Bodies[], int n_bodies)
{
    for(int i = 0; i < n_bodies; i++)
        Array_to_State(&Bodies[i], &y[i * STATE_SIZE]);
}
void Bodies_to_Array(double y[ ],  float NBODIES, RigidBody Bodies[]){
    for(int i = 0; i < NBODIES; i++)
        State_to_Array(&Bodies[i], &y[i * STATE_SIZE]);
}

void Compute_Force(double t, RigidBody *rb){
    //computes the force F(t) and torque τ(t) acting on the rigid body *rb at time t, and stores F(t)
    //and τ(t) in rb->force and rb->torque respectively. C
}

void ddt_State_to_Array(RigidBody* rb, double* ydot)
{
    // d/dt position = velocity
    *ydot++ = rb->v.x;
    *ydot++ = rb->v.y;
    *ydot++ = rb->v.z;

    // d/dt orientation = 0.5 * w * q
    glm::quat wq(0, rb->w.x, rb->w.y, rb->w.z);
    glm::quat dq = 0.5f * wq * rb->q;

    *ydot++ = dq.w;
    *ydot++ = dq.x;
    *ydot++ = dq.y;
    *ydot++ = dq.z;

    // d/dt linear momentum = force
    *ydot++ = rb->force.x;
    *ydot++ = rb->force.y;
    *ydot++ = rb->force.z;

    // d/dt angular momentum = torque
    *ydot++ = rb->torque.x;
    *ydot++ = rb->torque.y;
    *ydot++ = rb->torque.z;
}


void dydt(double t, double y[ ], double ydot[ ], float NBODIES, RigidBody *rb)
{
    /* put data in y[ ] into Bodies[ ] */
    Array_to_Bodies(y, rb, NBODIES);
    for(int i = 0; i < NBODIES; i++)
    {
        Compute_Force(t, &rb[i]);
        ddt_State_to_Array(&rb[i],
                           &ydot[i * STATE_SIZE]);
    }
}
//for a system of n particles, len = 6n
//t0 = start timed
//t1 = end time

//goal is to compute the state vector and return it at t1
void ode(double y0[ ], double yend[ ], int len, double t0,
         double t1, float NBODIES, RigidBody *rb,float deltaTime){
    float n = NBODIES;
    double h = deltaTime;         // step size
    double *k1 = new double[len];
    double *k2 = new double[len];
    double *k3 = new double[len];
    double *k4 = new double[len];
    double *ytmp = new double[len];

    // -----------------------------
    // k1 = f(t, y)
    // -----------------------------
    Phys::dydt(t0, y0, k1, n, rb);

    // -----------------------------
    // k2 = f(t + h/2, y + h*k1/2)
    // -----------------------------
    for (int i = 0; i < len; i++){
        ytmp[i] = y0[i] + 0.5 * h * k1[i];
        Phys::dydt(t0 + 0.5 * h, ytmp, k2, n, rb);
    }
    // -----------------------------
    // k3 = f(t + h/2, y + h*k2/2)
    // -----------------------------
    for (int i = 0; i < len; i++)
        ytmp[i] = y0[i] + 0.5 * h * k2[i];
    dydt(t0 + 0.5 * h, ytmp, k3, n , rb);

    // -----------------------------
    // k4 = f(t + h, y + h*k3)
    // -----------------------------
    for (int i = 0; i < len; i++)
        ytmp[i] = y0[i] + h * k3[i];
    dydt(t0 + h, ytmp, k4, n, rb);

    // -----------------------------
    // final RK4 combination
    // yend = y0 + h/6 * (k1 + 2k2 + 2k3 + k4)
    // -----------------------------
    for (int i = 0; i < len; i++)
        yend[i] = y0[i] + (h/6.0) * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);

    delete[] k1;
    delete[] k2;
    delete[] k3;
    delete[] k4;
    delete[] ytmp;

};
}
