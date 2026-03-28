# 🤖 Robô Seguidor de Linha com Controle PID

## 📌 Descrição do Projeto
Este projeto foi desenvolvido no contexto da disciplina de **Robótica** do curso de **Engenharia da Computação**. O objetivo principal é a implementação de um **robô seguidor de linha autônomo**, capaz de detectar e seguir uma trajetória pré-definida, além de evitar colisões com obstáculos.

O sistema integra sensores, atuadores e técnicas de **controle digital**, garantindo precisão no deslocamento e estabilidade no seguimento da linha.

---

## 🎯 Objetivos
- Desenvolver um robô móvel capaz de seguir uma linha com precisão
- Implementar controle de velocidade utilizando **controle PID digital**
- Aplicar conceitos de **transformada Z** e **equações de diferenças**
- Realizar identificação de sistema e ajuste de parâmetros
- Integrar sensores para navegação e prevenção de colisões

---

## ⚙️ Componentes Utilizados

### Sensores
- **LDR (Light Dependent Resistor)**  
  Utilizados para detectar a presença da linha no solo com base na variação de luminosidade.

- **Sensor Ultrassônico**  
  Responsável pela detecção de obstáculos e prevenção de colisões.
  
- **Encoder**  
  Responsável pela contagem de rotações por minuto (RPM) das rodas.

### Atuadores
- Motores DC com controle de velocidade via modulação PWM

### Plataforma de Controle
- Microcontrolador (ex: Arduino ou equivalente)

---

## 🧠 Controle e Modelagem

O sistema de controle foi projetado com base em técnicas de **controle digital**, incluindo:

- **Controlador PID (Proporcional-Integral-Derivativo)**  
  Utilizado para ajustar continuamente a velocidade dos motores com base no erro de trajetória.

- **Transformada Z**  
  Aplicada para análise e projeto de sistemas discretos.

- **Equações de Diferenças**  
  Utilizadas para implementação do controlador em tempo discreto no microcontrolador.

---

## 🧪 Simulação e Identificação de Sistema

Foram utilizadas ferramentas computacionais para análise e validação do sistema:

- **MATLAB**
- **Simulink**

Essas ferramentas permitiram:
- Identificação do modelo dinâmico dos motores
- Simulação do comportamento do sistema
- Ajuste e otimização dos parâmetros do controlador PID

---

## 🔁 Funcionamento do Sistema

1. Os sensores LDR captam a diferença de luminosidade entre a linha e o fundo.
2. O erro de posição é calculado com base na leitura dos sensores.
3. O controlador PID processa o erro e gera sinais de controle.
4. Os motores são ajustados via PWM para corrigir a trajetória.
5. O sensor ultrassônico monitora a presença de obstáculos e interrompe ou ajusta o movimento quando necessário.

---

## 📊 Resultados Esperados
- Seguimento de linha suave e estável
- Correção rápida de desvios
- Redução de oscilações no movimento
- Capacidade de evitar colisões em tempo real

---

## 🚀 Possíveis Melhorias
- Implementação de controle adaptativo
- Uso de sensores infravermelhos de maior precisão
- Integração com visão computacional
- Mapeamento de ambiente e navegação autônoma avançada
