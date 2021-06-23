//Programa que mide la distancia mediante 4 sensores ultrasonicos HC-SR04 y
//envia los datos a un servidor mediante mqtt.

//Materiales:
//4 Sensores ultrasonicos HC-SR04
//Placa MCU ESP8266Wifi
//Jumpers H-H

//Conexion:
//Vcc: 5v
//Trig: D1
//EchoPin: D0
//Gnd: G

//Vin modificado para que saque 5v y poder conectar


#include <Arduino.h>
//#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>


const int triggerPin = 16; //D0
const int echoPin = 5; //D1 long duracion;
float distancia;
float duracion;

const int triggerPin2 = 4; //D2
const int echoPin2 = 0; //D3 long duracion;
float distancia2;
float duracion2;

const int triggerPin3 = 2; //D4
const int echoPin3 = 14; //D5 long duracion;
float distancia3;
float duracion3;


const int triggerPin4 = 12; //D6
const int echoPin4 = 13; //D7 long duracion;
float distancia4;
float duracion4;


String mensajeSalida;//MENSAJE DONDE CONCATENA LA SALIDA A MOSTRAR

//Mensajes de cada sensor
String msgSensor1, msgSensor2, msgSensor3, msgSensor4;

//Concatenación de todos los mensajes
String mensajeCompleto;

bool bandSensor1 = false;
bool bandSensor2 = false;
bool bandSensor3 = false;
bool bandSensor4 = false;

//====================================
//           WIFICONFIG
//====================================
const char* ssid = "ARRIS-EFF2";
const char* password =  "GearS2255";




//====================================
//             MQTT CONFIG
//====================================

const char *mqtt_server = "35.239.161.254";
//const char *mqtt_server = "192.168.0.13";
const int mqtt_port = 1883;
const char *root_topic_subscribe = "casa/ultra/input";
const char *root_topic_publish = "casa/ultra/output";
const char *mqtt_user = "";
const char *mqtt_pass = "";



//====================================
//               GLOBALES
//====================================
WiFiClient espClient;
PubSubClient client(espClient);




//====================================
//           F U N C I O N E S
//====================================
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();;

void setup() {


  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(triggerPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(triggerPin3, OUTPUT);
  pinMode(echoPin3, INPUT);

  pinMode(triggerPin4, OUTPUT);
  pinMode(echoPin4, INPUT);

  Serial.begin(115200);


  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}



void loop() {

  mensajeSalida = "";

  if (client.connected() == true && bandSensor1 == true) {
    mensajeSalida += sensor1();
  }

  if (client.connected() == true && bandSensor2 == true) {
    if (mensajeSalida == "") {
      mensajeSalida += sensor2();
    } else {
      mensajeSalida += "," + sensor2();
    }
  }

  if (client.connected() == true && bandSensor3 == true) {
    if (mensajeSalida == "") {
      mensajeSalida += sensor3();
    } else {
      mensajeSalida += "," + sensor3();
    }
  }

  if (client.connected() == true && bandSensor4 == true) {
    if (mensajeSalida == "") {
      mensajeSalida += sensor4();
    } else {
      mensajeSalida += "," + sensor4();
    }
  }

  if (client.connected() == true && mensajeSalida != "") {
    client.publish(root_topic_publish, mensajeSalida.c_str());
  }

  if (!client.connected()) {
    Serial.print("Reconectando ...");
    reconnect();
  }

  delay(1000);
  client.loop();
}



//====================================
//           CONEXION WIFI
//====================================
void setup_wifi() {
  delay(10);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//====================================
//             CONEXION MQTT
//====================================

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "ESP-UV_";
    clientId += String(random(0xffff), HEX) ;
    // Intentamos conectar
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      // Nos suscribimos



      if (client.subscribe(root_topic_subscribe)) {
        Serial.println("Suscripcion ok");
      } else {
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló :( con error ===> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}


//====================================
//              CALLBACK
//====================================
void callback(char* topic, byte* payload, unsigned int length) {

  byte* p = (byte*)malloc(length);
  memcpy(p, payload, length);

  Serial.print("Se recibió mensaje:  ");
  Serial.print(topic);
  Serial.println("");

  String incoming = (char*)p;
  incoming.trim();


  String incomingVaidated = "";
  for (int i = 0; i <= 1; i++) {
    incomingVaidated += (char)incoming[i];
  }

  Serial.println("Mensaje  ==>  " + incomingVaidated + "\n\n");


  //======================================
  // APAGADO/ENCENDIDO TODO LOS SENSORES
  //======================================
  if (incomingVaidated == "1") {
    bandSensor1 = true;
    bandSensor2 = true;
    bandSensor3 = true;
    bandSensor4 = true;
  }

  if (incomingVaidated == "0") {
    bandSensor1 = false;
    bandSensor2 = false;
    bandSensor3 = false;
    bandSensor4 = false;
  }

  //============================
  // APAGADO/ENCENDIDO SENSOR1
  //============================
  if (incomingVaidated == "11") {
    bandSensor1 = true;
  }

  if (incomingVaidated == "01") {
    bandSensor1 = false;
  }

  //============================
  // APAGADO/ENCENDIDO SENSOR2
  //============================

  if (incomingVaidated == "12") {
    bandSensor2 = true;
  }

  if (incomingVaidated == "02") {
    bandSensor2 = false;
  }
  //============================
  // APAGADO/ENCENDIDO SENSOR3
  //============================

  if (incomingVaidated == "13") {
    bandSensor3 = true;
  }

  if (incomingVaidated == "03") {
    bandSensor3 = false;
  }

  //============================
  // APAGADO/ENCENDIDO SENSOR4
  //============================

  if (incomingVaidated == "14") {
    bandSensor4 = true;
  }

  if (incomingVaidated == "04") {
    bandSensor4 = false;
  }

  free(p);
}


//===========================
//         SENSOR 1
//===========================

String sensor1() {

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2); // Iniciar elproceso de envío de ultrasonido
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duracion = pulseIn(echoPin, HIGH);
  //Serial.print("Duración Sensor 1: ");
  //Serial.println(duracion); // Calcular la distancia
  distancia = duracion * 0.034 / 2;
  // Serial.print("Distancia Sensor 1: ");
  // Serial.println(distancia);
  delay(500);

  //msgSensor1 = "\n** SENSOR 1 *\nDuración: "+String(duracion)+"\nDistancia: "+String(distancia)+"cm\n";
  msgSensor1 = "S1:" + String(distancia);
  return msgSensor1;

}



//===========================
//         SENSOR 2
//===========================

String sensor2() {
  //SENSOR 2
  digitalWrite(triggerPin2, LOW);
  delayMicroseconds(2); // Iniciar elproceso de envío de ultrasonido
  digitalWrite(triggerPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin2, LOW);
  duracion2 = pulseIn(echoPin2, HIGH);
  //Serial.print("Duración Sensor 2: ");
  //Serial.println(duracion2); // Calcular la distancia
  distancia2 = duracion2 * 0.034 / 2;
  //Serial.print("Distancia Sesnor 2: ");
  //Serial.println(distancia2);
  delay(500);


  //msgSensor2 = "\n** SENSOR 2 *\nDuración: "+String(duracion2)+"\nDistancia: "+String(distancia2)+"cm\n";
  msgSensor2 = "S2:" + String(distancia2);
  return msgSensor2;

}




//===========================
//         SENSOR 3
//===========================

String sensor3() {

  //SENSOR 3
  digitalWrite(triggerPin3, LOW);
  delayMicroseconds(2); // Iniciar elproceso de envío de ultrasonido
  digitalWrite(triggerPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin3, LOW);
  duracion3 = pulseIn(echoPin3, HIGH);
  //Serial.print("Duración Sensor 3: ");
  //Serial.println(duracion3); // Calcular la distancia
  distancia3 = duracion3 * 0.034 / 2;
  //Serial.print("Distancia Sesnor 3: ");
  //Serial.println(distancia3);
  delay(500);

  //msgSensor3 = "\n** SENSOR 3 *\nDuración: "+String(duracion3)+"\nDistancia: "+String(distancia3)+"cm\n";
  msgSensor3 = "S3:" + String(distancia3);
  return msgSensor3;

}


//===========================
//         SENSOR 4
//===========================

String sensor4() {

  //SENSOR 4
  digitalWrite(triggerPin4, LOW);
  delayMicroseconds(2); // Iniciar elproceso de envío de ultrasonido
  digitalWrite(triggerPin4, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin4, LOW);
  duracion4 = pulseIn(echoPin4, HIGH);
  //Serial.print("Duración Sensor 4: ");
  //Serial.println(duracion4); // Calcular la distancia
  distancia4 = duracion4 * 0.034 / 2;
  //Serial.print("Distancia Sesnor 4: ");
  //Serial.println(distancia4);
  delay(500);


  //msgSensor4 = "\n** SENSOR 4 *\nDuración: "+String(duracion4)+"\nDistancia: "+String(distancia4)+"cm\n";
  msgSensor4 = "S4:" + String(distancia4);

  return msgSensor4;

}
