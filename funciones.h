int ObtenerNivelConcentracionCO2(u_int8_t MQPIN){
  int ADCvalue = (float) analogRead(MQPIN);

  const float Vmax = 3.3;
  const float Vout = (ADCvalue / 4095.0) * Vmax;

  if( debug ){
    Serial.print( "MQ135 (voltaje) -> " );
    Serial.println( Vout );
  }

  // Rs = Resistencia del sensor en presencia de gas.
  const float Rs= ((Vmax / Vout) - 1) * RL;
  
  if( debug ){
    Serial.print( "MQ135 (Rs) -> " );
    Serial.println( Rs );
  }

  // Se aplica la fórmula para estimar el nivel de concentración.
  const float concentracion = A * ( pow((Rs/Ro),B) );

  if( debug ){
    Serial.print( "MQ135 (concentracion) -> " );
    Serial.println( concentracion );
  }

  return (int)concentracion;
}

//====================================
const char* serverHOST = "http://192.168.10.1";
const short serverPORT = 4040;
const char* serverSSID = "airasphealthy";
const char* serverPASS = "raspados1108";
const char* serverENDPOINT = "/medicion";
const char* serverAUTHASH = "9dae27a1e26d7445e30ec3fa4b722667";

short timeout = 15;
short count = 0;
bool HayConexion = false;
//====================================
bool ConectarseAlServidor(){
  // Conectarse a red WiFi
  WiFi.begin(serverSSID, serverPASS);

  // Esperar hasta que la conexión sea exitosa
  while(WiFi.status() != WL_CONNECTED){
    count++;
    delay(1000);
    Serial.print(".");

    if( WiFi.status() == WL_CONNECTED ){
      HayConexion = true;
      break;
    }else{
      if( count == timeout ){
        HayConexion = false;
        break;
      } 
    }
  }

  Serial.print("\n");

  return HayConexion;
}

void EnviarDatosalServidor(int co2, int temperature, int humidity){
  if( debug ){
    Serial.print("HOST (DESTINO DE LOS DATOS) -> ");
    Serial.println( serverHOST );
  }

  // Crear un objeto JSON
  DynamicJsonDocument doc(1024);
  doc["co2"] = co2;
  doc["temp"] = temperature;
  doc["hum"] = humidity;

  // Serializar el objeto JSON a una cadena
  String jsonString;
  serializeJson(doc, jsonString);

  // Crear una instancia de HTTPClient
  HTTPClient http;

  // Configurar la dirección del servidor y el punto de enlace
  String serverAddress = String(serverHOST) + ":" + String(serverPORT) + String(serverENDPOINT);
  http.begin(serverAddress);

  // Establecer encabezados
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", String("Bearer ") + serverAUTHASH);

  // Realizar la solicitud POST con los datos JSON
  int httpResponseCode = http.POST(jsonString);

  // Verificar la respuesta del servidor
  if (httpResponseCode > 0) {
    Serial.println("> Se han enviado los datos al servidor.");
  } else {
    if( httpResponseCode == -1 ){
      Serial.print("> No se pudo realizar una solicitud con el servidor a ");
      Serial.println(serverENDPOINT);
    }else{
      Serial.print("> Hubo un error en la solicitud al servidor. Código de error: ");
      Serial.println(httpResponseCode);
    }
  }

  // Cerrar la conexión
  http.end();
}
