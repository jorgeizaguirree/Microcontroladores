/* YourDuino.com Example Software Sketch (modificado)
   Sin usar LiquidCrystal.h
   Compatible con Displaytech 162J BC BW (LCD 16x2)
   Modo 4 bits
*/

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
  delay(5);
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
  if (row > 1) row = 1;
  lcd_send_byte(0x80 | (col + row_offsets[row]), 0);
  cursor_col = col;
  cursor_row = row;
}

void lcd_print(const char *str) {
  while (*str) {
    lcd_send_byte(*str++, 1);
  }
}

void lcd_clear() {
  lcd_send_byte(0x01, 0);
  delay(2);
}

// =============================
// TUS CONSTANTES Y VARIABLES
// =============================
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

int lcd_key = 0;
int adc_key_in = 0;
int adc_key_prev = 0;
int pos_chr = 1;
int pos_enemy_x = 16;
int pos_enemy_y = 0;
char chr = 'o';
char enemy = 'x';
long tick_speed = 100;
long prev_time = 0;
int enemy_number = 0;
int rng = 0;
int arr_pos_enemy[5];

// =============================
// SETUP
// =============================
void setup() {
  lcd_begin(16, 2);
  delay(20);
  lcd_setCursor(2, 0);
  lcd_print("TRAFFIC RUSH");
  lcd_setCursor(0, 1);
  lcd_print("Press any button");
  delay(20);
}

// =============================
// LOOP PRINCIPAL
// =============================
void loop() {
  lcd_key = read_LCD_buttons();
  if (lcd_key != btnNONE) {
    resetLCD();
    delay(400);
    change_pos();
    while (true) {
      lcd_key = read_LCD_buttons();
      if (lcd_key == btnUP) {
        if (pos_chr) change_pos();
      } else if (lcd_key == btnDOWN) {
        if (!pos_chr) change_pos();
      }
      enemy_movement();
      if (pos_enemy_x == 0) {
        if (pos_chr == pos_enemy_y) {
          break;
        }
      }
    }
    delay(500);
    game_over();
  }
}

// =============================
// FUNCIONES DEL JUEGO
// =============================
void game_over() {
  resetLCD();
  lcd_setCursor(3, 0);
  lcd_print("GAME  OVER");
  lcd_setCursor(0, 1);
  lcd_print("Press any button");
}

void enemy_movement() {
  if (millis() - prev_time > tick_speed) {
    rng = prev_time % 2;
    if (!enemy_number) {
      lcd_setCursor(16, rng);
      lcd_print("x");
      pos_enemy_x = 16;
      pos_enemy_y = rng;
      enemy_number = 1;
    } else {
      lcd_setCursor(pos_enemy_x, pos_enemy_y);
      lcd_print(" ");
      if (pos_enemy_x != 0) {
        pos_enemy_x -= 1;
        lcd_setCursor(pos_enemy_x, pos_enemy_y);
        lcd_print("x");
      } else {
        enemy_number = 0;
      }
    }
    prev_time = millis();
  }
}

void change_pos() {
  lcd_setCursor(0, pos_chr);
  lcd_print(" ");
  pos_chr = !pos_chr;
  lcd_setCursor(0, pos_chr);
  lcd_print("o");
  delay(100);
}

void resetLCD() {
  lcd_clear();
}

int read_LCD_buttons() {
  adc_key_in = analogRead(0);
  delay(10);
  int k = (analogRead(0) - adc_key_in);
  if (5 < abs(k)) return btnNONE;
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50) return btnRIGHT;
  if (adc_key_in < 195) return btnUP;
  if (adc_key_in < 380) return btnDOWN;
  if (adc_key_in < 555) return btnLEFT;
  if (adc_key_in < 790) return btnSELECT;
  return btnNONE;
}
