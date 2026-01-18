#include <Arduino.h>
#line 1 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
#include <Wire.h>  // Comes with Arduino IDE
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
char arr_display[][15] = { "APAGADO", "ENCENDIDO", "MANUAL", "TERMOSTATO", "POTENCIOMETRO" };
char arr_comando[][15] = { "TEMPERATURA", "ENCENDER", "MANUAL", "TERMOSTATO", "APAGAR", "PARAMETROS", "RESET", "POTENCIOMETRO" };

// =============================
// FUNCIONES LCD
// =============================
#line 98 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void lcd_send_nibble(uint8_t nibble);
#line 110 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void lcd_send_byte(uint8_t value, uint8_t mode);
#line 117 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void lcd_begin(uint8_t cols, uint8_t rows);
#line 144 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void lcd_setCursor(uint8_t col, uint8_t row);
#line 155 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void lcd_print(const char *str);
#line 166 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void lcd_print(char c);
#line 172 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void lcd_clear();
#line 181 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
int media(int a, int b);
#line 185 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
int encontrar_tecla(int v);
#line 204 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void Teclado_libre();
#line 215 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
int Leer_teclado_serial();
#line 255 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
int Leer_teclado();
#line 275 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
int Tecla_mantenida(int boton);
#line 293 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void blink();
#line 317 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void print_dAPAGADO(bool first_time);
#line 339 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void print_dENCENDIDO(bool first_time);
#line 362 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void print_dMANUAL(bool first_time);
#line 387 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void print_dTERMOSTATO(bool first_time);
#line 399 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void print_dPOTENCIOMETRO(bool first_time);
#line 419 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void print_COMANDOS(bool change_pending);
#line 439 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void comando_TEMPERATURA();
#line 445 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void comando_ENCENDER();
#line 452 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void comando_MANUAL();
#line 459 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void comando_TERMOSTATO();
#line 464 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void comando_APAGAR();
#line 475 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void comando_PARAMETROS();
#line 481 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void comando_RESET();
#line 488 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void comando_POTENCIOMETRO();
#line 521 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void activar_agente_calefactor(float porcentaje);
#line 530 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void leer_temperatura_pin_a1();
#line 536 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void leer_potenciometro_pin_a2();
#line 544 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void setup();
#line 554 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
void loop();
#line 98 "/home/tomasbeboshvili/Documents/Microcontroladores/programa_final/programa_final.ino"
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
  digitalWrite(LCD_RS, mode);  // 0 = comando, 1 = dato
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

  lcd_send_byte(0x28, 0);  // 4 bits, 2 líneas
  lcd_send_byte(0x0C, 0);  // display ON, cursor OFF
  lcd_send_byte(0x06, 0);  // autoincremento
  lcd_send_byte(0x01, 0);  // clear
  delay(2);
}

void lcd_setCursor(uint8_t col, uint8_t row) {
  static const uint8_t row_offsets[] = { 0x00, 0x40 };
  if (row > 1) row = 1;
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
        return btnDOWN;  // ¡Ahora sí funcionará el botón abajo!

      case 'a':
      case 'A':
        return btnLEFT;

      case 'd':
      case 'D':
        return btnRIGHT;

      case 'e':
      case 'E':
        return btnSELECT;  // Usamos 'e' para enter/aceptar

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
Función que muestra en el display el modo Apagado y la Temperatura leída por el sensor
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
Función que muestra en el display el modo Encendido y la Temperatura leída por el sensor.
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
Función que muestra en el display el modo Control manual, la Temperatura leída por el sensor 
y porcentaje de funcionamiento del agente calefactor.
TO DO
*/
void print_dMANUAL(bool first_time) {
  if (first_time) {
    lcd_clear();
    lcd_setCursor(0,0);
    //Primera Linea: nombre del modo del array
    lcd_print(arr_display[dMANUAL]);
  } else {
    //Mostrar la temperatura actual
    lcd_setCursor(0, 1);
    dtostrf(temperatura, 5, 2, buffer); //funcion para pasar de float a char[]
    lcd_print(buffer);
    lcd_print("C");
	//Mostrar el porcentaje actual
	lcd_setCursor(9, 1);
	dtostrf(potenciometro, 5, 2, buffer); //funcion para pasar de float a char[]
	lcd_print(buffer);
	lcd_print("%");
  }
}

/*
Función que muestra en el display el modo Termostato la Temperatura leída por el sensor, 
temperatura objetivo y porcentaje de funcionamiento del agente calefactor.
TO DO
*/
void print_dTERMOSTATO(bool first_time) {
  if (first_time) {
    lcd_clear();
  } else {
  }
}

/*
Función que muestra en el display el modo Potenciómetro, la Temperatura leída por el sensor
y porcentaje de funcionamiento del agente calefactor.
TO DO
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
    lcd_setCursor(10, 1);
    leer_temperatura_pin_a1();
    dtostrf(temperatura, 4, 2, buffer);
    lcd_print(temperatura);
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
  } else blink();
}

/*
Función Temperatura: Establece la temperatura objetivo en grados.
TO DO
*/
void comando_TEMPERATURA() {}

/*
Función Encender: Enciende el agente calefactor suministrando el 100% de energía.
TO DO
*/
void comando_ENCENDER() {}

/*
Función Control manual: Establece el porcentaje de funcionamiento dal agente calefactor
y regula la energía suministrada.
TO DO
*/
void comando_MANUAL() {}

/*
Función Termostato: Regula la energía suministrada al agente calefactor manteniendo 
la temperatura objetivo establecida.
TO DO
*/
void comando_TERMOSTATO() {}

/*
Función Apagar: Apaga el agente calefactor.
*/
void comando_APAGAR() {
  display = dAPAGADO;
  pantalla = pDISPLAY;
  posicion = 0;
  print_dAPAGADO(true);
}

/*
Función Parámetros: Establece las constantes del filtro PID.
TO DO
*/
void comando_PARAMETROS() {}

/*
Función Reset: Inicializa todos los parámetros (los pone a cero).
TO DO
*/
void comando_RESET() {}

/*
Función Potenciómetro: Regula la energía sumimistrada al agente calefactor 
por medio del potenciómetro.
TO DO
*/
void comando_POTENCIOMETRO() {
  bool configurado = false;
  lcd_clear();
  lcd_setCursor(0, 0);
  lcd_print(arr_comando[cPOTENCIOMETRO]);
  while (!configurado) {
    Leer_teclado_serial();
    if (boton != btnSELECT) {
      leer_temperatura_pin_a1();
      leer_potenciometro_pin_a2();
      lcd_setCursor(6, 1);
      lcd_print("     ");
      lcd_setCursor(6, 1);
      itoa(potenciometro, buffer, 10);
      lcd_print(buffer);
      lcd_print("%");
      lcd_setCursor(10, 1);
    } else {
      activar_agente_calefactor(potenciometro);
      pantalla = pDISPLAY;
      posicion = 0;
      display = dPOTENCIOMETRO;
      print_dPOTENCIOMETRO(true);
      configurado = true;
    }
  }
}

/*
Función auxiliar para activar y configurar potencia del agente calefactor
TO DO
*/

void activar_agente_calefactor(float porcentaje) {
}


/*
Función que permite leer la temperatura de un sensor de temperatura conectado al pin a1
gris masa, morado 5V, blanco señal
*/

void leer_temperatura_pin_a1() {
  int valor = analogRead(A1);
  temperatura = (valor * 5.0 * 100.0) / 1024.0;
  delay(20);
}

void leer_potenciometro_pin_a2() {
  int valor = analogRead(A2);
  potenciometro = valor / 10;
  Serial.println(potenciometro);
  delay(20);
}


void setup() {
  Serial.begin(9600);
  pinMode(A1, INPUT);  // Sensor de temperatura
  pinMode(A2, INPUT);  // potenciómetro
  lcd_begin(16, 2);
  print_dAPAGADO(true);

} /*--(end setup )---*/


void loop() {
  int boton = Leer_teclado_serial();
  leer_temperatura_pin_a1();
  if (pantalla == pDISPLAY) {
    if (boton == btnSELECT) {
      // salir del display y entrar al menu de comandos
      lcd_clear();
      print_COMANDOS(true);
      pantalla = pCOMANDOS;
    } else {
      // mostrar la pantalla correspondiente
      switch (display) {
        case dAPAGADO:
          {
            print_dAPAGADO(false);
            break;
          }
        case dENCENDIDO:
          {
            print_dENCENDIDO(false);
            break;
          }
        case dMANUAL:
          {
            print_dMANUAL(false);
            break;
          }
        case dTERMOSTATO:
          {
            print_dTERMOSTATO(false);
            break;
          }
        case dPOTENCIOMETRO:
          {
            print_dPOTENCIOMETRO(false);
            break;
          }
      }
    }
  } else if (pantalla == pCOMANDOS) {
    if (boton == btnDOWN) {
      if (posicion < sizeCOMANDOS - 1) { posicion++; }
      print_COMANDOS(true);
    } else if (boton == btnUP) {
      if (posicion > 0) { posicion--; }
      print_COMANDOS(true);
    } else {
      print_COMANDOS(false);
    }
    if (boton == btnSELECT) {
      comando = posicion;
      switch (comando) {
        case cTEMPERATURA:
          {
            comando_TEMPERATURA();
            break;
          }
        case cENCENDER:
          {
            comando_ENCENDER();
            break;
          }
        case cMANUAL:
          {
            comando_MANUAL();
            break;
          }
        case cTERMOSTATO:
          {
            comando_TERMOSTATO();
            break;
          }
        case cAPAGAR:
          {
            comando_APAGAR();
            break;
          }
        case cPARAMETROS:
          {
            comando_PARAMETROS();
            break;
          }
        case cRESET:
          {
            comando_RESET();
            break;
          }
        case cPOTENCIOMETRO:
          {
            comando_POTENCIOMETRO();
            break;
          }
      }
    }
  }
}

/* ( THE END ) */

