int valor;
int prev_valor;
int count;
#define sbi(port, bit) (port) |= (1 << (bit))
#define cbi(port, bit) (port) &= ~(1 << (bit))
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(57600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(13, OUTPUT);
  pinMode(A1, INPUT);
  sbi(DDRD, DDD1);
  sbi(DDRB, DDB6);
}


int convertir(int a) {
  if (a > 900) {
    return 1;
  }
  if (a < 200) {
    return 0;
  }
  return -1;
}

// the loop function runs over and over again forever
void loop() {
  /*
  Serial.println("Enciendo el led");
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  sbi(PORTD, PORTD1);
  sbi (PORTB, PORTB6);
  valor = analogRead(A1);
  Serial.print("El Paso Por Cero está dando valor: ");
  Serial.println(valor);
  delay(40);                      // wait for a second
  Serial.println("Apago el led");
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  cbi(PORTD, PORTD1);
  cbi(PORTB, PORTB6);
  valor = analogRead(A1);
  Serial.print("El Paso Por Cero está dando valor: ");
  Serial.println(valor);
  delay(6);                      // wait for a second
  */
  int prev_time = millis();
  while (millis() - prev_time < 1000) {
    valor = convertir(analogRead(A1));
    if (valor != -1) {
      if (valor != prev_valor) {
        count++;
      }
      prev_valor = valor;
    }
  }
  Serial.print("Ha habido ");
  Serial.print(count);
  Serial.println(" pasos por cero");
  count = 0;
}
