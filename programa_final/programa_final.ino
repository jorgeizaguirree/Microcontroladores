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

int posicion = 0;
int opcion = 0;
int boton = 0;
unsigned long last_blink_time = 0;
bool blink_state = true;
int pantalla = pDISPLAY;
int display = dAPAGADO;
int comando = cTEMPERATURA;
char arr_display[][15] = { "APAGADO", "ENCENDIDO", "MANUAL", "TERMOSTATO", "POTENCIOMETRO" };
char arr_comando[][15] = { "TEMPERATURA", "ENCENDER", "MANUAL", "TERMOSTATO", "APAGAR", "PARAMETROS", "RESET", "POTENCIOMETRO" };

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

void print_dAPAGADO() {
  lcd_setCursor(4, 0);
  lcd_print(arr_display[dAPAGADO]);
  delay(2000);
  lcd_setCursor(2, 1);
  lcd_print("press select");
  delay(300);
}
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

void print_dENCENDIDO() {}

void print_dMANUAL() {}

void print_dTERMOSTATO() {}

void print_dPOTENCIOMETRO() {}
//{ "TEMPERATURA", "ENCENDER", "MANUAL", "TERMOSTATO", "APAGAR", "PARAMETROS", "RESET", "POTENCIOMETRO" };
void comando_TEMPERATURA() {}
void comando_ENCENDER() {}
void comando_MANUAL() {}
void comando_TERMOSTATO() {}
void comando_APAGAR() {}
void comando_PARAMETROS() {}
void comando_RESET() {}
void comando_POTENCIOMETRO() {}




void setup() {
  Serial.begin(9600);
  lcd_begin(16, 2);
  print_dAPAGADO();

} /*--(end setup )---*/

//char arr_display[][15] = { "APAGADO", "ENCENDIDO", "MANUAL", "TERMOSTATO", "POTENCIOMETRO" };



void loop() {
  int boton = Leer_teclado_serial();
  if (pantalla == pDISPLAY) {
    if (boton == btnSELECT) {
      lcd_clear();
      print_COMANDOS(true);
      pantalla = pCOMANDOS;
    } else {
      switch (display) {
        case dAPAGADO:
          {
            print_dAPAGADO();
            break;
          }
        case dENCENDIDO:
          {
            print_dENCENDIDO();
            break;
          }
        case dMANUAL:
          {
            print_dMANUAL();
            break;
          }
        case dTERMOSTATO:
          {
            print_dTERMOSTATO();
            break;
          }
        case dPOTENCIOMETRO:
          {
            print_dPOTENCIOMETRO();
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