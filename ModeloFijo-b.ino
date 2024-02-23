#include <WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
//====================================
#define MQ_PIN 16
#define A 116.6020682
#define B -2.769034857
#define Ro 28000.0
#define RL 10000.0
#define CO2_ATMOSFERICO 420
//====================================
#define DHT_PIN 34
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);
//====================================
const bool debug = false;
bool ResultoConexion = false;
//====================================
#include "funciones.h"
//====================================
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2200);

  // Iniciar una Conexion con el Servidor
  ResultoConexion = ConectarseAlServidor();
  
  if( ResultoConexion ){
    // Establecer lecturas de ADC en rango de 4095.0
    analogReadResolution(12);
    // Inicializar sensor DHT(11)
    dht.begin();
    // Imprimir el siguiente mensaje...
    Serial.print("> Se establecio una conexion con ");
    Serial.print( serverSSID );
    Serial.println(".");
  }else{
    // Imprimir el siguiente mensaje...
    Serial.print("> No se pudo realizar una conexion con ");
    Serial.print( serverSSID );
    Serial.println(".");
  }
}

void loop() {
  // Verificar antes de cada proceso que haya Conexion con el Servidor...
  if( HayConexion ){
    float ADCvalue = (float) analogRead(MQ_PIN);
    int temp = 0;
    int hum = 0;

    if( !isnan(dht.readTemperature()) ){
      temp = dht.readTemperature();
    }else{
      temp = 0;
    }
    if( !isnan(dht.readHumidity()) ){
      hum = dht.readHumidity();
    }else{
      hum = 0;
    }

    if( debug ){
      Serial.print("\n");

        Serial.print( "MQ135 (ADC) -> " );
        Serial.println( ADCvalue );

      if( temp > 0 && hum > 0 ){
        Serial.print( "DHT11 (temperatura) ->" );
        Serial.println( temp );

        Serial.print( "DHT11 (humedad) ->" );
        Serial.println( hum );
      }
    }
    
    const int ConcentracionCO2 = (int) ObtenerNivelConcentracionCO2(MQ_PIN);
    const int NivelCO2 = CO2_ATMOSFERICO + ConcentracionCO2;

    if( debug ){
      Serial.print( "MQ135 (CO2 ppm) -> " );
      Serial.println( NivelCO2 );
    }

    if( ResultoConexion ){
      // Enviar datos al servidor
      EnviarDatosalServidor(NivelCO2, temp, hum);
    }else{
      Serial.println("> No se envio datos al servidor.");
    }
    

    delay(30000);
  }
}
