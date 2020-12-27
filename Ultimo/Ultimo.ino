unsigned long intervalo=2000;
unsigned long intervalo1=10000;
unsigned long tiempoanterior=0;
unsigned long tiempoanterior1=0;
long dt=0;
long t0=0;
volatile int Pulsos=0; //variable para la cantidad de pulsos recibidos

int PinSensor = 3;    //Sensor conectado en el pin 2
float factor_conversion=7.288; //para convertir de frecuencia a caudal
float volumen=0;

//---Función que se ejecuta en interrupción---------------
void ContarPulsos ()  
{ 
  Pulsos++;  //incrementamos la variable de pulsos 
} 

//---Función para obtener frecuencia de los pulsos--------
int ObtenerFrecuecia() 
{
  int frecuencia;
  Pulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Deshabilitamos  las interrupciones
  frecuencia=Pulsos; //Hz(pulsos por segundo)
  return frecuencia;
}
//TEMPERATURA
#include <OneWire.h>                
#include <DallasTemperature.h>
 
OneWire ourWire(2); //Pin digital para conectar la senal del sensor.               
DallasTemperature sensor(&ourWire); 

//HUMEDAD
#include <SHT1x.h>
// Specify data and clock connections and instantiate SHT1x object
#define dataPin  10
#define clockPin 11
SHT1x sht1x(dataPin, clockPin);

//RELES
char var;
String var1;
int led1=2;
int led2=3;
int led3=4;



void setup(){
Serial.begin(115200);
pinMode(PinSensor, INPUT); 
attachInterrupt(digitalPinToInterrupt(PinSensor),ContarPulsos,RISING);//(Interrupción 0(Pin2),función,Flanco de subida)
sensor.begin(); //para sensor de temperatura
t0=millis();
//reles

pinMode(led1,OUTPUT);
pinMode(led2,OUTPUT);
pinMode(led3,OUTPUT);

}

void loop(){

//TEMPERATURA
  sensor.requestTemperatures();   
  float temp= sensor.getTempCByIndex(0); 
//HUMEDAD ATMOSFERICA
  float temp_c;
  float temp_f;
  float humidity;

  humidity = sht1x.readHumidity();

  
 



//HUMEDAD
  float val0 = (analogRead(A1)*(-100.0/1023.0))+100.0;
  float val1 = (analogRead(A2)*(-100.0/1023.0))+100.0;
  float val2 = (analogRead(A3)*(-100.0/1023.0))+100.0;
  float val3 = (analogRead(A4)*(-100.0/1023.0))+100.0;
  float val4 = (analogRead(A5)*(-100.0/1023.0))+100.0;
  float val5 = (analogRead(A6)*(-100.0/1023.0))+100.0;
  float val6 = (analogRead(A7)*(-100.0/1023.0))+100.0;
  float val7 = (analogRead(A8)*(-100.0/1023.0))+100.0;
  float val8 = (analogRead(A9)*(-100.0/1023.0))+100.0;
  


 //SENSOR DE CAUDAL

float frecuencia=ObtenerFrecuecia(); //obtenemos la frecuencia de los pulsos en Hz
float caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
dt=millis()-t0; //calculamos la variación de tiempo
t0=millis();
volumen=volumen+(caudal_L_m/60)*(dt/1000); // volumen(L)=caudal(L/s)*tiempo(s)

//SE IMPRIMEN TODAS LAS FUNCIONES  
unsigned long tiempoactual1=millis();
if((unsigned long)(tiempoactual1-tiempoanterior1)>=intervalo1){
 
 //Serial.print(" Litros:");
 Serial.print(volumen); 
 Serial.print(","); 
 Serial.print(caudal_L_m); 
 Serial.print(","); 
 //Serial.print(" Frecuencia:");Serial.print(Pulsos);
 //Serial.println(" ");
 tiempoanterior1=millis();
 Serial.println((String)temp+","+(String)humidity+","+(String)val0+","+(String)val1+","+(String)val2+","+(String)val3+","+(String)val4+","+(String)val5+","+(String)val6+","+(String)val7+","+(String)val8);
 //Volumen, Caudal, Temperatura en centigrados, Humedad de aire, Humedades del suelo 
}

 
  
  //RELES
  if(Serial.available())
  {
    var = Serial.read();
    var1+=var;
    if(var=='\n')
    {
      if(var1[0]=='a'){
        digitalWrite(led1,LOW);
      }
      else{
        digitalWrite(led1,HIGH);
      }

      if(var1[1]=='b'){
        digitalWrite(led2,LOW);
      }
      else{
        digitalWrite(led2,HIGH);
      }

      if(var1[2]=='c'){
        digitalWrite(led3,LOW);
      }
      else{
        digitalWrite(led3,HIGH);
      }

      var1="";
    }
  }



}
