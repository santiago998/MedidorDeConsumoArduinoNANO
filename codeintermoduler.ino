// ===================================================
//  PROYECTO INTERMODULAR - VERSION FINAL CON ALERTA
// ===================================================
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== PANTALLA =====
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

// ===== PINES =====
#define BTN_ACCEPT 8
#define BTN_SELECT 7
#define LED_IND 6
#define LED_ROJO1 9
#define LED_ROJO2 10
#define RELE 23
#define PINZA 30

// ===== VARIABLES =====
float peff = 0;
int valor = 0;
int offset_medio = 512;
bool estadoRele = false;

// Menu
int opcionActual = 0;
int pantallaActual = 0;
int subPantalla = 0;

// Consumo
bool medidorActivo = false;
bool esperandoBoton = false;
double costoTotal = 0;
unsigned long tiempoInicioMedicion = 0;

// Variables para guardar el resumen
int ultimaHoras = 0;
int ultimosMinutos = 0;
int ultimosSegundos = 0;
float ultimaEnergia = 0;
float ultimoCostoMostrar = 0;

unsigned long tiempoInicio = 0;
unsigned long ultimoParpadeo = 0;
bool mostrarCursor = true;

// ===== CALIBRACIÓN =====
float K = 1.21 * 1.4;
float Vred = 230.0;

// ===================================================
// SETUP
// ===================================================
void setup() {
  Serial.begin(9600);

  pinMode(BTN_ACCEPT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(LED_IND, OUTPUT);
  pinMode(LED_ROJO1, OUTPUT);
  pinMode(LED_ROJO2, OUTPUT);
  pinMode(RELE, OUTPUT);
  digitalWrite(LED_IND, LOW);
  digitalWrite(LED_ROJO1, LOW);
  digitalWrite(LED_ROJO2, LOW);
  digitalWrite(RELE, LOW);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  tiempoInicio = millis();

  // Pantalla de inicio
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(20, 20);
  display.print("Iniciando");
  display.setCursor(15, 32);
  display.print("programa...");
  display.display();
  delay(1500);

  // Calibrar offset
  long suma = 0;
  for (int i = 0; i < 500; i++) {
    suma += analogRead(A0);
    delayMicroseconds(200);
  }
  offset_medio = suma / 500;
}

// ===================================================
// CALCULO POTENCIA
// ===================================================
float calcula_potencia(int num_valores) {
  int valor_max = 0;
  for (int i = 0; i < num_valores; i++) {
    valor = analogRead(A0);
    if (valor > valor_max) valor_max = valor;
    delayMicroseconds(200);
  }
  int amplitud_adc = valor_max - offset_medio;
  if (amplitud_adc < 3) return 0;
  float vp = (amplitud_adc * 5.0) / 1024.0;
  float ip = (vp * PINZA * K) / 5.0;
  float irms = ip / 1.4142;
  return irms * Vred;
}

// ===================================================
// ACTUALIZAR COSTO
// ===================================================
void actualizarCosto() {
  if (medidorActivo && tiempoInicioMedicion > 0) {
    unsigned long segundos = (millis() - tiempoInicioMedicion) / 1000;
    float horas = segundos / 3600.0;
    float potencia_kW = peff / 1000.0;
    costoTotal = potencia_kW * horas * 80;
  }
}

// ===================================================
// PANTALLA CONFIRMACION
// ===================================================
void mostrarConfirmacion(const char* mensaje) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 25);
  display.print(mensaje);
  display.display();
  delay(1000);
}

// ===================================================
// PANTALLA INFO GENERAL
// ===================================================
void mostrarInfoGeneral() {
  float corriente = peff / Vred;
  unsigned long segundos = (millis() - tiempoInicio) / 1000;
  int horas = segundos / 3600;
  int minutos = (segundos % 3600) / 60;
  int segs = segundos % 60;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (subPantalla == 0) {
    display.setTextSize(1);
    display.setCursor(10, 0);
    display.print("MEDIDOR POTENCIA");
    display.setTextSize(3);
    display.setCursor(15, 15);
    display.print(peff, 0);
    display.setTextSize(2);
    display.print("W");
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.print("I:");
    display.print(corriente * 100, 1);
    display.print("A  V:");
    display.print(Vred, 0);
  } else if (subPantalla == 1) {
    display.setTextSize(1);
    display.setCursor(20, 0);
    display.print("POTENCIA");
    display.drawLine(0, 8, 127, 8, SSD1306_WHITE);
    display.setTextSize(3);
    display.setCursor(15, 25);
    display.print(peff, 0);
    display.setTextSize(1);
    display.setCursor(95, 32);
    display.print("W");
  } else if (subPantalla == 2) {
    display.setTextSize(1);
    display.setCursor(20, 0);
    display.print("INTENSIDAD");
    display.drawLine(0, 8, 127, 8, SSD1306_WHITE);
    display.setTextSize(3);
    display.setCursor(25, 25);
    display.print(corriente, 2);
    display.setTextSize(1);
    display.setCursor(100, 32);
    display.print("A");
  } else if (subPantalla == 3) {
    display.setTextSize(1);
    display.setCursor(20, 0);
    display.print("TIEMPO ACTIVO");
    display.drawLine(0, 8, 127, 8, SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.print(horas);
    display.print("h ");
    display.print(minutos);
    display.print("m ");
    display.print(segs);
    display.print("s");
  }

  display.display();
}

// ===================================================
// PANTALLA MEDIDOR DE CONSUMO
// ===================================================
void mostrarMedidorConsumo() {
  actualizarCosto();

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(15, 0);
  display.print("MEDIDOR CONSUMO");
  display.drawLine(0, 8, 127, 8, SSD1306_WHITE);

  if (medidorActivo) {
    display.setTextSize(2);
    display.setCursor(10, 18);
    display.print(costoTotal, 4);
    display.setTextSize(1);
    display.print(" Eur");

    display.setTextSize(1);
    display.setCursor(10, 42);
    display.print("Potencia: ");
    display.print(peff, 0);
    display.print(" W");

    display.setCursor(10, 52);
    display.print("SELECT -> DETENER");
  } else {
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.print("INACTIVO");

    display.setTextSize(1);
    display.setCursor(10, 50);
    display.print("SELECT -> INICIAR");
  }

  display.display();
}

// ===================================================
// MENU PRINCIPAL
// ===================================================
void mostrarMenu() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.print("MENU PRINCIPAL");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  unsigned long ahora = millis();
  if (ahora - ultimoParpadeo > 300) {
    ultimoParpadeo = ahora;
    mostrarCursor = !mostrarCursor;
  }

  const char* opciones[] = {"Info General", "Medidor Consumo"};

  for (int i = 0; i < 2; i++) {
    if (i == opcionActual && pantallaActual == 0) {
      if (mostrarCursor) {
        display.setTextColor(SSD1306_WHITE);
      } else {
        display.setTextColor(SSD1306_BLACK);
      }
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    display.setCursor(10, 25 + (i * 15));
    display.print(opciones[i]);
  }

  display.display();
}

// ===================================================
// BOTONES
// ===================================================
void leer_botones() {
  // SELECT
  if (digitalRead(BTN_SELECT) == LOW) {
    digitalWrite(LED_IND, HIGH);

    if (pantallaActual == 0) {
      opcionActual++;
      if (opcionActual > 1) opcionActual = 0;
    } else if (pantallaActual == 1) {
      subPantalla++;
      if (subPantalla > 3) subPantalla = 0;
    } else if (pantallaActual == 2) {
      if (!medidorActivo) {
        // INICIAR medición
        medidorActivo = true;
        tiempoInicioMedicion = millis();
        costoTotal = 0;
        for (int i = 0; i < 3; i++) {
          digitalWrite(LED_IND, HIGH);
          delay(200);
          digitalWrite(LED_IND, LOW);
          delay(200);
        }
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(10, 25);
        display.print("MEDICION INICIADA");
        display.display();
        delay(1000);
      } else {
        // DETENER medición - solo guardar datos, no esperar aquí
        esperandoBoton = true;
        medidorActivo = false;
        actualizarCosto();
        
        // Guardar datos del resumen
        unsigned long segundos = (millis() - tiempoInicioMedicion) / 1000;
        ultimaHoras = segundos / 3600;
        ultimosMinutos = (segundos % 3600) / 60;
        ultimosSegundos = segundos % 60;
        ultimaEnergia = (peff / 1000.0) * (segundos / 3600.0);
        ultimoCostoMostrar = costoTotal;
      }
    }

    delay(200);
    digitalWrite(LED_IND, LOW);
  }

  // ACCEPT
  if (digitalRead(BTN_ACCEPT) == LOW) {
    digitalWrite(LED_IND, HIGH);

    if (pantallaActual == 0) {
      pantallaActual = opcionActual + 1;
      if (pantallaActual == 1) subPantalla = 0;
    } else {
      pantallaActual = 0;
    }

    delay(200);
    digitalWrite(LED_IND, LOW);
  }
}

// ===================================================
// RELE
// ===================================================
void control_rele(float potencia) {
  if (potencia > 1000) estadoRele = true;
  digitalWrite(RELE, estadoRele);
}

// ===================================================
// SERIAL
// ===================================================
void muestra_en_terminal() {
  Serial.print("Potencia: ");
  Serial.print(peff, 0);
  Serial.println(" W");
}

// ===================================================
// LOOP PRINCIPAL
// ===================================================
void loop() {
  float nueva_medida = calcula_potencia(300);
  peff = peff * 0.90 + nueva_medida * 0.10;

  if (peff > 1500) {
    digitalWrite(LED_ROJO1, HIGH);
    digitalWrite(LED_ROJO2, HIGH);
  } else {
    if (!esperandoBoton) {
      digitalWrite(LED_ROJO1, LOW);
      digitalWrite(LED_ROJO2, LOW);
    }
  }

  if (medidorActivo) actualizarCosto();

  delay(100);
  leer_botones();
  control_rele(peff);

  // ===== MODO RESUMEN =====
  if (esperandoBoton) {
    // Mostrar resumen
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(15, 0);
    display.print("RESUMEN FINAL");
    display.drawLine(0, 8, 127, 8, SSD1306_WHITE);
    
    display.setCursor(10, 20);
    display.print("Tiempo: ");
    display.print(ultimaHoras);
    display.print("h ");
    display.print(ultimosMinutos);
    display.print("m ");
    display.print(ultimosSegundos);
    display.print("s");
    
    display.setCursor(10, 35);
    display.print("Energia: ");
    display.print(ultimaEnergia, 3);
    display.print(" kWh");
    
    display.setCursor(10, 50);
    display.print("Total: ");
    if (ultimoCostoMostrar < 1.0) {
      display.print(ultimoCostoMostrar * 100, 1);
      display.print(" cent");
    } else {
      display.print(ultimoCostoMostrar, 2);
      display.print(" Eur");
    }
    display.display();
    
    // LED rojo parpadeante
    static unsigned long tiempoParpadeo = 0;
    static bool ledEstado = true;
    if (millis() - tiempoParpadeo > 500) {
      tiempoParpadeo = millis();
      ledEstado = !ledEstado;
      digitalWrite(LED_ROJO1, ledEstado);
      digitalWrite(LED_ROJO2, ledEstado);
    }
    
    // Esperar botón (sin bloquear completamente)
    if (digitalRead(BTN_SELECT) == LOW || digitalRead(BTN_ACCEPT) == LOW) {
      esperandoBoton = false;
      digitalWrite(LED_ROJO1, LOW);
      digitalWrite(LED_ROJO2, LOW);
      pantallaActual = 2;  // Volver al medidor de consumo
      delay(300);
    }
    return;  // Salir sin mostrar otras pantallas
  }
  // ========================

  if (pantallaActual == 0) {
    mostrarMenu();
  } else if (pantallaActual == 1) {
    mostrarInfoGeneral();
  } else if (pantallaActual == 2) {
    mostrarMedidorConsumo();
  }

  muestra_en_terminal();
  delay(80);
}