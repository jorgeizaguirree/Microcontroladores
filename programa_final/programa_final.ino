#include <Wire.h> // Comes with Arduino IDE
// Get the LCD I2C Library here:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.

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
#define PIN_ZCD A3         // Pin donde conectamos el detector de paso por cero
float potencia_agente = 0; // Variable global para la potencia (0-100)

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
// VARIABLES GENERALES
// =============================

float temperatura = 0.0;
float temperatura_objetivo = 25.0; // Temperatura que queremos alcanzar
// Variables PID
double Kp = 2.0;
double Ki = 5.0;
double Kd = 1.0;
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
char arr_display[][15] = {"APAGADO", "ENCENDIDO", "MANUAL", "TERMOSTATO",
                          "POTENCIOMETRO"};
char arr_comando[][15] = {"TEMPERATURA", "ENCENDER",     "MANUAL",
                          "TERMOSTATO",  "APAGAR",       "PARAMETROS",
                          "RESET",       "POTENCIOMETRO"};

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
  delayMicroseconds(1);
  digitalWrite(LCD_EN, LOW);
  delayMicroseconds(100);
}

void lcd_send_byte(uint8_t value, uint8_t mode) {
  digitalWrite(LCD_RS, mode); // 0 = comando, 1 = dato
  lcd_send_nibble(value >> 4);
  lcd_send_nibble(value & 0x0F);
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

  lcd_send_byte(0x28, 0); // 4 bits, 2 líneas
  lcd_send_byte(0x0C, 0); // display ON, cursor OFF
  lcd_send_byte(0x06, 0); // autoincremento
  lcd_send_byte(0x01, 0); // clear
  delay(2);
}

void lcd_setCursor(uint8_t col, uint8_t row) {
  static const uint8_t row_offsets[] = {0x00, 0x40};
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

// --- Imprimir un solo carácter (ej. Serial.read()) ---
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

int media(int a, int b) { return (a + b) / 2; }

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
  // 1. Preguntamos si hay datos esperando en el cable USB
  if (Serial.available() > 0) {

    // 2. Leemos el carácter enviado
    char tecla = Serial.read();

    // 3. Convertimos la letra (wasd) al código de botón del Arduino
    // Aceptamos tanto minúsculas como mayúsculas
    switch (tecla) {
    case 'w':
    case 'W':
      return btnUP;

    case 's':
    case 'S':
      return btnDOWN; // ¡Ahora sí funcionará el botón abajo!

    case 'a':
    case 'A':
      return btnLEFT;

    case 'd':
    case 'D':
      return btnRIGHT;

    case 'e':
    case 'E':
      return btnSELECT; // Usamos 'e' para enter/aceptar

    default:
      // Si llega un carácter raro (como un salto de línea), lo ignoramos
      return btnNONE;
    }
  }

  // Si no se ha enviado nada por el puerto serie, devolvemos NONE
  return btnNONE;
}

int Leer_teclado() {
  valor = v6;
  fi = 10;
  int boton = 5;
  while ((valor = analogRead(A0)) > media(v5, v6)) {
    return -1;
  }
  if (valor < media(v5, v6)) {
    boton = encontrar_tecla(valor);
    while (fi > 0) {
      if (boton == encontrar_tecla(analogRead(A0))) {
        fi--;
      } else {
        fi = 10;
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
        Serial.print("Se está manteniendo la tecla: ");
        Serial.println(boton);
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
    blink_state = !blink_state; // Invertir estado (visible/invisible)

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
    delay(100);
    lcd_setCursor(1, 1);
    lcd_print("select command");
    delay(100);
  } else {
    lcd_setCursor(10, 0);
    dtostrf(temperatura, 4, 2, buffer);
    lcd_print(buffer);
    lcd_print("C");
    delay(100);
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
    delay(100);
    lcd_setCursor(1, 1);
    lcd_print("select command");
    delay(100);
  } else {
    lcd_setCursor(10, 0);
    dtostrf(temperatura, 4, 2, buffer);
    lcd_print(buffer);
    lcd_print("C");
    delay(100);
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
    // Primera Linea: nombre del modo del array
    lcd_print(arr_display[dMANUAL]);
  } else {
    // Segunda Linea: temperatura actual y porcentaje de funcionamiento
    lcd_setCursor(0, 1);
    dtostrf(temperatura, 5, 2, buffer); // funcion para pasar de float a char[]
    lcd_print(buffer);
    lcd_print("C");

    lcd_setCursor(9, 1);
    dtostrf(potenciometro, 5, 2,
            buffer); // funcion para pasar de float a char[]
    lcd_print(buffer);
    lcd_print("%");
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
    // Primera Linea: nombre del modo del array
    lcd_setCursor(0, 0);
    lcd_print(arr_display[dTERMOSTATO]);
  } else {
    // Segunda Linea: Temp Actual | Objetivo | Potencia
    // Formato: "25.0 >30 100%"

    // 1. Temp Actual
    lcd_setCursor(0, 1);
    dtostrf(temperatura, 4, 1, buffer); // 1 decimal es suficiente
    lcd_print(buffer);

    // 2. Objetivo con BLINK
    lcd_setCursor(5, 1);

    // Lógica de parpadeo manual (usamos las mismas variables que la función
    // blink)
    if (millis() - last_blink_time > 400) {
      last_blink_time = millis();
      blink_state = !blink_state;
    }

    if (blink_state) {
      lcd_print(">");
    } else {
      lcd_print(" ");
    }

    dtostrf(temperatura_objetivo, 2, 0, buffer); // Sin decimales
    lcd_print(buffer);

    // 3. Potencia
    lcd_setCursor(9, 1); // Ajustar según espacio
    // Usamos espacios en blanco para limpiar si pasamos de 100 a 10
    if (potenciometro < 100)
      lcd_print(" ");
    if (potenciometro < 10)
      lcd_print(" ");

    itoa(potenciometro, buffer, 10);
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
    lcd_setCursor(0, 0);
    lcd_print(arr_comando[cPOTENCIOMETRO]);
    lcd_setCursor(1, 1);
    itoa(potenciometro, buffer, 10);
    lcd_print(buffer);
    lcd_print("%");
  } else {
    // Actualizar porcentaje
    lcd_setCursor(1, 1);
    lcd_print("   "); // Limpiar valor anterior
    lcd_setCursor(1, 1);
    itoa(potenciometro, buffer, 10);
    lcd_print(buffer);
    lcd_print("%");

    // Actualizar temperatura
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
  display = dTERMOSTATO;
  pantalla = pDISPLAY;
  posicion = 0;
  print_dTERMOSTATO(true);
  // hacemos que al usar los botones arriba y abajo se cambie 1 grado la temp y
  // izquierda derecha se cambie 10 grados
  while (boton != btnSELECT) {
    gestionar_fase_bombilla(); // <--- IMPORTANTE
    Leer_teclado_serial();
    if (boton == btnUP) {
      temperatura_objetivo += 1;
      print_dTERMOSTATO(false);
    } else if (boton == btnDOWN) {
      temperatura_objetivo -= 1;
      print_dTERMOSTATO(false);
    } else if (boton == btnLEFT) {
      temperatura_objetivo += 10;
      print_dTERMOSTATO(false);
    } else if (boton == btnRIGHT) {
      temperatura_objetivo -= 10;
      print_dTERMOSTATO(false);
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
  print_dENCENDIDO(true);
  activar_agente_calefactor(100);
}

/*
Función Control manual: Establece el porcentaje de funcionamiento del agente
calefactor(bombilla) y regula la energía suministrada. TO DO
*/
void comando_MANUAL() {
  display = dMANUAL;
  pantalla = pDISPLAY;
  posicion = 0;
  print_dMANUAL(true);

  int boton = btnNONE;
  // Bucle para ajustar la potencia manualmente con BOTONES
  while (boton != btnSELECT) {
    gestionar_fase_bombilla(); // Mantener la bombilla funcionando

    boton = Leer_teclado_serial();

    if (boton == btnUP) {
      potenciometro += 5; // Subir 5%
      if (potenciometro > 100)
        potenciometro = 100;
      activar_agente_calefactor(potenciometro);
      print_dMANUAL(false);
    } else if (boton == btnDOWN) {
      potenciometro -= 5; // Bajar 5%
      if (potenciometro < 0)
        potenciometro = 0;
      activar_agente_calefactor(potenciometro);
      print_dMANUAL(false);
    }
  }
}

/*
Función Termostato: Regula la energía suministrada al agente
calefactor(bombilla) manteniendo la temperatura objetivo establecida. TO DO
*/
void comando_TERMOSTATO() {
  display = dTERMOSTATO;
  pantalla = pDISPLAY;
  posicion = 0;
  print_dTERMOSTATO(true);
  // No hacemos bucle aquí, dejamos que el loop principal se encargue del
  // control
}

/*
Función Apagar: Apaga el agente calefactor(bombilla).
*/
void comando_APAGAR() {
  display = dAPAGADO;
  pantalla = pDISPLAY;
  posicion = 0;
  activar_agente_calefactor(0); // Apagar la bombilla
  print_dAPAGADO(true);
}

/*
Función Parámetros: Establece las constantes del filtro PID.
TO DO
*/
void comando_PARAMETROS() {
  bool salir = false;
  int parametro_seleccionado = 0; // 0:Kp, 1:Ki, 2:Kd

  lcd_clear();

  while (!salir) {
    gestionar_fase_bombilla(); // Mantener control de fase

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

    int boton = Leer_teclado_serial();

    if (boton == btnUP) {
      if (parametro_seleccionado == 0)
        Kp += 0.1;
      else if (parametro_seleccionado == 1)
        Ki += 0.1;
      else
        Kd += 0.1;
    } else if (boton == btnDOWN) {
      if (parametro_seleccionado == 0)
        Kp -= 0.1;
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
      display = dAPAGADO; // Volver al menú principal
      print_dAPAGADO(true);
    }

    delay(100); // Pequeño delay para no refrescar tan rápido
  }
}

/*
Función Reset: Inicializa todos los parámetros (los pone a cero).
TO DO
*/
void comando_RESET() {
  // Resetear variables PID a valores por defecto
  Kp = 2.0;
  Ki = 5.0;
  Kd = 1.0;
  error_acumulado = 0;
  error_anterior = 0;

  // Resetear temperatura objetivo
  temperatura_objetivo = 25.0;

  // Apagar bombilla
  activar_agente_calefactor(0);

  // Volver a pantalla de inicio
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
  // No hacemos bucle aquí, dejamos que el loop principal se encargue del
  // control
}

/*
Función auxiliar para activar y configurar potencia del agente calefactor
TO DO
*/

/*
Función para calcular el PID
*/
double calcular_PID(double input, double setpoint) {
  unsigned long now = millis();
  double timeChange = (double)(now - last_pid_time);

  // Ejecutar solo si ha pasado suficiente tiempo (ej. 200ms)
  if (timeChange < 200)
    return potencia_agente;

  double error = setpoint - input;
  error_acumulado += (error * timeChange);
  double error_derivado = (error - error_anterior) / timeChange;

  double output = (Kp * error) + (Ki * error_acumulado) + (Kd * error_derivado);

  // Guardar variables para la próxima
  error_anterior = error;
  last_pid_time = now;

  // Limitar salida
  if (output > 100)
    output = 100;
  if (output < 0)
    output = 0;

  return output;
}

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

void gestionar_fase_bombilla() {
  if (potencia_agente <= 0) {
    cbi(PORTD, PORTD1);
    cbi(PORTB, PORTB6);
    return;
  }

  // Detectar paso por cero en A3
  if (analogRead(PIN_ZCD) > 900) {

    // CALCULO DE ESPERA (Phase Delay)
    // Para 50Hz, el semiciclo es de 10ms (10000us).
    // Si 50% de potencia es ~6ms de espera (6000us):
    // 0%   -> 9500us (casi todo el ciclo apagado)
    // 50%  -> 6000us (tu valor de prueba)
    // 100% -> 0us    (encendido inmediato)

    long espera;
    if (potencia_agente >= 100) {
      espera = 0;
    } else {
      // Usamos una fórmula que se acerque a tus 6ms para el 50%
      espera = map(potencia_agente, 0, 100, 9500, 0);
    }

    // 1. Esperar el tiempo de fase
    if (espera > 0) {
      delayMicroseconds(espera);
    }

    // 2. DISPARO DEL OPTOTRIAC (34 microsegundos)
    sbi(PORTD, PORTD1);
    sbi(PORTB, PORTB6);

    delayMicroseconds(34); // Pulso de disparo constante

    // 3. APAGAR PULSO (El Triac seguirá conduciendo hasta el próximo paso por
    // cero)
    cbi(PORTD, PORTD1);
    cbi(PORTB, PORTB6);

    // 4. Sincronización: esperar a que el pulso de ZCD termine para no repetir
    // en el mismo ciclo
    while (analogRead(PIN_ZCD) > 800)
      ;
  }
}

/*
Función que permite leer la temperatura de un sensor de temperatura conectado al
pin a1 gris masa, morado 5V, blanco señal
*/

void leer_temperatura_pin_a1() {
  int valor = analogRead(A1);
  temperatura = (valor * 5.0 * 100.0) / 1024.0;
}

void leer_potenciometro_pin_a2() {
  int valor = analogRead(A2);
  potenciometro = valor / 10;
  Serial.println(potenciometro);
}

void setup() {
  Serial.begin(9600);
  pinMode(A1, INPUT); // Sensor de temperatura
  pinMode(A2, INPUT); // potenciómetro
  pinMode(PIN_ZCD, INPUT);
  sbi(DDRD, DDD1); // Pin D1 como salida
  sbi(DDRB, DDB6); // Pin D14/A0 (o el que usemos) como salida
  lcd_begin(16, 2);
  print_dAPAGADO(true);

} /*--(end setup )---*/

void loop() {
  gestionar_fase_bombilla();
  int boton = Leer_teclado_serial();
  leer_temperatura_pin_a1();
  // -----------------------------

  if (pantalla == pDISPLAY) {
    if (boton == btnSELECT) {
      // salir del display y entrar al menu de comandos
      lcd_clear();
      print_COMANDOS(true);
      pantalla = pCOMANDOS;
    } else {
      // mostrar la pantalla correspondiente y ejecutar lógica
      switch (display) {
      case dAPAGADO: {
        print_dAPAGADO(false);
        break;
      }
      case dENCENDIDO: {
        // En modo encendido siempre al 100%
        activar_agente_calefactor(100);
        print_dENCENDIDO(false);
        break;
      }
      case dMANUAL: {
        // La lógica manual ya está en el comando_MANUAL (es bloqueante con
        // botones) Aquí solo refrescamos pantalla si volviéramos a este modo
        print_dMANUAL(false);
        break;
      }
      case dTERMOSTATO: {
        // --- LÓGICA DEL TERMOSTATO PID ---
        double potencia_pid = calcular_PID(temperatura, temperatura_objetivo);
        activar_agente_calefactor(potencia_pid);
        potenciometro = potencia_agente; // Actualizar para mostrar en LCD

        print_dTERMOSTATO(false);
        break;
      }
      case dPOTENCIOMETRO: {
        // --- LÓGICA DEL POTENCIÓMETRO ---
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
      switch (comando) {
      case cTEMPERATURA: {
        comando_TEMPERATURA();
        break;
      }
      case cENCENDER: {
        comando_ENCENDER();
        break;
      }
      case cMANUAL: {
        comando_MANUAL();
        break;
      }
      case cTERMOSTATO: {
        comando_TERMOSTATO();
        break;
      }
      case cAPAGAR: {
        comando_APAGAR();
        break;
      }
      case cPARAMETROS: {
        comando_PARAMETROS();
        break;
      }
      case cRESET: {
        comando_RESET();
        break;
      }
      case cPOTENCIOMETRO: {
        comando_POTENCIOMETRO();
        break;
      }
      }
    }
  }
}
