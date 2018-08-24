#include <SoftwareSerial.h>
// Include the ESP8266 AT library:
#include <SparkFunESP8266WiFi.h>

//////////////////////////////
// WiFi Network Definitions //
//////////////////////////////
const char mySSID[] = "someSSID";
const char myPSK[] = "somePassword";

/////////////////////
// Dweet IO Constants //
/////////////////////
// Dweet IO server:
const String dweetServer = "dweet.io";

const String POST_TO_DWEET = "/dweet/for/";
String thing_name = "cleverFridge";

String httpHeader = "HTTP/1.1\n Host: " + dweetServer + "\n" +
                    "Connection: close\n" +
                    "Content-Type: application/x-www-form-urlencoded\n";

ESP8266Client client;

// temperatura -----------------------
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 3

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

float tempMin = 999;
float tempMax = 0;
// -----------------------------------


const int analogInPin = A0;
int luminosidade = 0;


void setup()
{
  int status;
  Serial.begin(9600);

  sensors.begin();

  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");

  if (!sensors.getAddress(sensor1, 0)) {
    Serial.println("Sensores não encontrados!");
  } else {
    Serial.print("Endereco sensor: ");
    //mostra_endereco_sensor(sensor1);
    Serial.println();
    Serial.println();
  }

  // To turn the MG2639 shield on, and verify communication
  // always begin a sketch by calling cell.begin().
  status = esp8266.begin();
  if (status <= 0)
  {
    Serial.println(F("Unable to communicate with shield. Looping"));
    while(1);
  }

  esp8266.setMode(ESP8266_MODE_STA); // Set WiFi mode to station
  if (esp8266.status() <= 0) // If we're not already connected
  {
    if (esp8266.connect(mySSID, myPSK) < 0)
    {
      Serial.println(F("Error connecting"));
      while (1) ;
    }
  }

  // Get our assigned IP address and print it:
  Serial.print(F("My IP address is: "));
  Serial.println(esp8266.localIP());

  Serial.println(F("Press any key to post to Dweet!"));

}

void loop()
{
  // If a character has been received over serial:
  if (Serial.available())
  {
    // Post to Dweet!
    postToDweet();
    // Get from Dweet!
    // getFromDweet();

    // Then clear the serial buffer:
    //while (Serial.available())
     // Serial.read();
  }
}

void postToDweet()
{

  // temperatura ----------------------------------
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(sensor1);

  Serial.println("Temp C:");
  Serial.println(tempC);

  if (tempC > tempMax) {
    tempMax = tempC;
  }

  if (tempC < tempMin) {
    tempMin = tempC;
  }

  Serial.println("Menor temperatura:");
  Serial.println(tempMin);

  Serial.println("Maior temperatura:");
  Serial.println(tempMax);

  Serial.println("-------------------");
  // --------------------------------------------

  // Create a client, and initiate a connection
  ESP8266Client client;

  Serial.println("Dweet Server: " + dweetServer);
  if (client.connect(dweetServer, 80) <= 0)
  {
    Serial.println(F("Failed to connect to server."));
    return;
  }
  Serial.println(F("Connected."));

  float umidade = analogRead(A1);
  float luminosidade = analogRead(A0);

  // Set up our Dweet post parameters:
  String params;
  params += "temperatura=" + String(tempC) + "&";
  params += "umidade=" + String(umidade) + "&";
  params += "luminosidade=" + String(luminosidade);

  String request = "POST /dweet/for/" + thing_name + "?" + params;
  Serial.println("REQUEST: " + request);
  client.println(request);
  client.println("Host: https://www.dweet.io");
  client.println("Connection: close");
  client.println();

  Serial.println(F("Reading answer..."));
  while (client.available()) {
      char c = client.read();
      //Serial.print(c);
    }

  // connected() is a boolean return value - 1 if the
  // connection is active, 0 if it's closed.
  if (client.connected())
    client.stop(); // stop() closes a TCP connection.
}

void getFromDweet()
{
  // Create a client, and initiate a connection
  ESP8266Client client;

  Serial.println("Dweet Server: " + dweetServer);
  if (client.connect(dweetServer, 80) <= 0)
  {
    Serial.println(F("Failed to connect to server."));
    return;
  }
  Serial.println(F("Connected."));

  String request = "GET /get/latest/dweet/for/" + thing_name;
  Serial.println("REQUEST: " + request);
  client.println(request);
  client.println("Host: https://www.dweet.io");
  client.println("Connection: close");
  client.println();

  Serial.println(F("Reading answer..."));

  String response;

  while (client.available()) {
      //char c = client.read();
      response = client.readString();
      //Serial.print(c);
  }

  //Serial.println("RESPONSE: " + response);
  // available() will return the number of characters
  // currently in the receive buffer.
  //while (client.available()){
  //  Serial.write(client.read()); // read() gets the FIFO char
  //}

  // connected() is a boolean return value - 1 if the
  // connection is active, 0 if it's closed.
  if (client.connected())
    client.stop(); // stop() closes a TCP connection.
}
