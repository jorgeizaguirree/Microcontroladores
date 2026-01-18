int valor;
#define sbi(port, bit) (port) |= (1 << (bit))
#define cbi(port, bit) (port) &= ~(1 << (bit))
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(57600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(A2, INPUT);
}



// the loop function runs over and over again forever
void loop() {
  valor = analogRead(A2);
  Serial.print("El potenciometro está dando valor: ");
  Serial.print(valor);
  Serial.println("  ... Buscando siguiente tecla");
}
