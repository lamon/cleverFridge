#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// wifi ------------------------------
const char mySSID[] = "someSSID";
const char myPSK[] = "somePassword";
// ------------------------------------------

// temperatura ------------------------------
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress temperatureSensor1;
// ------------------------------------------

// umidade --------------------------------
#define humidityAnalogInPin A0
// -----------------------------------------

// luminosidade -----------------------------------
const int luminosityAnalogInPin = A2;
// ----------------------------------------------

// gateway --------------------------------
const String cleverFridgeGateway = "192.168.2.15:8080";
// ----------------------------------------------

void setup()
{
  int status;
  Serial.begin(9600);

  sensors.begin();

  Serial.println("Localizando sensor de temperatura...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");

  if (!sensors.getAddress(temperatureSensor1, 0)) {
    Serial.println("Sensor de temperatura não encontrado!");
  }

  status = esp8266.begin();
  if (status <= 0) {
    Serial.println(F("Unable to communicate with shield. Looping"));
    while(1);
  }

  esp8266.setMode(ESP8266_MODE_STA);
  if (esp8266.status() <= 0) {
    if (esp8266.connect(mySSID, myPSK) < 0) {
      Serial.println(F("Error connecting"));
      while (1) ;
    }
  }

  Serial.print(F("My IP address is: "));
  Serial.println(esp8266.localIP());

  Serial.println(F("Press any key to post to Dweet!"));

}

void loop() {

  if (Serial.available()) {
      postToGateway();
  }

}

void postToGateway() {

  // temperatura ----------------------------------
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(temperatureSensor1);

  Serial.println("Temp C:");
  Serial.println(tempC);
  // --------------------------------------------

  // Create a client, and initiate a connection
  ESP8266Client client;

  Serial.println("Clever Fridge Gateway: " + cleverFridgeGateway);
  if (client.connect(cleverFridgeGateway, 80) <= 0) {
    Serial.println(F("Failed to connect to server."));
    return;
  }
  Serial.println(F("Connected."));

  float humidity = analogRead(humidityAnalogInPin);
  float luminosity = analogRead(luminosityAnalogInPin);

  // Set up our Dweet post parameters:
  String params;
  params += "temperature=" + String(tempC) + "&";
  params += "humidity=" + String(humidity) + "&";
  params += "luminosity=" + String(luminosity);

  String request = "POST /cleverFridge";
  Serial.println("REQUEST: " + request);
  client.println(request);
  client.println("Host: http://"+ cleverFridgeGateway);
  client.println("Connection: close");
  client.println();

  Serial.println(F("Reading answer..."));
  while (client.available()) {
      char c = client.read();
      //Serial.print(c);
  }

  if (client.connected()) {
    client.stop();
  }

}