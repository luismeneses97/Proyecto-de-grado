//MODULO DE RELOJ
#include <ThreeWire.h>  
#include <RtcDS1302.h>

ThreeWire myWire(20,21,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
//SENSOR DE CAUDAL
unsigned long intervalo1=20000;
unsigned long tiempoanterior1=0;

unsigned long intervalo_establece_flanco=60000;
unsigned long tiempo_anterior_para_flanco=0;

unsigned long intervalo_rangos_de_humedad=120000;
unsigned long tiempo_anterior_para_rango=0;
int num=0;
long dt=0;
long t0=0;
volatile int Pulsos=0; //variable para la cantidad de pulsos recibidos
int PinSensor = 3;    //Sensor conectado en el pin 3
float factor_conversion=7.288; //para convertir de frecuencia a caudal
float volumen=0;

//FUNCION QUE USA LA FUNCION INTERRUPCION() PARA CONTAR LOS PULSOS
void ContarPulsos (){ 
  Pulsos++;  
} 

// FUNCION PARA OBTENER LA FRECUENCIA DE LOS PULSOS
int obtener_frecuecia(){
  int frecuencia;
  Pulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Deshabilitamos  las interrupciones
  frecuencia=Pulsos; //Hz(pulsos por segundo)
  return frecuencia;
}

// SENSOR TEMPERATURA DS18B20
#include <OneWire.h>                
#include <DallasTemperature.h>
OneWire ourWire(40); //Pin digital para conectar la senal del sensor.               
DallasTemperature sensor(&ourWire); 

// SENSOR DE TEMPERATURA Y HUMEDAD ATMOSFERICA SHT10
#include <SHT1x.h>
// Specify data and clock connections and instantiate SHT1x object
#define dataPin  46
#define clockPin 47
SHT1x sht10(dataPin, clockPin);

// PINES DE RELES
int electrovalvula_1=8;
int electrovalvula_2=9;
int electrovalvula_3=10;

// VARIABLES DE LAS ELECTROVALVULAS
boolean estado_electrovalvula_1 = false;
boolean estado_electrovalvula_2 = false;
boolean estado_electrovalvula_3 = false;

boolean ascendente_electrovalvula_1 = true;
boolean ascendente_electrovalvula_2 = true;
boolean ascendente_electrovalvula_3 = true;

// VARIABLES DE LOS SENSORES DE HUMEDAD YL-69
float humedad_era_1_antes = 0;
float humedad_era_2_antes = 0;
float humedad_era_3_antes = 0;
boolean ascendente = false;

// FUNCION QUE CALCULA LA DURACION DEL CICLO
unsigned long tiempoOne=0;
unsigned long tiempoTwo=0;
void duracion_de_ciclo(){
  tiempoTwo=millis();
  Serial.println(tiempoTwo-tiempoOne);
  tiempoOne=millis();
}

// FUNCION QUE DETERMINA SI LA ELECTROVALVULA SE ENCIENDE
boolean estado_electrovalvula(float humedad_era, boolean flanco_ascendente, float rango_de_humedad_mayor, float rango_de_humedad_menor){
  boolean electrovalvula;
  if(flanco_ascendente == true && humedad_era <= rango_de_humedad_mayor){
    electrovalvula = true;
  }
  else if(flanco_ascendente == true && humedad_era > rango_de_humedad_mayor){
    electrovalvula = false; 
  }
  else if(flanco_ascendente == false && humedad_era < rango_de_humedad_menor){
    electrovalvula = true;
  }
  else{
    electrovalvula = false;
  } 
  return electrovalvula;
 }

// FUNCION QUE DETERMINA SI SE ESTA EN FLANCO ASCENDENTE O DESENDIENTE
boolean estado_flanco_ascendente(float humedad_era_antes,float humedad_era){
  float resta = humedad_era_antes-humedad_era;
  boolean ascendente;
  if(resta<=(0)){
    ascendente = true;
  }else{
    ascendente = false;
  } 
  return ascendente;
}

//*****PRUEBA DE COMUNICACION*******
String rangos_de_humedad = "7060";

int led1=40;
String rango_humedad_menor = "90";
String rango_humedad_mayor = "50";
//
float rango_de_humedad(float humedad_era, float humedad_mayor_inicial, float humedad_menor_inicial){

    float valor_final = (-0.014*(humedad_era*humedad_era))+(0.33*humedad_era)+(98.091);
    float resta = valor_final - humedad_mayor_inicial;
    float rango_humedad_mayor = humedad_mayor_inicial - resta;
    //float rango_humedad_menor = humedad_menor_inicial - resta;
  
    //rangos[0] = rango_humedad_mayor;
    //rangos[1] = rango_humedad_menor;
    return rango_humedad_mayor;
 }

//
  float rango_humedad_mayor_era_1;
  float rango_humedad_menor_era_1; 

  float rango_humedad_mayor_era_2;
  float rango_humedad_menor_era_2; 

  float rango_humedad_mayor_era_3;
  float rango_humedad_menor_era_3; 

//VOID SETUP  
void setup(){
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led1,OUTPUT);
  digitalWrite(led1, HIGH);
  pinMode(PinSensor, INPUT); 
  attachInterrupt(digitalPinToInterrupt(PinSensor),ContarPulsos,RISING);//(Interrupción 0(Pin2),función,Flanco de subida)
  sensor.begin(); //para sensor de temperatura DS18B20
  t0=millis();
  
  //RELES
  pinMode(electrovalvula_1,OUTPUT);
  pinMode(electrovalvula_2,OUTPUT);
  pinMode(electrovalvula_3,OUTPUT);
  tiempoOne=millis();
  //MODULO DE RELOJ
  //Rtc.Begin();
  //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
}

//VOID LOOP
void loop(){
// SE RECIBE EL VALOR DEL RANGO DE HUMEDAD DESDE LABVIEW
if (Serial.available())
   {
      rangos_de_humedad = Serial.readStringUntil('\n'); 
   }
   if(rangos_de_humedad.length()>=4){ //la variable var1 toma dos espacios de mas aumentando su longitud 2 veces 
      rango_humedad_mayor = (String)rangos_de_humedad[0]+(String)rangos_de_humedad[1]; 
      rango_humedad_menor = (String)rangos_de_humedad[2]+(String)rangos_de_humedad[3]; 


    }else if(rangos_de_humedad.length()==3){ //la variable var1 toma dos espacios de mas aumentando su longitud 2 veces 
      rango_humedad_mayor = (String)rangos_de_humedad[0]+(String)rangos_de_humedad[1]; 
      rango_humedad_menor = (String)rangos_de_humedad[2]; 
      
    }else if(rangos_de_humedad.length()<=2){
      rango_humedad_mayor = (String)rangos_de_humedad[0]; 
      rango_humedad_menor = (String)rangos_de_humedad[1]; 
      
    }
    
   /* if(rango_humedad_mayor=="40" && rango_humedad_menor=="20")
      {digitalWrite(led1, LOW);}
      else{digitalWrite(led1, HIGH);}
    */

//TEMPERATURA DS18B20
//tiempo de respuesta:534
  sensor.requestTemperatures();   
  float temperatura_DS18B20= sensor.getTempCByIndex(0); 

//HUMEDAD ATMOSFERICA Y TEMPERATURA SHT10 
/*tiempo de sht1x.readTemperatureC: 411
  tiempo de sht1x.readTemperatureF: 411
  tiempo de sht1x.readHumidity(): 642
*/
/*  
  float humedad_atmosferica_SHT10 = sht10.readHumidity();
  float temperatura_SHT10 = sht10.readTemperatureC();
*/  

//HUMEDAD
  float sensor_humedad_1 = (analogRead(A1)*(-100.0/643.0))+159.0;
  float sensor_humedad_2 = (analogRead(A2)*(-100.0/643.0))+159.0;
  float sensor_humedad_3 = (analogRead(A3)*(-100.0/643.0))+159.0;
  float sensor_humedad_4 = (analogRead(A4)*(-100.0/643.0))+159.0;
  float sensor_humedad_5 = (analogRead(A5)*(-100.0/643.0))+159.0;
  float sensor_humedad_6 = (analogRead(A6)*(-100.0/643.0))+159.0;
  float sensor_humedad_7 = (analogRead(A7)*(-100.0/643.0))+159.0;
  float sensor_humedad_8 = (analogRead(A8)*(-100.0/643.0))+159.0;
  float sensor_humedad_9 = (analogRead(A9)*(-100.0/643.0))+159.0;
  float humedad_era_1 = (sensor_humedad_1+sensor_humedad_2+sensor_humedad_3)/3 ;
  float humedad_era_2 = (sensor_humedad_4+sensor_humedad_5+sensor_humedad_6)/3 ;
  float humedad_era_3 = (sensor_humedad_7+sensor_humedad_8+sensor_humedad_9)/3 ;

  //humedad_era_1 = sensor_humedad_1;
//  humedad_era_2 = sensor_humedad_4;
//  humedad_era_3 = sensor_humedad_7;

 //****CONTROL DE ELECTROVALVULAS******
  unsigned long tiempo_actual_para_flanco=millis();
  if((unsigned long)(tiempo_actual_para_flanco-tiempo_anterior_para_flanco)>=intervalo_establece_flanco){

 
    
    ascendente_electrovalvula_1 = estado_flanco_ascendente(humedad_era_1_antes, humedad_era_1);  
    ascendente_electrovalvula_2 = estado_flanco_ascendente(humedad_era_2_antes, humedad_era_2);  
    ascendente_electrovalvula_3 = estado_flanco_ascendente(humedad_era_3_antes, humedad_era_3);  

    //Guardo el valor de la humedad
    humedad_era_1_antes = humedad_era_1;
    humedad_era_2_antes = humedad_era_2;
    humedad_era_3_antes = humedad_era_3;
    tiempo_anterior_para_flanco=millis();
    //duracion_de_ciclo(); 
  }
  /*int cont = 0;
  String rango_humedad_mayor_inicial;
  String rango_humedad_menor_inicial;
  //Modifico el rango de humedad a mantener de acuerdo a la funcion de correcion
  if (cont<1){
    rango_humedad_mayor_inicial = rango_humedad_mayor;
    rango_humedad_menor_inicial = rango_humedad_menor;
    cont += 1;
  }*/
  //


  
  if(num < 3){
    rango_humedad_mayor_era_1 = rango_humedad_mayor.toFloat();
    rango_humedad_menor_era_1 = rango_humedad_menor.toFloat();

    rango_humedad_mayor_era_2 = rango_humedad_mayor.toFloat();
    rango_humedad_menor_era_2 = rango_humedad_menor.toFloat(); 

    rango_humedad_mayor_era_3 = rango_humedad_mayor.toFloat();
    rango_humedad_menor_era_3 = rango_humedad_menor.toFloat(); 
  }
  num = num + 1;
  unsigned long tiempo_actual_para_rango=millis();
  if((unsigned long)(tiempo_actual_para_rango-tiempo_anterior_para_rango)>=intervalo_rangos_de_humedad){
 
  // En la era 1
  float rangos_era_1[2];
  rangos_era_1[0]=rango_humedad_mayor.toFloat();
  rangos_era_1[1]=rango_humedad_menor.toFloat();
  rango_humedad_mayor_era_1 = rango_humedad_mayor.toFloat();
  rango_humedad_menor_era_1 = rango_humedad_menor.toFloat(); 
 
  if (humedad_era_1 < rango_humedad_mayor.toFloat()){
  rango_humedad_mayor_era_1 = rango_de_humedad(humedad_era_1, rango_humedad_mayor.toFloat(), rango_humedad_menor.toFloat());
  }
  if (humedad_era_1 > rango_humedad_menor.toFloat()){
  rango_humedad_mayor_era_1 = rango_humedad_mayor_era_1 - 9;
  //rango_humedad_mayor_era_1 = rangos_era_1[0];
  //rango_humedad_menor_era_1 = rangos_era_1[1]; 
  
  }
  
    // En la era 2
  float rangos_era_2[2];
  rangos_era_2[0]=rango_humedad_mayor.toFloat();
  rangos_era_2[1]=rango_humedad_menor.toFloat();
  rango_humedad_mayor_era_2 = rango_humedad_mayor.toFloat();
  rango_humedad_menor_era_2 = rango_humedad_menor.toFloat(); 
  
  if (humedad_era_2 < rango_humedad_mayor.toFloat()){
  rango_humedad_mayor_era_2 = rango_de_humedad(humedad_era_2, rango_humedad_mayor.toFloat(), rango_humedad_menor.toFloat());
  }
  if (humedad_era_2 > rango_humedad_menor.toFloat()){
  rango_humedad_mayor_era_2 = rango_humedad_mayor_era_2 - 9;
  //rango_humedad_mayor_era_2 = rangos_era_2[0];
  //rango_humedad_menor_era_2 = rangos_era_2[1]; 
  }
    // En la era 3
  float rangos_era_3[2];
  rangos_era_3[0]=rango_humedad_mayor.toFloat();
  rangos_era_3[1]=rango_humedad_menor.toFloat();
  rango_humedad_mayor_era_3 = rango_humedad_mayor.toFloat();
  rango_humedad_menor_era_3 = rango_humedad_menor.toFloat(); 
  
  if (humedad_era_3 < rango_humedad_mayor.toFloat()){
  rango_humedad_mayor_era_3 = rango_de_humedad(humedad_era_3, rango_humedad_mayor.toFloat(), rango_humedad_menor.toFloat());
  }
  if (humedad_era_3 > rango_humedad_menor.toFloat()){
  rango_humedad_mayor_era_3 = rango_humedad_mayor_era_3 - 9;
  //rango_humedad_mayor_era_3 = rangos_era_3[0];
  //rango_humedad_menor_era_3 = rangos_era_3[1];
  }
  tiempo_anterior_para_rango=millis(); 
  
  }

  /*rango_humedad_menor_era_1 = rango_humedad_menor_era_1 - 9;
  rango_humedad_menor_era_2 = rango_humedad_menor_era_2 - 9;
  rango_humedad_menor_era_3 = rango_humedad_menor_era_3 - 9;
  */
  estado_electrovalvula_1 = estado_electrovalvula(humedad_era_1,ascendente_electrovalvula_1, rango_humedad_mayor_era_1, rango_humedad_menor_era_1);
  estado_electrovalvula_2 = estado_electrovalvula(humedad_era_2,ascendente_electrovalvula_2, rango_humedad_mayor_era_2, rango_humedad_menor_era_2);
  estado_electrovalvula_3 = estado_electrovalvula(humedad_era_3,ascendente_electrovalvula_3, rango_humedad_mayor_era_3, rango_humedad_menor_era_3);

  
  if(estado_electrovalvula_1 == true){
    digitalWrite(electrovalvula_1,HIGH);
  }else{
    digitalWrite(electrovalvula_1,LOW);
  }
  
  if(estado_electrovalvula_2 == true){
    digitalWrite(electrovalvula_2,HIGH);
  }else{
    digitalWrite(electrovalvula_2,LOW);
  }
  
  if(estado_electrovalvula_3 == true){
    digitalWrite(electrovalvula_3,LOW);
  }else{
    digitalWrite(electrovalvula_3,HIGH);
  }

//SENSOR DE CAUDAL

  float frecuencia = obtener_frecuecia(); //obtenemos la frecuencia de los pulsos en Hz
  float caudal_L_m = frecuencia/factor_conversion; //calculamos el caudal en L/m
  dt = millis()-t0; //calculamos la variación de tiempo
  t0 = millis();
  volumen=volumen+(caudal_L_m/60)*(dt/1000); // volumen(L)=caudal(L/s)*tiempo(s)
//MODULO DE RELOJ
RtcDateTime now = Rtc.GetDateTime();



//SE IMPRIMEN TODAS LAS FUNCIONES  
  unsigned long tiempoactual1=millis();
  if((unsigned long)(tiempoactual1-tiempoanterior1)>=intervalo1){
    Serial.print((String)volumen+";");
    Serial.print((String)caudal_L_m+";");
    Serial.print((String)temperatura_DS18B20+";");
    Serial.print((String)200+";");
    //Serial.print((String)tempeeratura_SHT10+";");
    //Serial.print((String)humedad_atmosferica_SHT10+";");
    Serial.print((String)sensor_humedad_1+";");
    Serial.print((String)sensor_humedad_2+";");
    Serial.print((String)sensor_humedad_3+";");
    Serial.print((String)sensor_humedad_4+";");
    Serial.print((String)sensor_humedad_5+";");
    Serial.print((String)sensor_humedad_6+";");
    Serial.print((String)sensor_humedad_7+";");
    Serial.print((String)sensor_humedad_8+";");
    Serial.print((String)sensor_humedad_9+";");
    Serial.print((String)estado_electrovalvula_1+";");
    Serial.print((String)estado_electrovalvula_2+";");
    Serial.print((String)estado_electrovalvula_3+";");
    Serial.print((String)rango_humedad_mayor_era_1+";");
    Serial.print((String)rango_humedad_menor_era_1+";");
    Serial.print((String)now.Year()+";");
    Serial.print((String)now.Month()+";");
    Serial.print((String)now.Day()+";");
    Serial.println((String)now.Hour());
    

    tiempoanterior1=millis();
    //duracion_de_ciclo(); 
  }
 
}
