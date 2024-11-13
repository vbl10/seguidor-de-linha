#include "PID.h"
#include "Arduino.h"

PID::PID(float kp, float ki, float kd, float c_max, float c_min, float i_max, float i_min)
    : kp(kp), ki(ki), kd(kd), c_max(c_max), c_min(c_min), i_max(i_max), i_min(i_min) {
}

void PID::atualizar(float r, float y) {
    this->r = r;
    this->y = y;


    int tp1 = micros() % (1 << (sizeof(tp0) * 8 - 1));
    if (tp1 < tp0) {
        tp0 -= 1 << (sizeof(tp0) * 8 - 1);
    }
    float dt = float(tp1 - tp0) / 1000000.0f;
    tp0 = tp1;


    float e1 = e0;
    float novo_e = r - y;
    if (novo_e == novo_e) e1 = novo_e;
    
    float novo_i = i + e1 * dt;
    if (novo_i == novo_i) i = novo_i;
    i = max(i_min, min(i_max, i));
    
    c = kp * e1 + ki * i + kd * (e1 - e0);
    c = max(c_min, min(i_max, c));
    
    e0 = e1;
}