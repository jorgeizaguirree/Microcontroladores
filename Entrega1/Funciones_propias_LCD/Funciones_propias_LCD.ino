/* YourDuino.com Example Software Sketch
 16 character 2 line I2C Display
 Backpack Interface labelled "YwRobot Arduino LCM1602 IIC V1"
 terry@yourduino.com */

/*-----( Import needed libraries )-----*/
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
// VARIABLES DE LCD
// =============================
int cursor_col = 0;
int cursor_row = 0;

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
  // static const uint8_t row_offsets[] = {0x00, 0x40};
  // if (row > 1) row = 1;
  // lcd_send_byte(0x80 | (col + row_offsets[row]), 0);
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


/*-----( Declare Variables )-----*/
//NONE

void setup() /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);  // Used to type in characters

  lcd_begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight

  //-------- Write characters on the display ------------------
  // NOTE: Cursor Position: (CHAR, LINE) start at 0
  lcd_setCursor(0, 0);  //Start at character 4 on line 0
  lcd_print("Hello");
  delay(1000);
  lcd_setCursor(0, 1);
  lcd_print("meme");
  delay(8000);

  // Wait and then tell user they can start the Serial Monitor and type in characters to
  // Display. (Set Serial Monitor option to "No Line Ending")
  lcd_clear();
  lcd_setCursor(0, 0);  //Start at character 0 on line 0
  lcd_print("Use Serial Mon");
  lcd_setCursor(0, 1);
  lcd_print("Type to display");


} /*--(end setup )---*/


void loop() /*----( LOOP: RUNS CONSTANTLY )----*/
{
  {
    // when characters arrive over the serial port...
    if (Serial.available()) {
      // wait a bit for the entire message to arrive
      delay(100);
      // clear the screen
      lcd_clear();
      // read all the available characters
      while (Serial.available() > 0) {
        // display each character to the LCD
        lcd_setCursor(0, 0);
        lcd_print(Serial.read());
      }
    }
  }

} /* --(end main loop )-- */


/* ( THE END ) */