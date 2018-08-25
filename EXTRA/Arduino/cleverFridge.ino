#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// wifi ------------------------------------
const char mySSID[] = "WLL-Inatel";
const char myPSK[] = "inatelsemfio";
// ------------------------------------------

// temperatura ------------------------------
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress temperatureSensor1;
// ------------------------------------------

// umidade --------------------------------
#define humidityAnalogInPin A0
// ----------------------------------------

// luminosidade ---------------------------------
const int luminosityAnalogInPin = A2;
// ----------------------------------------------

// leds wearable ---------------------------------
#define PIN 5
#define PIXELS 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);
// ----------------------------------------------

// gateway --------------------------------
const String cleverFridgeGateway = "10.10.1.13";
// ----------------------------------------------

void setup()
{
  int status;
  Serial.begin(9600);

  // leds off
  strip.begin();
  strip.show(); // all pixels to off
  // -----

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
    Serial.println(F("Não foi possível comunicar com o shield wifi. Looping"));
    while(1);
  }

  esp8266.setMode(ESP8266_MODE_STA);
  if (esp8266.status() <= 0) {
    if (esp8266.connect(mySSID, myPSK) < 0) {
      Serial.println(F("Error connecting"));
      while (1) ;
    }
  }

  Serial.print(F("IP address: "));
  Serial.println(esp8266.localIP());

  Serial.println(F("Pressione qualquer tecla para iniciar ..."));

}

void loop() {

  if (Serial.available()) {
      postToGateway();
  }

  delay(5000);

}

void colorWipe(uint32_t c, uint8_t wait) {

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }

}

void postToGateway() {

  // temperatura ----------------------------------
  sensors.requestTemperatures();
  float temperature = sensors.getTempC(temperatureSensor1);
  int tempAsInt = (int) temperature;

  Serial.println("Temperatura C:");
  Serial.println(temperature);
  // --------------------------------------------

  // umidade ----------------------------------
  float humidity = analogRead(humidityAnalogInPin);
  int humidityAsInt = (int) humidity;

  Serial.println("Umidade:");
  Serial.println(humidity);
  // --------------------------------------------

  // luminosidade e leds -----------------------------
  float luminosity = analogRead(luminosityAnalogInPin);
  int luminosityAsInt = (int) luminosity;

  Serial.println("Luminosidade:");
  Serial.println(luminosity);

  // se a luminosidade aumentar, porta abriu, acende LED
  if (luminosity >= 600) {
    colorWipe(strip.Color(255, 0, 0), 500); // RED
    delay(1000);
  } else {
    colorWipe(strip.Color(0, 0, 255), 500); // BLUE
    delay(1000);
  }
  // --------------------------------------------

  ESP8266Client client;

  Serial.println("Clever Fridge Gateway: " + cleverFridgeGateway);
  if (client.connect(cleverFridgeGateway, 8080) <= 0) {
    Serial.println(F("Failed to connect to server."));
    return;
  }
  Serial.println(F("Connected."));

  String postData = "{\"temperature\":\"" + String(tempAsInt) + "\",\"humidity\":\"" + String(humidityAsInt) + "\",\"luminosity\":\"" + String(luminosityAsInt) + "\"}";

  client.println("POST /cleverFridge HTTP/1.1");
  client.println("Host: http://10.10.1.13:8080");
  client.println("Content-Type: application/json");
  client.println("Cache-Control: no-cache");
  client.println("Connection: close");
  client.print("Content-Length: ");
  //client.println(strlen(postData));
  client.println(postData.length());
  client.println();
  client.println(postData);

  Serial.println(F("Reading answer..."));
  while (client.available()) {
      char c = client.read();
      //Serial.print(c);
  }

  if (client.connected()) {
    client.stop();
  }

}