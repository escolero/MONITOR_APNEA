
/*  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  - Selector Start-Stop = P5
  - Boton OK = P7
  - Buzzer = P6
*/

#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)

uint16_t irBuffer[100]; 
uint16_t redBuffer[100];
#else
uint32_t irBuffer[100]; 
uint32_t redBuffer[100];  
#endif

int32_t bufferLength; 
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;
int xyz;
byte pulseLED = 11;
byte readLED = 13; 
int32_t oxinito;
int sensor = 0;
int mayor = 0;
int menor = 0;
float oxigeno;

int startstop = 5;
int pulsador = 7;
const int alarma = 6;
int botonok = 0;
int alarmaloop = 0;
int inicio = 0;
int mensajeselector = 0;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
    pinMode(startstop, INPUT);  //definir pin como entrada
  pinMode(pulsador, INPUT);  //definir pin como entrada
  pinMode(alarma, OUTPUT);  //definir pin como salida
 Serial.println("-------------------------------------------------------");
 Serial.println("--------- MONITOR DE APNEA -----------");
 Serial.println("-------------------------------------------------------");
 Serial.println("-------------------------------------------------------");
 Serial.println("- Argueta, Kevin -");
 Serial.println("- Fuentes, Edwin -");
 Serial.println("- Mendoza, Adriana -");
 Serial.println("-------------------------------------------------------");
 Serial.print("");
 Serial.println("");
 delay(1000);
 
 pinMode(pulseLED, OUTPUT);
 pinMode(readLED, OUTPUT);
 
      if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
      {
      Serial.println(F("Conecte MAX30105 y reinicie."));
      while (1);
      }
      byte ledBrightness = 60; 
      byte sampleAverage = 4;
      byte ledMode = 2;
      byte sampleRate = 100;
      int pulseWidth = 411;
      int adcRange = 4096;
      particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

}

void loop() {
inicio = digitalRead(startstop);


if (inicio==LOW)
{
  if(mensajeselector==0)
  {
    Serial.println("");
        Serial.println("Coloque selector en posicion START");
            Serial.println("");
        mensajeselector = 1;
    }
    delay(500);
  }
  
  
  
if (inicio==HIGH)
  {// INICIO IF SWITCH STARTSTOP
mensajeselector = 0;
  digitalWrite(alarma, LOW);  
//INICIO CODIGO DE FRECUENCIA
           delay(500);
           sensor = analogRead(A0);
          
           menor = sensor;
          Serial.println("Detectando pico minimo de respiración, espere...");
          for(int k = 0; k <150; k++)
          {
           delay(100);
           sensor = analogRead(A0);
           if (sensor < menor)
           {
           menor = sensor;
           }
          
           if(sensor > menor)
           {
           k = 150;
           }
           delay(100);
          }
           Serial.println("Pico minimo detectado, comenzando medición");
          
           sensor = analogRead(A0);
           //Serial.println(sensor);
           double contador = 0;
           mayor = sensor;
           Serial.println("Midiendo tiempo de exhalación");
           
          for(int i = 0; i <90; i++)
          {
           delay(100);
           sensor = analogRead(A0);
          // Serial.println(sensor);
           if (sensor > mayor)
           {
           mayor = sensor;
           }
          
           if(sensor < mayor)
           {
           i = 90;
           }
           delay(100);
           contador++;
          }
          Serial.println("Midiendo tiempo de inhalación");
           sensor = analogRead(A0);
          double contador2 = 0;
          menor = sensor;
          for(int j = 0; j <90; j++)
          {
           delay(100);
           sensor = analogRead(A0);
           if (sensor < menor)
           {
           menor = sensor;
           }
          
           if(sensor > menor)
           {
           j = 90;
           }
           delay(100);
           contador2++;
          }
          double tiempo = (contador + contador2)*0.2;
          double freq1 = 60/tiempo;
          double freq=round(freq1);
          Serial.print("");
          Serial.print("Respiraciones por minuto: "); Serial.println(freq);
          delay (1000);
          Serial.println();
//FIN CODIGO DE FRECUENCIA




      if (freq < 10)
  { // inicio if
     xyz=1;
       Serial.println("");
  Serial.println("Frecuencia respiratoria baja, se medirá SPO2");
  delay(100);
    Serial.println("");
  Serial.println("Midiendo SPO2, espere...");
  Serial.println("");
 bufferLength = 100; 
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false)
    particleSensor.check();
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); 
  }

  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  while (xyz==1)
  {
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false)
      particleSensor.check();
      digitalWrite(readLED, !digitalRead(readLED));
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();

      //Serial.print("SPO2: ");
      //Serial.println(spo2, DEC);
      oxinito=(spo2);
    }
    xyz=0;
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }
  Serial.print(("SPO2 = "));
  Serial.print(oxinito);
  delay(1000);
  Serial.println("");
xyz=0;

if (oxinito > 90)
{
      Serial.println("");

  Serial.println("Nivel de SPO2 normal");
    Serial.println("El paciente no presenta apnea");

    Serial.println("");
  delay(1000);
  }
  if (oxinito==90)
{
      Serial.println("");

  Serial.println("Nivel de SPO2 normal");
    Serial.println("El paciente no presenta apnea");

    Serial.println("");
  delay(1000);
  }
if (oxinito < 90)
{
  alarmaloop=0;
  Serial.println("");
  Serial.println("ALERTA!");
  Serial.println("Nivel de SPO2 y frecuencia respiratoria baja");
  Serial.println("El paciente presenta apnea");
  Serial.println("Mantenga presionado el boton OK para continuar");
  Serial.println("");

while (alarmaloop==0)
{
  digitalWrite(alarma, HIGH);
  delay(200);
  digitalWrite(alarma, LOW);
  delay(200);
  botonok = digitalRead(pulsador);  //lectura digital de pin
if (botonok==LOW)
{
  alarmaloop=1;
  }
digitalWrite(alarma, LOW);  
  }
  
  delay(500);
  }


  } //Fin del if
  

  }//FIN IF STARTSTOP
  }//Fin del loop
