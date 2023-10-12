#include <TimerOne.h>
#define VALOR_MODO_MANUAL 250
#define T_CICLO 4000
#define T_MAX   200
#define T_MIN   0
#define LAZO_C  1  
#define TEMP_INPUT   A0        //A0 coorenponde a la entrada analoga de temperatura
#define CUENTA_INPUT A1        //A0 coorenponde a la cuenta de PID
#define PID_STATE 0            //Correponde a si PID esta en uso
#define PIN_PWM 13             //Pin digital de salida de tiempod e activacionde rele
#define T_INT  1000000         //3000 //es el minimo con cierto margend e error          //Corresponde a cada cuanto tiempo sera llamada la interrupcion, el tiempo es medido en microsegundos   
#define RELE_ACTIVACION  1     // Si RELE_ACTIVACION es 1 indica que rele se activa con señal positiva, si RELE_ACTIVACION es 0 indica que rele se activa con señal negativa
#define ON    RELE_ACTIVACION == 1 ?  HIGH: LOW
#define OFF   RELE_ACTIVACION == 1 ?  LOW: HIGH
#define VEL 9600

#ifdef LAZO_C
   float mapeo(int x){ return (float) map(x, 204, 1023, T_MIN*10, T_MAX*10)/10; }
#else   
   float mapeo(int x){ return (float) map(x, 0, 1023, T_MIN*10, T_MAX*10)/10; }
#endif 

int Ta;
static char Se_Envia[6];

struct Data
{
  unsigned long tiempo_m;
  unsigned long tiempo_u;
  int tiempo_act;
  float temperatura;
};

//int contador = 0;

void Interrupcion_tiempo()
{
     struct Data datos; 
     int sensorValue = analogRead(TEMP_INPUT);
     float t = mapeo(sensorValue);
     int cuenta = analogRead(CUENTA_INPUT);
     if( PID_STATE == 1)
            Ta = map(cuenta, 0, 1023, 0, T_CICLO);
     else
            Ta = VALOR_MODO_MANUAL;
     //dtostrf(t, 5 , 1 , Se_Envia);
     datos.tiempo_u = micros();
     datos.tiempo_m = millis();
     
     datos.tiempo_act  = Ta;
     datos.temperatura = t;
     Serial.print(datos.tiempo_u);
     Serial.print(";");

     Serial.print(datos.tiempo_m);
     Serial.print(";");
     Serial.print(datos.temperatura);
     Serial.print(";");
     Serial.print(datos.tiempo_act);
     Serial.print("\n");
     //Serial.println(t);
     //contador++;
     //if( contador == 10) Serial.print("salir");
}

void setup()
{
     pinMode(PIN_PWM, OUTPUT);
     Timer1.initialize(T_INT); 
     Timer1.attachInterrupt( Interrupcion_tiempo) ;
     Serial.begin(VEL,SERIAL_8N1);
}

void loop()
{
     digitalWrite(PIN_PWM, ON); 
     delay(Ta); 
     digitalWrite(PIN_PWM, OFF); 
     delay(T_CICLO-Ta); 
}
