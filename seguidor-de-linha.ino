#include "Filtro.h"
#include "ArranjoCircular.h"

#define PIN_MOTOR_DIR_A 9
#define PIN_MOTOR_DIR_B 10
#define PIN_MOTOR_ESQ_A 6
#define PIN_MOTOR_ESQ_B 5

#define PIN_BUZINA 11

#define PIN_ULTRASSOM_ECHO 4
#define PIN_ULTRASSOM_TRIG 3

#define PIN_LDR_DIR A0
#define PIN_LDR_ESQ A1

constexpr float distanciaObstaculoMinima_cm = 40.0f;

constexpr float velMax_un = 0.10f;

constexpr float sensibilidadeDeteccaoLinhaDir_un = 0.60f;
constexpr float sensibilidadeDeteccaoLinhaEsq_un = 0.45f;

float distanciaObstaculo_cm = 0.0f;

bool avancar = false;
enum DIRECAO { ESQUERDA, MANTER, DIREITA };
DIRECAO direcao = DIRECAO::MANTER;

bool ldrEsqSobreLinha = false;
bool ldrDirSobreLinha = false;

void setup() {
    Serial.begin(9600);

    pinMode(PIN_MOTOR_DIR_A, OUTPUT);
    pinMode(PIN_MOTOR_DIR_B, OUTPUT);
    pinMode(PIN_MOTOR_ESQ_A, OUTPUT);
    pinMode(PIN_MOTOR_ESQ_B, OUTPUT);

    pinMode(PIN_BUZINA, OUTPUT);

    pinMode(PIN_ULTRASSOM_ECHO, INPUT);
    pinMode(PIN_ULTRASSOM_TRIG, OUTPUT);

    pinMode(PIN_LDR_DIR, INPUT);
    pinMode(PIN_LDR_ESQ, INPUT);
}

void loop() {

    tarefaSensorUltrassonico();
    tarefaSensoresLDR();
    tarefaControlarMotores();
    //tarefaDegub();

    {
        static long tempoUltimoToque_ms = 0;
        if (distanciaObstaculo_cm < distanciaObstaculoMinima_cm) {
            if (millis() - tempoUltimoToque_ms >= 3000) {
                tempoUltimoToque_ms = millis();
                tone(PIN_BUZINA, 440, 250);
            }

            avancar = false;
        }
        else {
            tempoUltimoToque_ms = 0;
            avancar = true;
        }
    }

    if (ldrDirSobreLinha) {
        direcao = DIRECAO::DIREITA;
    }
    else if (ldrEsqSobreLinha) {
        direcao = DIRECAO::ESQUERDA;
    }
    else {
        direcao = DIRECAO::MANTER;
    }

}



void tarefaSensorUltrassonico() {
    static constexpr int periodoAmostragem_ms = 30;

    static Filtro<float, 20> filtro(distanciaObstaculo_cm);
    static long tempoUltimaAmostra_ms = 0;

    if (millis() - tempoUltimaAmostra_ms >= periodoAmostragem_ms) {
        tempoUltimaAmostra_ms = millis();
        filtro.inserirAmostra(medirDistancia_cm());
    }
}
float medirDistancia_cm() {
    digitalWrite(PIN_ULTRASSOM_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_ULTRASSOM_TRIG, LOW);
    float atrasoEcoUs = pulseIn(PIN_ULTRASSOM_ECHO, HIGH);
    return 0.017f * atrasoEcoUs;
}

float ldrDir = 0.0f, ldrEsq = 0.0f;
float deltaDir = 0.0f;
float deltaEsq = 0.0f;
void tarefaSensoresLDR() {

    ldrDir = (float)analogRead(PIN_LDR_DIR) / 1024.0f;
    ldrEsq = (float)analogRead(PIN_LDR_ESQ) / 1024.0f;

    ldrDirSobreLinha = ldrDir < sensibilidadeDeteccaoLinhaDir_un;
    ldrEsqSobreLinha = ldrEsq < sensibilidadeDeteccaoLinhaEsq_un;
}

static float velDir = 0.0f, velEsq = 0.0f;
void tarefaControlarMotores() {
    static unsigned long tempoUltimaChamada_us = 0;
    static float ultimoVelDir = 0.0f, ultimoVelEsq = 0.0f;
    
    static constexpr int curvaVelT1 = 300, curvaVelT2 = 450;

    static long tempoUltimoEstadoDir = 0;
    static bool ultimoEstadoDir = false;
    static long tempoUltimoEstadoEsq = 0;
    static bool ultimoEstadoEsq = false;

    if (direcao != DIRECAO::DIREITA && avancar) {
        if (!ultimoEstadoDir) {
            ultimoEstadoDir = true;
            tempoUltimoEstadoDir = millis();
        }
        if (millis() - tempoUltimoEstadoDir < curvaVelT1) {
            velDir = 1.0f;
        }
        else if (millis() - tempoUltimoEstadoDir < curvaVelT2) {
            float alpha = (float)(millis() - (tempoUltimoEstadoDir + curvaVelT1)) / (curvaVelT2 - curvaVelT1);
            velDir = (1.0f - alpha) * 1.0f + alpha * velMax_un;
        }
        else {
            velDir = velMax_un;
        }
    }
    else {
        velDir = 0.0f;
        ultimoEstadoDir = false;
    }

    if (direcao != DIRECAO::ESQUERDA && avancar) {
        if (!ultimoEstadoEsq) {
            ultimoEstadoEsq = true;
            tempoUltimoEstadoEsq = millis();
        }
        if (millis() - tempoUltimoEstadoEsq < curvaVelT1) {
            velEsq = 1.0f;
        }
        else if (millis() - tempoUltimoEstadoEsq < curvaVelT2) {
            float alpha = (float)(millis() - (tempoUltimoEstadoEsq + curvaVelT1)) / (curvaVelT2 - curvaVelT1);
            velEsq = (1.0f - alpha) * 1.0f + alpha * velMax_un;
        }
        else {
            velEsq = velMax_un;
        }
    }
    else {
        velEsq = 0.0f;
        ultimoEstadoEsq = false;
    }

    if (velDir != ultimoVelDir) {
        defVelMotorDireito(velDir);
        ultimoVelDir = velDir;
    }
    if (velEsq != ultimoVelEsq) {
        defVelMotorEsquerdo(velEsq);
        ultimoVelEsq = velEsq;
    }
}
void defVelMotorDireito(float vel_un) {
    analogWrite(PIN_MOTOR_DIR_A, vel_un * 255.0f);
    digitalWrite(PIN_MOTOR_DIR_B, LOW);
}
void defVelMotorEsquerdo(float vel_un) {
    analogWrite(PIN_MOTOR_ESQ_A, vel_un * 255.0f);
    digitalWrite(PIN_MOTOR_ESQ_B, LOW);
}


void tarefaDegub() {
    static long tempoUltimoPrint_ms = 0;

    if (millis() - tempoUltimoPrint_ms >= 100) {
        tempoUltimoPrint_ms = millis();

        Serial.print(ldrEsq);
        Serial.print("; ");
        Serial.print(ldrDir);
        Serial.print("; ");

        Serial.print(ldrEsqSobreLinha);
        Serial.print("; ");
        Serial.print(ldrDirSobreLinha);
        Serial.print("; ");

        Serial.print(avancar);
        Serial.print("; ");
        Serial.print(direcao);
        Serial.print("; ");
        
        Serial.print(velEsq);
        Serial.print("; ");
        Serial.println(velDir);
    }
}