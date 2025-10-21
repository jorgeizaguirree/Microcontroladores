int v1 = 0;
int v2 = 132;
int v3 = 310;
int v4 = 481;
int v5 = 720;
int v6 = 1023;
int valor;
int fi;
#define sbi(port, bit) (port) |= (1 << (bit))
#define cbi(port, bit) (port) &= ~(1 << (bit))
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(57600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(A0, INPUT);
}
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


void Teclado_Libre() {
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

int Repetir_Tecla(int tecla, int espera){
  int boton 255;
  int Fi = 0;
  int j;
  int valor;

  for(j = 0; j <= espera; j++){
    valor = analogRead(A0);
    if (valor >= 0 && valor < 5){
      if (Fi ++ == espera )
    }
  }
}

int Leer_Teclado() {
  valor = v6;
  fi = 10;
  int boton = 5;
  while ((valor = analogRead(A0)) > media(v5, v6)) {
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


// the loop function runs over and over again forever
void loop() {
  int boton;

  while (true) {
    Serial.print("Se ha pulsado la tecla: ");
    Serial.print(boton = Leer_Teclado());
    Serial.print(" (");
    Serial.print(valor);
    Serial.println(") ... Buscando siguiente tecla");

    Teclado_Libre();
  }
}