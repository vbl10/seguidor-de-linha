#include "ControladorPID.h"
#include <Arduino.h>

ControladorPID::ControladorPID(float kp, float ki, float kd, float n, float ts)
    :kp(kp), ki(ki), kd(kd), n(n), ts(ts)
{
}

float ControladorPID::atualizar(float e) {
    i = i + ki*ts*e1;
    i = max(0.0f, min(i, 1.0f));        //para lidar com a saturação do atuador
    d = -(n*ts-1)*d + kd*n*(e - e1);
    u = kp*e + i + d;
    u = max(0.0f, min(1.0f, u));        //para lidar com a saturação do atuador
    e1 = e;
    return u;
}