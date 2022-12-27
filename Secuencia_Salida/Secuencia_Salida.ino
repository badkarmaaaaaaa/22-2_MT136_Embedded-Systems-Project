#include <Servo.h>
Servo servoMotor;
int TRIG = 10;			// trigger en pin 10
int ECO = 9;			// echo en pin 9
int LED = 3;			// LED en pin 3
int DURACION;
int DISTANCIA;

void setup()
{
  pinMode(TRIG, OUTPUT); 	// trigger como salida
  pinMode(ECO, INPUT);		// echo como entrada
  pinMode(LED, OUTPUT);		// LED como salida
  Serial.begin(9600);  		// inicializacion de comunicacion serial a 9600 bps
 servoMotor.attach(6);
 digitalWrite(TRIG,LOW);
}

void loop()
{
  
  digitalWrite(TRIG, HIGH); 		// generacion del pulso a enviar
  _delay_ms(10);				// al pin conectado al trigger
  digitalWrite(TRIG, LOW);		// del sensor
  
  DURACION = pulseIn(ECO, HIGH);	// con funcion pulseIn se espera un pulso
  					// alto en Echo
  DISTANCIA = DURACION / 58.2;		// distancia medida en centimetros
  Serial.println(DISTANCIA);		// envio de valor de distancia por monitor serial
  delay(200);				// demora entre datos

  if (DISTANCIA <=7 ){	// si distancia entre 0 y 20 cms.
    servoMotor.write(0);
  // Esperamos 1 segundo
  delay(1000);
  
  // Desplazamos a la posición 90º
  servoMotor.write(90);
  // Esperamos 1 segundo
  delay(4000);
  
  // Desplazamos a la posición 180º
  servoMotor.write(0);
  // Esperamos 1 segundo
  delay(1000);		// apaga LED
    }

}
