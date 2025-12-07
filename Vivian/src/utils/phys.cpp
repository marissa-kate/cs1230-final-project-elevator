#include "phys.h"
namespace Phys {

/**
 * @brief State_to_Array convert from state to array
 * @param rb
 * @param y
 */
void State_to_Array(RigidBody* rb, double* y)
{
    // position
    *y++ = rb->x.x;
    *y++ = rb->x.y;
    *y++ = rb->x.z;

    // quaternion
    *y++ = rb->q.w;
    *y++ = rb->q.x;
    *y++ = rb->q.y;
    *y++ = rb->q.z;

    // momenta
    *y++ = rb->P.x;
    *y++ = rb->P.y;
    *y++ = rb->P.z;

    *y++ = rb->L.x;
    *y++ = rb->L.y;
    *y++ = rb->L.z;
}

/**
 * @brief Array_to_State Convert from array to state
 * @param rb
 * @param y
 */
void Array_to_State(RigidBody* rb, const double* y)
{
    rb->x = glm::vec3(y[0], y[1], y[2]);
    rb->q = glm::normalize(glm::quat(y[3], y[4], y[5], y[6]));
    rb->P = glm::vec3(y[7], y[8], y[9]);
    rb->L = glm::vec3(y[10], y[11], y[12]);

    // derived quantities
    rb->v = rb->P * rb->invMass;
    rb->R = glm::mat3_cast(rb->q);
    rb->I_inv = rb->R * rb->I_body_inv * glm::transpose(rb->R);
    rb->w = rb->I_inv * rb->L;
}

/**
 * @brief Array_to_Bodies
 * @param y
 * @param Bodies
 * @param n_bodies
 */
void Array_to_Bodies(const std::vector<double>& y, RigidBody Bodies[], int n_bodies)
{
    for(int i = 0; i < n_bodies; i++)
        Array_to_State(&Bodies[i], &y[i * STATE_SIZE]);
}

/**
 * @brief Bodies_to_Array
 * @param y
 * @param n_bodies
 * @param Bodies
 */
void Bodies_to_Array(std::vector<double>& y, int n_bodies, RigidBody Bodies[])
{
    for(int i = 0; i < n_bodies; i++)
        State_to_Array(&Bodies[i], &y[i * STATE_SIZE]);
}


void ddt_State_to_Array(RigidBody* rb, double* ydot)
{
    // dx/dt = v
    *ydot++ = rb->v.x;
    *ydot++ = rb->v.y;
    *ydot++ = rb->v.z;

    // dq/dt = 0.5 w ⊗ q
    glm::quat wq(0, rb->w.x, rb->w.y, rb->w.z);
    glm::quat dq = 0.5f * wq * rb->q;

    *ydot++ = dq.w;
    *ydot++ = dq.x;
    *ydot++ = dq.y;
    *ydot++ = dq.z;

    // dP/dt = F
    *ydot++ = rb->force.x;
    *ydot++ = rb->force.y;
    *ydot++ = rb->force.z;

    // dL/dt = τ
    *ydot++ = rb->torque.x;
    *ydot++ = rb->torque.y;
    *ydot++ = rb->torque.z;
}

/**
 * @brief dydt calculate forces
 * @param t
 * @param y
 * @param ydot
 * @param NBODIES
 * @param rb
 */
void dydt(double t,
          const std::vector<double>& y,
          std::vector<double>& ydot,
          int NBODIES,
          RigidBody* rb)
{
    Array_to_Bodies(y, rb, NBODIES);
    for(int i = 0; i < NBODIES; i++)
    {
        Compute_Force(t, &rb[i]);
        ddt_State_to_Array(&rb[i], &ydot[i * STATE_SIZE]);
    }

}


/**
 * @brief ode RK4 Integrator
 * @param y0
 * @param yEnd
 * @param len
 * @param t0
 * @param t1
 * @param NBODIES
 * @param rb
 * @param dt
 */
void ode(const std::vector<double>& y0,
         std::vector<double>& yFinal,
         int len,
         double t0,
         double t1,
         int NBODIES,
         RigidBody* rb,
         double dt)
{
    const double h = dt;  // step size

    std::vector<double> k1(len), k2(len), k3(len), k4(len), yTemp(len);

    // k1 = f(t, y)
    dydt(t0, y0, k1, NBODIES, rb);

    // k2
    for (int i = 0; i < len; ++i)
        yTemp[i] = y0[i] + 0.5 * h * k1[i];
    dydt(t0 + 0.5*h, yTemp, k2, NBODIES, rb);

    // k3
    for (int i = 0; i < len; ++i)
        yTemp[i] = y0[i] + 0.5 * h * k2[i];
    dydt(t0 + 0.5*h, yTemp, k3, NBODIES, rb);

    // k4
    for (int i = 0; i < len; ++i)
        yTemp[i] = y0[i] + h * k3[i];
    dydt(t0 + h, yTemp, k4, NBODIES, rb);

    // Output
    for (int i = 0; i < len; ++i)
        yFinal[i] = y0[i] + (h/6.0) * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
}
/**
 * @brief Compute_Force
 * @param t
 * @param rb
 */
void Compute_Force(double t, RigidBody* rb)
{
    const float thrust = 10.0f;
    const float spin   = 8.0f;
    const float drag   = 0.4f;
    const float angDrag = 0.25f;

    // force = input - damping
    rb->force =
        rb->inputForce * thrust
        - rb->v * drag;

    // torque = input - damping
    rb->torque =
        rb->inputTorque * spin
        - rb->w * angDrag;
}


} // namespace Phys
