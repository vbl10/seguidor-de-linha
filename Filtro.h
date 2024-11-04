#ifndef FILTRO_H
#define FILTRO_H

template <typename T, int TAM>
class Filtro {
private:
    T amostras[TAM] = { 0 };    //amostras em ordem de chegada
    int ordem[TAM];             //indexadores de amostras em ordem crescente
    int k = 0;
    T& saida;

public:
    Filtro(T& saida): saida(saida) {
        for (int i = 0; i < TAM; i++) ordem[i] = i;
    }

    void inserirAmostra(T amostra) {
        amostras[k] = amostra;

        //ordenar amostras
        int i = 0;
        for (; i < TAM && ordem[i] != k; i++);
        for (; i + 1 < TAM && amostras[ordem[i]] > amostras[ordem[i + 1]]; i++) {
            int temp = ordem[i];
            ordem[i] = ordem[i + 1];
            ordem[i + 1] = temp;
        }
        for (; i - 1 >= 0 && amostras[ordem[i]] < amostras[ordem[i - 1]]; i--) {
            int temp = ordem[i];
            ordem[i] = ordem[i - 1];
            ordem[i - 1] = temp;
        }

        // extrair média dos valores medianos
        T media = 0;
        for (int i = TAM / 4, j = TAM -1 -i; i <= j; i++) {
            media += amostras[ordem[i]];
        }
        media /= (TAM + 1) / 2;
        saida = media;

        k = (k + 1) % TAM;
    }
};

#endif