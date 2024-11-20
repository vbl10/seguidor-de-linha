#include "Filtro.h"
#include "ArranjoCircular.h"
#include "PID.h"
#include "Encoder.h"

#define PIN_MOTOR_ESQ_A 10
#define PIN_MOTOR_ESQ_B 9
#define PIN_MOTOR_DIR_A 6
#define PIN_MOTOR_DIR_B 5

#define PIN_BUZINA 11

#define PIN_ULTRASSOM_ECHO 4
#define PIN_ULTRASSOM_TRIG 7

#define PIN_ENCODER_DIR 2
#define PIN_ENCODER_ESQ 3

#define PIN_LDR_ESQ A1
#define PIN_LDR_DIR A0

constexpr float distanciaObstaculoMinima_cm = 40.0f;

constexpr float raioRoda_cm = 3.2f;
constexpr float velMax_cmps = 10.0f;

constexpr float sensibilidadeDeteccaoLinhaEsq_un = 0.45f;
constexpr float sensibilidadeDeteccaoLinhaDir_un = 0.45f;

float distanciaObstaculo_cm = 0.0f;

bool avancar = false;
enum DIRECAO { ESQUERDA, MANTER, DIREITA };
DIRECAO direcao = DIRECAO::MANTER;

bool ldrEsqSobreLinha = false;
bool ldrDirSobreLinha = false;

Encoder<PIN_ENCODER_ESQ> encoderEsq(20);
Encoder<PIN_ENCODER_DIR> encoderDir(20);

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

    encoderEsq.inicializar();
    encoderDir.inicializar();
}

void loop() {
    tarefaSensorUltrassonico();
    tarefaSensoresLDR();
    tarefaControlarMotores();
    tarefaDegub();


    {
        static long tempoUltimoToque_ms = 0;
        if (distanciaObstaculo_cm < distanciaObstaculoMinima_cm) {
            if (millis() - tempoUltimoToque_ms >= 5000) {
                tempoUltimoToque_ms = millis();
                //tone(PIN_BUZINA, 440, 250);
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

    static Filtro<float, 10> filtro(distanciaObstaculo_cm);
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

static PID 
    pidVelEsq(0.2f, 4.0f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f), 
    pidVelDir(0.3f, 4.0f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f);

float refEsq = 0.8f, refDir = 0.8f, saidaEsq = 0.0f, saidaDir = 0.0f;

void tarefaControlarMotores() {
    static unsigned long tp0 = 0;
    
    if (millis() - tp0 >= 30) {
        tp0 += 30;

        saidaEsq = encoderEsq.pegarFrequencia_Hz();
        saidaDir = encoderDir.pegarFrequencia_Hz();

        pidVelDir.atualizar(
            direcao != DIRECAO::DIREITA && avancar 
                ? velMax_cmps / (2.0f * PI * raioRoda_cm) 
                : 0.0f, 
            saidaDir
        );
        pidVelEsq.atualizar(
            direcao != DIRECAO::ESQUERDA && avancar 
                ? velMax_cmps / (2.0f * PI * raioRoda_cm) 
                : 0.0f, 
            saidaEsq
        );

        defPotenciaMotorDir(
            direcao != DIRECAO::DIREITA && avancar 
                ? refDir
                : 0.0f);//pidVelDir.c);
        defPotenciaMotorEsq(
            direcao != DIRECAO::ESQUERDA && avancar 
                ? refEsq
                : 0.0f);//pidVelEsq.c);

        refEsq = refDir = sin((float)millis() / 3000.0f * 2.0f * PI) * 0.5f + 0.5f;
    }
}
void defPotenciaMotorDir(float pot_un) {
    static float ultimoPot = 0.0f;
    if (ultimoPot != pot_un) {
        if (pot_un >= 0.0f) {
            analogWrite(PIN_MOTOR_DIR_A, pot_un * 255.0f);
            digitalWrite(PIN_MOTOR_DIR_B, LOW);
        }
        else {
            analogWrite(PIN_MOTOR_DIR_B, pot_un * 255.0f);
            digitalWrite(PIN_MOTOR_DIR_A, LOW);
        }
        ultimoPot = pot_un;
    }
}
void defPotenciaMotorEsq(float pot_un) {
    static float ultimoPot = 0.0f;
    if (ultimoPot != pot_un) {
        if (pot_un >= 0.0f) {
            analogWrite(PIN_MOTOR_ESQ_A, pot_un * 255.0f);
            digitalWrite(PIN_MOTOR_ESQ_B, LOW);
        }
        else {
            analogWrite(PIN_MOTOR_ESQ_B, pot_un * 255.0f);
            digitalWrite(PIN_MOTOR_ESQ_A, LOW);
        }
        ultimoPot = pot_un;
    }
}


void tarefaDegub() {
    static long tempoUltimoPrint_ms = 0, tp0 = 0;
    
    if (millis() - tempoUltimoPrint_ms >= 30) {
        tempoUltimoPrint_ms += 30;
        
        //Serial.print(ldrEsq);
        //Serial.print("; ");
        //Serial.print(ldrEsqSobreLinha);
        //Serial.print("; ");
        //Serial.print(ldrDir);
        //Serial.print("; ");
        //Serial.print(ldrDirSobreLinha);
        //Serial.print("; ");

        //Serial.print(encoderEsq.pegarFrequencia_Hz());
        //Serial.print("; ");
        //Serial.print(pidVelEsq.r);
        //Serial.print("; ");
        //Serial.print(pidVelEsq.y);
        //Serial.print("; ");
        //Serial.print(pidVelEsq.c);
        //Serial.print("; ");

        //Serial.print(encoderDir.pegarFrequencia_Hz());
        //Serial.print("; ");
        //Serial.print(pidVelDir.r);
        //Serial.print("; ");
        //Serial.print(pidVelDir.y);
        //Serial.print("; ");
        //Serial.print(pidVelDir.c);
        //Serial.print("; ");
        
        Serial.print(refEsq);
        Serial.print(", ");
        Serial.print(saidaEsq);
        Serial.print(", ");
        Serial.print(refDir);
        Serial.print(", ");
        Serial.print(saidaDir);

        Serial.print(", ");
        Serial.print(millis() - tp0);
        tp0 = millis();
        Serial.println();
    }
}