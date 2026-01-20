#include <Wire.h>  // Comes with Arduino IDE
#include <PID_v1.h>
// =============================
// CONFIGURACIÓN DE PINES LCD
// =============================
#define LCD_RS 8
#define LCD_EN 9
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

// =============================
// CONFIGURACIÓN DE BOTONES
// =============================
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5
#define sbi(port, bit) (port) |= (1 << (bit))
#define cbi(port, bit) (port) &= ~(1 << (bit))

// =============================
// CONFIGURACIÓN DE PANTALLAS
// =============================
#define pDISPLAY 0
#define pCOMANDOS 1

// =============================
// CONFIGURACIÓN DE DISPLAY
// =============================
#define dAPAGADO 0
#define dENCENDIDO 1
#define dMANUAL 2
#define dTERMOSTATO 3
#define dPOTENCIOMETRO 4
#define sizeDISPLAY 5

// =============================
// CONFIGURACIÓN DE COMANDOS
// =============================
#define cTEMPERATURA 0
#define cENCENDER 1
#define cMANUAL 2
#define cTERMOSTATO 3
#define cAPAGAR 4
#define cPARAMETROS 5
#define cRESET 6
#define cPOTENCIOMETRO 7
#define sizeCOMANDOS 8

// =============================
// CONFIGURACIÓN AGENTE (BOMBILLA)
// =============================
#define PIN_ZCD 3  // Pin donde conectamos el detector de paso por cero
#define PIN_TRIAC 2
float potencia_agente = 0;  // Variable global para la potencia (0-100)

// =============================
// VARIABLES DE LCD
// =============================
int cursor_col = 0;
int cursor_row = 0;

// =============================
// VARIABLES DEL LECTOR DE TECLAS
// =============================

int v1 = 0;
int v2 = 132;
int v3 = 310;
int v4 = 481;
int v5 = 720;
int v6 = 1023;
int valor;
int fi;

// =============================
// VARIABLES PID (LIBRERÍA)
// =============================
double Setpoint, Input, Output;  // Variables para la librería
// Tuning inicial (Ajustables desde el menú)
double Kp = 30.0;
double Ki = 0.5;  // Nota: La librería maneja Ki diferente a la función manual, quizás necesites reajustar
double Kd = 2.0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// =============================
// VARIABLES GENERALES
// =============================

float temperatura = 0.0;
float temperatura_objetivo = 25.0;  // Temperatura que queremos alcanzar
static unsigned long last_slow_task = 0;
static unsigned long last_temperature_check = 0;
double error_acumulado = 0;
double error_anterior = 0;
unsigned long last_pid_time = 0;

int potenciometro = 0;
int posicion = 0;
int opcion = 0;
int boton = 0;
unsigned long last_blink_time = 0;
bool blink_state = true;
int pantalla = pDISPLAY;
int display = dAPAGADO;
int comando = cTEMPERATURA;
char buffer[10];
char arr_display[][15] = { "APAGADO", "ENCENDIDO", "MANUAL", "TERMOSTATO",
                           "POTENCIOMETRO" };
char arr_comando[][15] = { "TEMPERATURA", "ENCENDER", "MANUAL",
                           "TERMOSTATO", "APAGAR", "PARAMETROS",
                           "RESET", "POTENCIOMETRO" };

// =============================
// PROTOTIPOS DE FUNCIONES
// =============================
void leer_temperatura_pin_a1();
void leer_potenciometro_pin_a2();
void activar_agente_calefactor(float porcentaje);
void print_dAPAGADO(bool first_time);
void print_dENCENDIDO(bool first_time);
void print_dMANUAL(bool first_time);
void print_dTERMOSTATO(bool first_time);
void print_dPOTENCIOMETRO(bool first_time);
void print_COMANDOS(bool change_pending);
void comando_TEMPERATURA();
void comando_ENCENDER();
void comando_MANUAL();
void comando_TERMOSTATO();
void comando_APAGAR();
void comando_PARAMETROS();
void comando_RESET();
void comando_POTENCIOMETRO();
void gestionar_fase_bombilla();
int Leer_teclado_serial();
int Leer_teclado();
void Teclado_libre();
int encontrar_tecla(int v);
int media(int a, int b);
void blink();

// =============================
// FUNCIONES LCD
// =============================
void lcd_send_nibble(uint8_t nibble) {
  digitalWrite(LCD_D4, (nibble >> 0) & 0x01);
  digitalWrite(LCD_D5, (nibble >> 1) & 0x01);
  digitalWrite(LCD_D6, (nibble >> 2) & 0x01);
  digitalWrite(LCD_D7, (nibble >> 3) & 0x01);

  digitalWrite(LCD_EN, HIGH);
  delayMicroseconds(5);
  digitalWrite(LCD_EN, LOW);
  delayMicroseconds(100);
}

void lcd_send_byte(uint8_t value, uint8_t mode) {
  noInterrupts();

  digitalWrite(LCD_RS, mode);  // 0 = comando, 1 = dato
  lcd_send_nibble(value >> 4);
  lcd_send_nibble(value & 0x0F);
  interrupts();
  delayMicroseconds(40);
}

void lcd_begin(uint8_t cols, uint8_t rows) {
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_EN, OUTPUT);
  pinMode(LCD_D4, OUTPUT);
  pinMode(LCD_D5, OUTPUT);
  pinMode(LCD_D6, OUTPUT);
  pinMode(LCD_D7, OUTPUT);

  delay(50);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_EN, LOW);

  // Inicialización en modo 4 bits
  lcd_send_nibble(0x03);
  delay(100);
  lcd_send_nibble(0x03);
  delayMicroseconds(150);
  lcd_send_nibble(0x03);
  lcd_send_nibble(0x02);

  lcd_send_byte(0x28, 0);  // 4 bits, 2 líneas
  lcd_send_byte(0x0C, 0);  // display ON, cursor OFF
  lcd_send_byte(0x06, 0);  // autoincremento
  lcd_send_byte(0x01, 0);  // clear
  delay(2);
}

void lcd_setCursor(uint8_t col, uint8_t row) {
  static const uint8_t row_offsets[] = { 0x00, 0x40 };
  if (row > 1)
    row = 1;
  lcd_send_byte(0x80 | (col + row_offsets[row]), 0);
  if (col > 16 || row > 2) {
    return;
  }
  cursor_col = col;
  cursor_row = row;
}

void lcd_print(const char *str) {
  char c;
  for (int i = 0; i < strlen(str); i++) {
    c = str[i];
    if (c != '\r' && c != '\n') {
      lcd_send_byte(c, 1);
    }
  }
}


void lcd_print(char c) {
  if (c != '\r' && c != '\n') {
    lcd_send_byte(c, 1);
  }
}

void lcd_clear() {
  lcd_send_byte(0x01, 0);
  delay(2);
}

// =============================
// FUNCIONES LECTOR DE TECLAS
// =============================

int media(int a, int b) {
  return (a + b) / 2;
}

int encontrar_tecla(int v) {
  if (v < media(v1, v2)) {
    return 0;
  }
  if (v < media(v2, v3)) {
    return 1;
  }
  if (v < media(v3, v4)) {
    return 2;
  }
  if (v < media(v4, v5)) {
    return 3;
  }
  if (v < media(v5, v6)) {
    return 4;
  }
  return 5;
}

void Teclado_libre() {
  int fi2 = 10;
  int boton = 5;
  while (fi2 > 0) {
    if (boton == encontrar_tecla(analogRead(A0))) {
      fi2--;
    } else {
      fi2 = 10;
    }
  }
}
int Leer_teclado_serial() {
  if (Serial.available() > 0) {

    char tecla = Serial.read();

    switch (tecla) {
      case 'w':
      case 'W':
        return btnUP;

      case 's':
      case 'S':
        return btnDOWN;  

      case 'a':
      case 'A':
        return btnLEFT;

      case 'd':
      case 'D':
        return btnRIGHT;

      case 'e':
      case 'E':
        return btnSELECT;  

      default:
        return btnNONE;
    }
  }

  return btnNONE;
}

void esperar_soltar() {
  while (analogRead(A0) < 800) {
    delay(10);
  }
  delay(20); 
}

int Leer_teclado() {
  valor = v6;
  fi = 10;
  int count = 0;
  int boton = btnNONE;
  valor = analogRead(A0);
  if (valor < media(v5, v6)) {
    boton = encontrar_tecla(valor);
    while (fi > 0) {
      if (boton == encontrar_tecla(analogRead(A0))) {
        fi--;
      } else {
        fi = 10;
        if (count == 5){
          return btnNONE;
        }
        count++;
      }
    }
  }
  return boton;
}

int Tecla_mantenida(int boton) {
  int fi2 = 10;
  int count = 0;
  while (fi2 > 0) {
    if (boton == encontrar_tecla(analogRead(A0))) {
      fi2 = 10;
      count++;
      if (count > 5000) {
        delay(100);
      }
    } else {
      fi2--;
    }
  }
}

void blink() {
  if (millis() - last_blink_time > 400) {
    last_blink_time = millis();
    blink_state = !blink_state;  // Invertir estado (visible/invisible)

    // Dibujar el cursor ">" según la posicion
    if (blink_state) {
      lcd_setCursor(0, 0);
      lcd_print(opcion == 0 ? ">" : " ");
      lcd_setCursor(0, 1);
      lcd_print(opcion == 1 ? ">" : " ");
    } else {
      lcd_setCursor(0, 0);
      lcd_print(" ");
      lcd_setCursor(0, 1);
      lcd_print(" ");
    }
  }
}

/*
Función que muestra en el display el modo Apagado y la Temperatura leída por el
sensor
*/

void print_dAPAGADO(bool first_time) {
  if (first_time) {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print(arr_display[dAPAGADO]);
    lcd_setCursor(1, 1);
    lcd_print("select command");
  } else {
    lcd_setCursor(10, 0);
    dtostrf(temperatura, 4, 2, buffer);
    lcd_print(buffer);
    lcd_print("C");
  }
}

/*
Función que muestra en el display el modo Encendido y la Temperatura leída por
el sensor.
*/
void print_dENCENDIDO(bool first_time) {
  if (first_time) {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print(arr_display[dENCENDIDO]);
    lcd_setCursor(1, 1);
    lcd_print("select command");
  } else {
    lcd_setCursor(10, 0);
    dtostrf(temperatura, 4, 2, buffer);
    lcd_print(buffer);
    lcd_print("C");
  }
}

/*
Función que muestra en el display el modo Control manual, la Temperatura leída
por el sensor y porcentaje de funcionamiento del agente calefactor. TO DO
*/
void print_dMANUAL(bool first_time) {
  if (first_time) {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print(arr_comando[cMANUAL]);
  } else {
    lcd_setCursor(1, 1);
    lcd_print("   ");
    lcd_setCursor(1, 1);
    itoa(potencia_agente, buffer, 10);
    lcd_print(buffer);
    lcd_print("%");

    lcd_setCursor(10, 1);
    dtostrf(temperatura, 4, 2, buffer);
    lcd_print(buffer);
    lcd_print("C");
  }
}

/*
Función que muestra en el display el modo Termostato la Temperatura leída por el
sensor, temperatura objetivo y porcentaje de funcionamiento del agente
calefactor. TO DO
*/
void print_dTERMOSTATO(bool first_time) {
  if (first_time) {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print(arr_display[dTERMOSTATO]);
  } else {
    lcd_setCursor(0, 1);
    dtostrf(temperatura, 5, 2, buffer);  // 1 decimal es suficiente
    lcd_print(buffer);
    lcd_setCursor(7, 1);
    dtostrf(temperatura_objetivo, 5, 2, buffer);
    lcd_print(buffer);
    lcd_setCursor(13, 1);
    if (potencia_agente < 100)
      lcd_print(" ");
    if (potencia_agente < 10)
      lcd_print(" ");
    itoa(potencia_agente, buffer, 10);
    lcd_print(buffer);
    lcd_print("%");
  }
}

/*
Función que muestra en el display el modo Potenciómetro, la Temperatura leída
por el sensor y porcentaje de funcionamiento del agente calefactor. TO DO
*/
void print_dPOTENCIOMETRO(bool first_time) {
  if (first_time) {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print(arr_comando[cPOTENCIOMETRO]);
  } else {
    lcd_setCursor(1, 1);
    lcd_print("   ");  
    lcd_setCursor(1, 1);
    itoa(potenciometro, buffer, 10);
    lcd_print(buffer);
    lcd_print("%");
    lcd_setCursor(10, 1);
    dtostrf(temperatura, 4, 1, buffer);
    lcd_print(buffer);
    lcd_print("C");
  }
}

/*
Función que muestra en pantalla la lista de comandos, navegable mediante botones
*/
void print_COMANDOS(bool change_pending) {
  if (change_pending) {
    lcd_clear();
    lcd_setCursor(1, 0);
    if ((opcion = posicion % 2) == 0) {
      lcd_print(arr_comando[posicion]);
      lcd_setCursor(1, 1);
      lcd_print(arr_comando[posicion + 1]);
    } else {
      lcd_print(arr_comando[posicion - 1]);
      lcd_setCursor(1, 1);
      lcd_print(arr_comando[posicion]);
    }
  } else
    blink();
}

/*
Función Temperatura: Establece la temperatura objetivo en grados.
TO DO
*/
void comando_TEMPERATURA() {
  lcd_clear();
  lcd_setCursor(0, 0);
  lcd_print("TEMPERATURA OBJ");
  bool configurado = false;
  float temp = temperatura_objetivo;
  while (!configurado) {
    boton = Leer_teclado();
    esperar_soltar();
    if (boton == btnUP) {
      temp += 0.25;
    } else if (boton == btnDOWN) {
      temp -= 0.25;
    } else if (boton == btnLEFT) {
      temp -= 1;
    } else if (boton == btnRIGHT) {
      temp += 1;
    } 
    lcd_setCursor(4, 1);
    dtostrf(temp, 5, 2, buffer);
    lcd_print(buffer);
    lcd_print("C");
    if (boton == btnSELECT) {
      configurado = true;
      temperatura_objetivo = temp;
      print_COMANDOS(true);
    }
  }
}

/*
Función Encender: Enciende el agente calefactor(bombilla) suministrando el 100%
de energía. TO DO
*/
void comando_ENCENDER() {
  display = dENCENDIDO;
  pantalla = pDISPLAY;
  posicion = 0;
  activar_agente_calefactor(100);
  print_dENCENDIDO(true);
}

/*
Función Control manual: Establece el porcentaje de funcionamiento del agente
calefactor(bombilla) y regula la energía suministrada. TO DO
*/
void comando_MANUAL() {
  pantalla = pDISPLAY;
  display = dMANUAL;
  posicion = 0;
  print_dMANUAL(true);
}

/*
Función Termostato: Regula la energía suministrada al agente
calefactor(bombilla) manteniendo la temperatura objetivo establecida. TO DO
*/
void comando_TERMOSTATO() {
  error_acumulado = 0;
  error_anterior = 0;
  display = dTERMOSTATO;
  pantalla = pDISPLAY;
  posicion = 0;
  print_dTERMOSTATO(true);
}

/*
Función Apagar: Apaga el agente calefactor(bombilla).
*/
void comando_APAGAR() {
  display = dAPAGADO;
  pantalla = pDISPLAY;
  posicion = 0;
  activar_agente_calefactor(0);  // Apagar la bombilla
  print_dAPAGADO(true);
}

/*
Función Parámetros: Establece las constantes del filtro PID.
TO DO
*/
void comando_PARAMETROS() {
  bool salir = false;
  int parametro_seleccionado = 0;  

  lcd_clear();
  while (!salir) {
    // Mostrar parámetro actual
    lcd_setCursor(0, 0);
    if (parametro_seleccionado == 0)
      lcd_print("Kp: ");
    else if (parametro_seleccionado == 1)
      lcd_print("Ki: ");
    else
      lcd_print("Kd: ");

    // Mostrar valor
    double valor_actual;
    if (parametro_seleccionado == 0)
      valor_actual = Kp;
    else if (parametro_seleccionado == 1)
      valor_actual = Ki;
    else
      valor_actual = Kd;

    dtostrf(valor_actual, 4, 1, buffer);
    lcd_print(buffer);

    lcd_setCursor(0, 1);
    lcd_print("UP/DW:Val SEL:Ok");

    int boton = Leer_teclado();
    esperar_soltar();

    if (boton == btnUP) {
      if (parametro_seleccionado == 0)
        Kp += 1.0;
      else if (parametro_seleccionado == 1)
        Ki += 0.1;
      else
        Kd += 0.1;
    } else if (boton == btnDOWN) {
      if (parametro_seleccionado == 0)
        Kp -= 1.0;
      else if (parametro_seleccionado == 1)
        Ki -= 0.1;
      else
        Kd -= 0.1;
    } else if (boton == btnRIGHT) {
      parametro_seleccionado++;
      if (parametro_seleccionado > 2)
        parametro_seleccionado = 0;
      lcd_clear();
    } else if (boton == btnLEFT) {
      parametro_seleccionado--;
      if (parametro_seleccionado < 0)
        parametro_seleccionado = 2;
      lcd_clear();
    } else if (boton == btnSELECT) {
      salir = true;
      pantalla = pDISPLAY;
      display = dAPAGADO;  // Volver al menú principal
      print_dAPAGADO(true);
    }

    delay(100);  // Pequeño delay para no refrescar tan rápido
  }
}

/*
Función Reset: Inicializa todos los parámetros (los pone a cero).
TO DO
*/
void comando_RESET() {
  Kp = 30.0;
  Ki = 0.5;
  Kd = 2.0;
  error_acumulado = 0;
  error_anterior = 0;
  temperatura_objetivo = 25.0;
  activar_agente_calefactor(0);
  display = dAPAGADO;
  pantalla = pDISPLAY;
  posicion = 0;
  print_dAPAGADO(true);
}

/*
Función Potenciómetro: Regula la energía suministrada al agente
calefactor(bombilla) por medio del potenciómetro. TO DO
*/
void comando_POTENCIOMETRO() {
  display = dPOTENCIOMETRO;
  pantalla = pDISPLAY;
  posicion = 0;
  print_dPOTENCIOMETRO(true);
}

/*
Función auxiliar para activar y configurar potencia del agente calefactor
TO DO
*/

void activar_agente_calefactor(float porcentaje) {
  // Limitamos el rango por seguridad
  if (porcentaje < 0)
    porcentaje = 0;
  if (porcentaje > 100)
    porcentaje = 100;

  // Guardamos la potencia en la variable global
  potencia_agente = porcentaje;

  // Nota: Ya no usamos analogWrite porque para una bombilla AC
  // necesitamos control de fase manual.
}

void cruce_por_cero_isr() {
  // --- CAMBIO 1: FILTRO DE RUIDO (Debounce) ---
  // El audio sugiere ignorar los pulsos espurios tras el primer cruce.
  // Usamos una variable estática para recordar el tiempo del último disparo válido.
  static unsigned long last_interrupt_time = 0;
  unsigned long current_time = micros();

  // Si han pasado menos de 5ms desde la última interrupción, es ruido (el semiciclo dura 10ms).
  // Ignoramos esta falsa alarma y salimos.
  if (current_time - last_interrupt_time < 5000) {
    return;
  }
  last_interrupt_time = current_time;

  // SEGURIDAD: Si la potencia es 0, apagamos el Triac y salimos.
  if (potencia_agente <= 0) {
    cbi(PORTD, PORTD1);   // Pone en LOW (0V) el Pin 2 -> J1 no recibe corriente
    cbi(TIMSK1, OCIE1A);  // Apagar Timer
    return;
  }

  if (potencia_agente >= 100) {
    sbi(PORTD, PORTD1);  // Pin 2 HIGH constante
    cbi(TIMSK1, OCIE1A);
    return;
  }

  // --- CAMBIO 2: CORRECCIÓN DEL DESFASE (1.8 ms) ---
  // El audio indica disparar entre 1.8 ms y 9.5 ms.
  // 1.8 ms = 1800 µs. Con prescaler 64 (4 µs/tick) -> 1800 / 4 = 450 ticks.
  // Antes tenías 375 (1.5 ms), que es demasiado pronto.

  TCNT1 = 0;  // Reset del contador

  // Map: 1% (mínima potencia) -> 9.5ms (2375 ticks)
  // Map: 99% (máxima potencia) -> 1.8ms (450 ticks)
  int ticks = map(potencia_agente, 1, 99, 2375, 450);

  OCR1A = ticks;        // Establecemos la meta
  sbi(TIFR1, OCF1A);    // Limpiamos banderas pendientes
  sbi(TIMSK1, OCIE1A);  // Habilitamos la interrupción del Timer
}

ISR(TIMER1_COMPA_vect) {
  sbi(PORTD, PORTD1);  // DISPARO TRIAC (Pin 2 HIGH)
  // Pequeño retardo de disparo (suficiente para activar Gate)
  // Al ser tan corto (20us) no afecta al LCD
  delayMicroseconds(40);
  cbi(PORTD, PORTD1);  // FIN DISPARO (Pin 2 LOW)

  cbi(TIMSK1, OCIE1A);  // Apagamos el timer hasta el próximo cruce
}

/*
Función que permite leer la temperatura de un sensor de temperatura conectado al
pin a1 gris masa, morado 5V, blanco señal
*/

void leer_temperatura_pin_a1() {
  long suma = 0;
  int lecturas = 20;  // Hacemos 20 lecturas para promediar
  for (int i = 0; i < lecturas; i++) {
    suma += analogRead(A1);
    delay(1);  // Pequeña pausa entre lecturas para no coger el mismo ruido
  }
  float promedio = suma / (float)lecturas;
  temperatura = (promedio * 5.0 * 100.0) / 1024.0;
}

void leer_potenciometro_pin_a2() {
  int valor = analogRead(A2);
  potenciometro = valor / 10;
  if (potenciometro > 100) potenciometro = 100;
}

void setup() {
  Serial.begin(9600);
  pinMode(A1, INPUT);  // Sensor de temperatura
  pinMode(A2, INPUT);  // potenciómetro
  pinMode(PIN_ZCD, INPUT_PULLUP);

  sbi(DDRD, DDD1);  // Pin D1 como salida

  noInterrupts();
  TCCR1A = 0;  // Modo Normal
  TCCR1B = 0;
  // Prescaler 64 (CS11 y CS10) -> 4us por tick a 16MHz
  sbi(TCCR1B, CS11);
  sbi(TCCR1B, CS10);
  interrupts();

  lcd_begin(16, 2);

  // CONFIGURACIÓN PID
  Input = temperatura;
  Setpoint = temperatura_objetivo;

  myPID.SetMode(AUTOMATIC);       // Encender PID
  myPID.SetOutputLimits(0, 100);  // Limitar salida 0-100%
  myPID.SetSampleTime(100);       // Calcular cada 100ms (coincide con tu loop)
  attachInterrupt(digitalPinToInterrupt(PIN_ZCD), cruce_por_cero_isr, RISING);

  print_dAPAGADO(true);

} /*--(end setup )---*/

void loop() {
  int boton_leido = Leer_teclado();
  if (boton_leido != btnNONE) {
    boton = boton_leido;
  }
  if (millis() - last_temperature_check > 1000) {
    leer_temperatura_pin_a1();
    last_temperature_check = millis();
  }
  if (millis() - last_slow_task > 50) {
    esperar_soltar();
    last_slow_task = millis();
    Input = temperatura;
    Setpoint = temperatura_objetivo;

    if (pantalla == pDISPLAY) {
      if (boton == btnSELECT) {
        // salir del display y entrar al menu de comandos
        lcd_clear();
        print_COMANDOS(true);
        pantalla = pCOMANDOS;
        boton = btnNONE;
      } else {
        switch (display) {
          case dAPAGADO:
            {
              print_dAPAGADO(false);
              break;
            }
          case dENCENDIDO:
            {
              activar_agente_calefactor(100);
              print_dENCENDIDO(false);
              break;
            }
          case dMANUAL:
            {
              if (boton == btnUP) {
                potencia_agente += 1;
              } else if (boton == btnDOWN) {
                potencia_agente -= 1;
              } else if (boton == btnRIGHT) {
                potencia_agente += 10;
              } else if (boton == btnLEFT) {
                potencia_agente -= 10;
              }
              if (potencia_agente > 100) potencia_agente = 100;
              if (potencia_agente < 0) potencia_agente = 0;
              activar_agente_calefactor(potencia_agente);
              print_dMANUAL(false);
              break;
            }
          case dTERMOSTATO:
            {
              myPID.Compute();
              activar_agente_calefactor(Output);
              print_dTERMOSTATO(false);
              break;
            }
          case dPOTENCIOMETRO:
            {
              leer_potenciometro_pin_a2();
              activar_agente_calefactor(potenciometro);
              print_dPOTENCIOMETRO(false);
              break;
            }
        }
      }
    } else if (pantalla == pCOMANDOS) {
      if (boton == btnDOWN) {
        if (posicion < sizeCOMANDOS - 1) {
          posicion++;
        }
        print_COMANDOS(true);
      } else if (boton == btnUP) {
        if (posicion > 0) {
          posicion--;
        }
        print_COMANDOS(true);
      } else {
        print_COMANDOS(false);
      }

      if (boton == btnSELECT) {
        comando = posicion;
        boton = btnNONE;
        switch (comando) {
          case cTEMPERATURA:
            comando_TEMPERATURA();
            break;
          case cENCENDER:
            comando_ENCENDER();
            break;
          case cMANUAL:
            comando_MANUAL();
            break;
          case cTERMOSTATO:
            comando_TERMOSTATO();
            break;
          case cAPAGAR:
            comando_APAGAR();
            break;
          case cPARAMETROS:
            comando_PARAMETROS();
            break;
          case cRESET:
            comando_RESET();
            break;
          case cPOTENCIOMETRO:
            comando_POTENCIOMETRO();
            break;
        }
      }
      boton = btnNONE;
    }
  }
}
