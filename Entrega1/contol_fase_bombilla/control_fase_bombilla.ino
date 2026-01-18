// ======================================================
// CONTROL DE FASE - ENTREGA 1
// ======================================================
// Este programa controla la potencia de una bombilla
// esperando un tiempo tras el paso por cero (ZCD).
// ======================================================

int pinZCD = A1;     // Detector de paso por cero
int pinPot = A2;     // Potenciómetro para controlar el %
int porcentaje = 50; // Potencia deseada (0-100%)

// Definiciones de bits para control rápido de pines (según tus archivos)
#define sbi(port, bit) (port) |= (1 << (bit))
#define cbi(port, bit) (port) &= ~(1 << (bit))

void setup() {
  Serial.begin(57600);
  pinMode(pinZCD, INPUT);
  pinMode(pinPot, INPUT);

  // Configurar pines de salida para el Optotriac (D1 y B6)
  sbi(DDRD, DDD1);
  sbi(DDRB, DDB6);

  Serial.println("Sistema de Control de Fase Listo");
  Serial.println("Introduce un porcentaje (0-100) en el Monitor Serie:");
}

void loop() {
  // 1. Leer el porcentaje desde el monitor serie
  if (Serial.available() > 0) {
    int input = Serial.parseInt();
    if (input >= 0 && input <= 100) {
      porcentaje = input;
      Serial.print("Potencia actualizada a: ");
      Serial.print(porcentaje);
      Serial.println("%");
    }
    // Limpiar el buffer por si acaso
    while (Serial.available() > 0)
      Serial.read();
  }

  // 2. Detectar el paso por cero
  int lectura = analogRead(pinZCD);

  if (lectura > 900) { // Detectado el pulso de paso por cero

    // 3. CALCULAR ESPERA (Phase Delay)
    // Un semiciclo de 50Hz dura 10ms (10000 us).
    long espera = map(porcentaje, 0, 100, 9500, 0);

    if (porcentaje > 0) {
      if (espera > 0)
        delayMicroseconds(espera);

      // 4. DISPARO (40us)
      sbi(PORTD, PORTD1);
      sbi(PORTB, PORTB6);

      delayMicroseconds(40);

      // 5. APAGAR DISPARO
      cbi(PORTD, PORTD1);
      cbi(PORTB, PORTB6);
    }

    // Pequeña espera para no detectar el mismo pulso varias veces
    while (analogRead(pinZCD) > 800)
      ;
  }
}