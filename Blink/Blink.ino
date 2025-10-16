/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://docs.arduino.cc/hardware/

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/Blink/

*/
#define sbi(port,bit)   (port) |= (1 << (bit))
#define cbi(port,bit)   (port) &= ~(1 << (bit))
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(57600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(13, OUTPUT);
  sbi(DDRC, DDC7);
  sbi(DDRB, DDB6);
}

// the loop function runs over and over again forever
void loop() {

  Serial.println("Enciendo el led");
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  sbi(PORTC, PORTC7);
  cbi(PORTB, PORTB6);
  delay(1000);                      // wait for a second
  Serial.println("Apago el led");
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  cbi(PORTC, PORTC7);
  sbi (PORTB, PORTB6);  
  delay(1000);                      // wait for a second
  
}
