#ifndef PID_H
#define PID_H

class PID {
private:
    float e0 = 0.0f;
    int tp0 = 0;
public:
    float kp, ki, kd;
    float i, r, y, c;
    float c_max, c_min, i_max, i_min;
    PID(float kp, float ki, float kd, float c_max, float c_min, float i_max, float i_min);
    void atualizar(float r, float y);
};

#endif