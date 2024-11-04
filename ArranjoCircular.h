#ifndef ARRANJO_CIRCULAR_H
#define ARRANJO_CIRCULAR_H

template <typename T, int TAM>
class ArranjoCircular {
public:
    static constexpr int tam = TAM;
private:
    T arr[TAM] = { 0 };
    int inicio = 0;

public:
    void inserir(const T& valor) {
        arr[inicio] = valor;
        inicio = (inicio + 1) % TAM;
    }
    T& operator[](int n) {
        return arr[(inicio + n) % TAM];
    }
};

#endif