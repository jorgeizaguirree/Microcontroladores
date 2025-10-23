int valor;
#define sbi(port,bit)   (port) |= (1 << (bit))
#define cbi(port,bit)   (port) &= ~(1 << (bit))
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(57600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(13, OUTPUT);
  pinMode(A1, INPUT);
  sbi(DDRD, DDD1);
  sbi(DDRB, DDB6);
}

// the loop function runs over and over again forever
void loop() {

  Serial.println("Enciendo el led");
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  sbi(PORTD, PORTD1);
  sbi (PORTB, PORTB6);
  valor = analogRead(A1);
  Serial.print("El Paso Por Cero está dando valor: ");
  Serial.print(valor);
  delay(1000);                      // wait for a second
  Serial.println("Apago el led");
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  cbi(PORTD, PORTD1);
  cbi(PORTB, PORTB6);
  valor = analogRead(A1);
  Serial.print("El Paso Por Cero está dando valor: ");
  Serial.print(valor);
  delay(1000);                      // wait for a second
  
}
