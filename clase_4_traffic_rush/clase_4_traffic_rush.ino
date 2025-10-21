/* YourDuino.com Example Software Sketch
 TEST LCD Display with Pushbuttons
 Based on code by Mark Bramwell and debouncing by peterstrobl
 terry@yourduino.com */

/*-----( Import needed libraries )-----*/
#include <LiquidCrystal.h>

/*-----( Declare objects )-----*/
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  //These are the pins used on this shield

/*-----( Declare Constants )-----*/
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

/*-----( Declare Variables )-----*/
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
int arr_pos_enemy[5]; // 


void setup() /*----( SETUP: RUNS ONCE )----*/
{
  lcd.begin(16, 2);  // start the lcd object
  delay(20);
  lcd.setCursor(2, 0);
  lcd.print("TRAFFIC RUSH");
  lcd.setCursor(0, 1);
  lcd.print("Press any button");
  delay(20);

} /*--(end setup )---*/

void loop() /*----( LOOP: RUNS CONSTANTLY )----*/
{
  lcd_key = read_LCD_buttons();
  if (lcd_key != btnNONE) {
    resetLCD();
    delay(400);
    change_pos();
    while (true) {
      lcd_key = read_LCD_buttons();  // read the buttons
      if (lcd_key == btnUP) {
        if (pos_chr) {
          change_pos();
        }
      } else if (lcd_key == btnDOWN) {
        if (!pos_chr) {
          change_pos();
        }
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

} /* --(end main loop )-- */

/*-----( Declare User-written Functions )-----*/
void game_over() {
  resetLCD();
  lcd.setCursor(3, 0);
  lcd.print("GAME  OVER");
  lcd.setCursor(0, 1);
  lcd.print("Press any button");
}

void enemy_movement() {
  if (millis() - prev_time > tick_speed) {
    rng = prev_time % 2;
    if (!enemy_number) {
      lcd.setCursor(16, rng);
      lcd.print(enemy);
      pos_enemy_x = 16;
      pos_enemy_y = rng;
      enemy_number = 1;
    } else {
      lcd.setCursor(pos_enemy_x, pos_enemy_y);
      lcd.print(" ");
      if (pos_enemy_x != 0) {
        pos_enemy_x -= 1;
        lcd.setCursor(pos_enemy_x, pos_enemy_y);
        lcd.print(enemy);
      } else {
        enemy_number = 0;
      }
    }
    prev_time = millis();
  }
}

void change_pos() {
  lcd.setCursor(0, pos_chr);
  lcd.print(" ");
  pos_chr = !pos_chr;
  lcd.setCursor(0, pos_chr);
  lcd.print(chr);
  delay(100);
}
void resetLCD() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

int read_LCD_buttons() {
  adc_key_in = analogRead(0);            // read the value from the sensor
  delay(10);                             //switch debounce delay. Increase this delay if incorrect switch selections are returned.
  int k = (analogRead(0) - adc_key_in);  //gives the button a slight range to allow for a little contact resistance noise
  if (5 < abs(k)) return btnNONE;        // double checks the keypress. If the two readings are not equal +/-k value after debounce delay, it tries again.
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE;  // We make this the 1st option for speed reasons since it will be the most likely result
  if (adc_key_in < 50) return btnRIGHT;
  if (adc_key_in < 195) return btnUP;
  if (adc_key_in < 380) return btnDOWN;
  if (adc_key_in < 555) return btnLEFT;
  if (adc_key_in < 790) return btnSELECT;
  return btnNONE;  // when all others fail, return this...
}
