#ifndef ENCODER_OTICO_H
#define ENCODER_OTICO_H
#include "Arduino.h"
#include "ArranjoCircular.h"

template <char PIN_ENCODER>
class Encoder {
private:
    static constexpr int TAM_FILTRO = 2;
    static ArranjoCircular<unsigned long, TAM_FILTRO> tempos;
    static unsigned char nFrestas;
public:
    Encoder(unsigned char nFrestas) {
        this->nFrestas = nFrestas;
        for (int i = 0; i < TAM_FILTRO; i++) {
            tempos[i] = 100000 * i;
        }
    }
    static void inicializar() {
        attachInterrupt(digitalPinToInterrupt(PIN_ENCODER), interrupt, RISING);
    }
    static float pegarPeriodo_s() {
        float num = 0.0f;
        float den = tempos[-1] - tempos[0];
        for (int i = 1; i < TAM_FILTRO; i++) {
            unsigned long dt = tempos[-i] - tempos[-i - 1];
            num += dt * dt;
        }
        unsigned long dt = millis() - tempos[-1];

        num += (float)dt * (float)dt;
        den += (float)dt;

        return num / den * (float)nFrestas / 1000.0f;
    }
    static float pegarFrequencia_Hz() {
        return 1.0f / pegarPeriodo_s();
    }
public:
    static void interrupt() {
        static constexpr int freqMinimaDetectavel_Hz = 0.05f;   //frequências abaixo desta reiniciam o descarte de amostras
        static constexpr int nMaxDiscartes = 2;                 //discartar primeiras 2 amostras (geralmente ruins)
        static constexpr float fMaxFreqDeltaRazao = 10.0f;      //máxima razão de frequência entre duas amostras

        static unsigned long tp0 = 0, tp1 = 0;
        static char contDiscartes = 0;
        
        unsigned long tp2 = millis();
        if (tp2 - tp1 > 1000.0f / (freqMinimaDetectavel_Hz * nFrestas)) {
            contDiscartes = 0;
        }
        if (contDiscartes < nMaxDiscartes) {
            contDiscartes++;
        }
        else if (float(tp1 - tp0) / float(tp2 - tp1) < fMaxFreqDeltaRazao) {
            tempos.inserir(tp2);
        }
        tp0 = tp1;
        tp1 = tp2;
    }
};

template <char PIN_ENCODER>
ArranjoCircular<unsigned long, 2> Encoder<PIN_ENCODER>::tempos;

template <char PIN_ENCODER>
unsigned char Encoder<PIN_ENCODER>::nFrestas;

#endif