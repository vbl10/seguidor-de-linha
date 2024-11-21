#ifndef CONTROLADOR_PID_H
#define CONTROLADOR_PID_H

class ControladorPID
{
private:
    float u = 0.0f, i = 0.0f, d = 0.0f, e1 = 0.0f;
    float kp, ki, kd, n, ts;

public:
    // n: frequência de corte da malha derivativa em rad/s
    // ts: periodo de amostragem
    ControladorPID(float kp, float ki, float kd, float n, float ts);

    // e: erro
    float atualizar(float e);
};

#endif