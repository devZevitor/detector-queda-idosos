# Detector de Queda em Idosos

Sistema de detecção automática de quedas usando acelerômetro e giroscópio (MPU6050), desenvolvido com Arduino e simulado no Wokwi [Projeto](https://wokwi.com/projects/476139505681837057).

## O problema

Quedas são uma das principais causas de lesões graves e perda de independência entre idosos. O maior risco muitas vezes não é a queda em si, mas o tempo até alguém perceber que ela aconteceu e prestar socorro. Idosos que moram sozinhos frequentemente não conseguem pedir ajuda após cair, seja por dor, desorientação ou perda de consciência, o que torna sistemas de alarme manual (botão de pânico) insuficientes.

## A solução

Este projeto detecta a queda automaticamente, sem depender de nenhuma ação da vítima. Um sensor de movimento monitora continuamente a aceleração e a inclinação do corpo. Quando o padrão característico de uma queda é identificado (impacto brusco seguido de imobilidade), um alarme sonoro é acionado para chamar atenção de quem estiver por perto.

O projeto é baseado no artigo científico *"Exploring smartphone sensors for fall detection"* (Figueiredo et al., 2016), que demonstra que sensores acessíveis, como o acelerômetro, já são suficientes para detectar quedas com alta precisão e baixo custo computacional.

## Como funciona

1. O sensor MPU6050 mede continuamente a aceleração nos eixos X, Y e Z.
2. Os três eixos são combinados em um único valor de intensidade através da Norma Euclidiana.
3. Um pico de aceleração acima do limiar indica um possível impacto.
4. Se, logo em seguida, a aceleração cai para um nível de quase imobilidade, a queda é confirmada.
5. Se o corpo também estiver em uma posição anormal (inclinado), o buzzer é acionado.
6. Um botão físico permite desligar o alarme quando a queda for uma falsa detecção ou a pessoa confirmar que está bem.

## Componentes

- Arduino Uno
- Sensor MPU6050 (acelerômetro + giroscópio), comunicação via I2C
- Buzzer
- Botão (push button)

## Simulação

O projeto foi montado e testado no simulador Wokwi. [Link do projeto](https://wokwi.com/projects/476139505681837057)

## Referência

Figueiredo, I. N.; Leal, C.; Pinto, L.; Bolito, J.; Lemos, A. *Exploring smartphone sensors for fall detection*. mUX: The Journal of Mobile User Experience, v. 5, artigo 2, 2016. DOI: 10.1186/s13678-016-0004-1.
