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
  Serial.println(" pasos por cero este segundo");
  count = 0;
}
