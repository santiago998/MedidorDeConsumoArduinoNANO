# Medidor de Consumo Eléctrico con Arduino Nano

Sistema de medición de corriente basado en Arduino Nano, sensor SCT-013, pantalla OLED y control automático mediante relé. Desarrollado como proyecto intermodular de Mantenimiento Electrónico.

## Características

- Medición de corriente eficaz (RMS) hasta 30A
- Cálculo de potencia en tiempo real
- Pantalla OLED para visualización de datos
- Menú navegable con dos botones
- Medidor de consumo con cálculo de coste eléctrico
- Alerta visual por LEDs (sobrecarga > 1500W)
- Desconexión automática mediante relé (sobrecarga > 1000W)

## Componentes necesarios

| Componente | Cantidad |
|------------|----------|
| Arduino Nano | 1 |
| Sensor SCT-013-030 (30A) | 1 |
| Pantalla OLED 0.96" I2C | 1 |
| Módulo relé 5V | 1 |
| LM358 (amplificador operacional) | 1 |
| Diodos 1N4148 | 2 |
| Resistencias varias | 11 |
| Condensadores 100µF y 10µF | 2 |
| Pulsadores | 2 |
| LEDs | 2 |
| Transistor BC547 | 1 |
| Jack 3.5mm hembra | 1 |
| Potenciómetro 20K | 1 |
| Contactor 1x2 | 1 |

## Conexiones

| Componente | Pin Arduino |
|------------|-------------|
| Sensor SCT-013 | A0 |
| Relé | D23 |
| Botón SELECT | D7 |
| Botón ACCEPT | D8 |
| LED indicador | D6 |
| LED rojo 1 (alerta) | D9 |
| LED rojo 2 (alerta) | D10 |
| OLED SDA | A4 |
| OLED SCL | A5 |

## Instalación

1. Clona este repositorio o descarga el archivo `.ino`
2. Abre el código en Arduino IDE
3. Instala las siguientes librerías:
   - Wire.h
   - Adafruit_GFX.h
   - Adafruit_SSD1306.h
4. Conecta el Arduino Nano a tu ordenador
5. Selecciona la placa: `Arduino Nano`
6. Selecciona el puerto correspondiente
7. Sube el código

## Cómo usar el sistema

1. Al encender, el sistema se inicia y calibra el sensor automáticamente
2. Usa los botones para navegar por el menú:
   - **SELECT (D7)**: mover entre opciones / iniciar o detener medidor
   - **ACCEPT (D8)**: entrar en una opción o volver al menú
3. Opciones del menú:
   - **Info General**: muestra potencia, intensidad y tiempo activo
   - **Medidor Consumo**: calcula el coste eléctrico acumulado
4. El relé se activa automáticamente si la potencia supera los 1000W
5. Los LEDs rojos se encienden si la potencia supera los 1500W

## Ajuste del circuito

El sistema incluye un circuito acondicionador con LM358 y un potenciómetro de 20K. Para un correcto funcionamiento:

1. Conecta un osciloscopio a la salida del amplificador
2. Ajusta el potenciómetro hasta obtener una señal limpia, sin saturación
3. La señal debe estar centrada y sin recortes en los picos

## Pruebas realizadas

El sistema se probó con un ventilador de tres modos, comparando las mediciones con un medidor comercial:

| Modo | Medidor comercial | Prototipo |
|------|-------------------|-----------|
| Calor nivel 1 | 890 - 905 W | 850 - 880 W |
| Calor nivel 2 | 1700 - 1870 W | 1600 - 1750 W |

Las diferencias se deben a la naturaleza inductiva de la carga y al margen de error del sensor.

## Autores

- Cristian Santiago Rivera Guzman
- Hafssa Emam

## Fecha

Mayo 2026

## Licencia

Este proyecto es de uso educativo y está disponible para fines académicos.
