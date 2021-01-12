unsigned long intervalo1=5000;
unsigned long tiempoanterior=0;
unsigned long tiempoanterior1=0;
unsigned long tiempoOne=0;
unsigned long tiempoTwo=0;
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
 
OneWire ourWire(36); //Pin digital para conectar la senal del sensor.               
DallasTemperature sensor(&ourWire); 

//HUMEDAD
#include <SHT1x.h>
// Specify data and clock connections and instantiate SHT1x object
#define dataPin  46
#define clockPin 47
SHT1x sht10(dataPin, clockPin);

//RELES
char var;
String var1;
int led1=8;
int led2=9;
int led3=10;



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
tiempoOne=millis();
}

void duracion_de_ciclo(){
   tiempoTwo=millis();
   Serial.println(tiempoTwo-tiempoOne);
   tiempoOne=millis();
}
void loop(){



//TEMPERATURA
//tiempo de respuesta:534
  sensor.requestTemperatures();   
  float temp= sensor.getTempCByIndex(0); 
  //Serial.println((String)temp);
//  duracion_de_ciclo();

  
//HUMEDAD ATMOSFERICA 
/*tiempo de sht1x.readTemperatureC: 411
  tiempo de sht1x.readTemperatureF: 411
  tiempo de sht1x.readHumidity(): 642
*/
/*  float temp_c;
  float humedad_atmosferica;
  humedad_atmosferica = sht10.readHumidity();
  temp_c = sht10.readTemperatureC();
*/  

//HUMEDAD
  float sensor_humedad_0 = (analogRead(A1)*(-100.0/1023.0))+100.0;
  float sensor_humedad_1 = (analogRead(A2)*(-100.0/1023.0))+100.0;
  float sensor_humedad_2 = (analogRead(A3)*(-100.0/1023.0))+100.0;
  float sensor_humedad_3 = (analogRead(A4)*(-100.0/1023.0))+100.0;
  float sensor_humedad_4 = (analogRead(A5)*(-100.0/1023.0))+100.0;
  float sensor_humedad_5 = (analogRead(A6)*(-100.0/1023.0))+100.0;
  float sensor_humedad_6 = (analogRead(A7)*(-100.0/1023.0))+100.0;
  float sensor_humedad_7 = (analogRead(A8)*(-100.0/1023.0))+100.0;
  float sensor_humedad_8 = (analogRead(A9)*(-100.0/1023.0))+100.0;
  float humedad_era_1 = (sensor_humedad_0+sensor_humedad_1+sensor_humedad_2)/3 ;
  float humedad_era_2 = (sensor_humedad_3+sensor_humedad_4+sensor_humedad_5)/3 ;
  float humedad_era_3 = (sensor_humedad_6+sensor_humedad_7+sensor_humedad_8)/3 ;
 // Serial.println((String)temp_c+","+(String)humidity+","+(String)val0+","+(String)val1+","+(String)val2+","+(String)val3+","+(String)val4+","+(String)val5+","+(String)val6+","+(String)val7+","+(String)val8);
  //duracion_de_ciclo();
  //1057

  //****CONTROL******
 
//SENSOR DE CAUDAL

  float frecuencia=ObtenerFrecuecia(); //obtenemos la frecuencia de los pulsos en Hz
  float caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
  dt=millis()-t0; //calculamos la variación de tiempo
  t0=millis();
  volumen=volumen+(caudal_L_m/60)*(dt/1000); // volumen(L)=caudal(L/s)*tiempo(s)


//SE IMPRIMEN TODAS LAS FUNCIONES  
  unsigned long tiempoactual1=millis();
  if((unsigned long)(tiempoactual1-tiempoanterior1)>=intervalo1){
 
   tiempoanterior1=millis();
   Serial.print((String)temp+";");
   Serial.print((String)volumen+";");
   Serial.print((String)caudal_L_m+",");
  // Serial.print((String)temp_c+",");
   //Serial.print((String)humidity+",");
   Serial.print((String)sensor_humedad_0+",");
   Serial.print((String)sensor_humedad_1+",");
   Serial.print((String)sensor_humedad_2+",");
   Serial.print((String)sensor_humedad_3+",");
   Serial.print((String)sensor_humedad_4+",");
   Serial.print((String)sensor_humedad_5+",");
   Serial.print((String)sensor_humedad_6+",");
   Serial.print((String)sensor_humedad_7+",");
   Serial.println((String)sensor_humedad_8);
   
   duracion_de_ciclo(); 

  }


  
//RELES
if(Serial.available())
  {
    var = Serial.read();
    var1+=var;
    if(var=='\n')
    {
      if(var1[0]=='a')
      {digitalWrite(led1,LOW);}
      else{digitalWrite(led1,HIGH);}

      if(var1[1]=='b')
      {digitalWrite(led2,LOW);}
      else {digitalWrite(led2,HIGH);}

      if(var1[2]=='c')
      {digitalWrite(led3,LOW);}
      else {digitalWrite(led3,HIGH);}

     
      var1="";
    }
  }
}
