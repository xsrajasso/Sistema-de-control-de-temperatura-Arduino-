const int TemperaturaPin=A0, PWMPin = 5;
float TemperaturaActual, TemperaturaDeseada, Error, FlotantePWM, PorcentajePWM;
int LecturaA0, i, PWM;
float TemperaturaAnterior, Frio, Normal, Caliente, Pendiente, Bajando, Estable, Subiendo;
float Regla1, Regla2, Regla3, Regla4, Regla5, Regla6, Regla7, Regla8, Regla9;
float Incrementar, Mantener, Decrementar, Cambio;


void setup() {
  // put your setup code here, to run once:
  analogWrite (PWMPin, 0);
  TemperaturaDeseada = 70;
  Serial.begin(9600);
  Serial.print(0);
  Serial.print(" ");
  Serial.print(60);
  Serial.print(" ");
}

void loop() {

  // ENTRADA
  TemperaturaAnterior = TemperaturaActual;

  LecturaA0 = 0;
  for (i=0;i<20;i++) {
    LecturaA0 += analogRead(TemperaturaPin);
    delay(100);
  }
  TemperaturaActual = (LecturaA0 / 1023.0) * 5 * 100/20;

 
  Error = TemperaturaActual - TemperaturaDeseada;
  Pendiente = (TemperaturaActual - TemperaturaAnterior) / 2;

  // FUZZIFICACION
  if (Error <= -5) {
    Frio = 1;
    Normal = 0;
    Caliente = 0;
  }
  if ((Error > -5) && (Error < 0)) {
    Caliente = 0;
    Normal = (5 - abs(Error)) / 5;
    Frio = 1 - Normal;
  }
  if ((Error > 0) && (Error < 5)) {
    Frio = 0;
    Normal = (5 - abs(Error)) / 5;
    Caliente = 1 - Normal;
  }
  if (Error >= 5) {
    Frio = 0;
    Normal = 0;
    Caliente = 1;
  }
if (Error == 0) {
    Frio = 0;
    Normal = 1;
    Caliente = 0;
  }

  // PENDIENTE
  if (Pendiente <= -0.3) {
    Bajando = 1;
    Estable = 0;
    Subiendo = 0;
  }
  if ((Pendiente > -0.3) && (Pendiente < 0)) {
    Subiendo = 0;
    Estable = (0.3 - abs(Pendiente)) / 0.3;
    Bajando = 1 - Estable;
  }
  if ((Pendiente > 0) && (Pendiente < 0.3)) {
    Bajando = 0;
    Estable = (0.3 - abs(Pendiente)) / 0.3; 
    Subiendo = 1 - Estable;
  }
  if (Pendiente >= 0.3) {
    Bajando = 0;
    Estable = 0;
    Subiendo = 1;
  }
  if (Pendiente == 0) {
    Bajando = 0;
    Estable = 1;
    Subiendo = 0;
  }

  // EVALUACION
  Regla1 = min(Frio, Bajando);       // Incrementar
  Regla2 = min(Frio, Estable);       // Incrementar
  Regla3 = min(Frio, Subiendo);      // Mantener
  Regla4 = min(Normal, Bajando);     // Incrementar
  Regla5 = min(Normal, Estable);     // Mantener
  Regla6 = min(Normal, Subiendo);    // Decrementar
  Regla7 = min(Caliente, Bajando);   // Mantener
  Regla8 = min(Caliente, Estable);   // Decrementar
  Regla9 = min(Caliente, Subiendo);  // Decrementar

  // CONSOLIDACION
  Incrementar = sqrt(sq(Regla1) + sq(Regla2) + sq(Regla4));
  Mantener = sqrt(sq(Regla3) + sq(Regla5) + sq(Regla7));
  Decrementar = sqrt(sq(Regla6) + sq(Regla8) + sq(Regla9));

  // DEFUZZIFICACION
  Cambio = ((Decrementar * -6) + (Mantener * 0) + (Incrementar * 6)) / (Decrementar + Mantener + Incrementar);

  // SALIDA
  FlotantePWM = FlotantePWM + Cambio;

  if (FlotantePWM > 255) {FlotantePWM=255;}
  if (FlotantePWM < 0) {FlotantePWM=0;}
  PWM = FlotantePWM;
  analogWrite (PWMPin, PWM);
  PorcentajePWM = (PWM / 255.0) * 100.0;

  Serial.print(TemperaturaActual);
  Serial.print(" ");
  Serial.print(TemperaturaDeseada);
  Serial.print(" ");
  Serial.print(TemperaturaDeseada-5);
  Serial.print(" ");
  Serial.print(TemperaturaDeseada+5);
  Serial.print(" ");
  Serial.println(PorcentajePWM);

}
