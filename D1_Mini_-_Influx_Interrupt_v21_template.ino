/**
   Water Meter Sensor to Influx
   Version 2.1
*/

// Included Libaries
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <InfluxDbClient.h>

// WLAN Parameter
#ifndef STASSID
#define STASSID "xxxx"                                     // enter WLAN SSID
#define STAPSK  "xxxx"                                     // enter WLAN PW
#endif

// Parameter for Writing / Checking Water Meter
volatile unsigned long LastTime = millis();
volatile int lCounter = 0;
const unsigned long DeltaTime = 250;                       // defining the minimum time difference in ms between impulses

// GPIO Definiton for Water Meter Sensor
#define SENSOR_PIN 14                                      // defining the monitored GPIO PIN (default GPIO14, D5)

// InfluxDB Parameter
#define INFLUXDB_URL "http://192.168.1.2:8086"             // enter InfluxDB URL like http/https://<ip>:<port>
#define INFLUXDB_DB_NAME "db"                              // enter InfluxDB DB Name
#define INFLUXDB_USER "user"                               // enter InfluxDB User
#define INFLUXDB_PASSWORD "pw"                             // enter InfluxDB PW
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);
Point sensor("water_meter");                               // enter InfluxDB Point Name

// Board Initialization
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();

  // Enable Wifi, wait for connection and print assigned IP
  WiFi.begin(STASSID, STAPSK);
  Serial.print("WiFi connecting: ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected! IP address: ");
  Serial.println(WiFi.localIP());

  // Enable Sensor GPIO PIN and attach it to the Interrupt Handler
  pinMode(SENSOR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), sensorCheck, FALLING);
  
  // Start InfluxDB Connection
  client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME, INFLUXDB_USER, INFLUXDB_PASSWORD);
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  // Initialize the LED_BUILTIN pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
}

// Function for Sensor Check
// if last Impule was before DeltaTime (default 250ms) counter will be increased
ICACHE_RAM_ATTR void sensorCheck() {
  Serial.print("Interrupt found... ");
  const unsigned long TimeDiff = millis() - LastTime;
  Serial.print("checking time: ");
  Serial.println(TimeDiff);
  if (TimeDiff > DeltaTime) {
    LastTime =  millis();
    lCounter += 1;
    Serial.println("counter increased...");
  }
}

void loop() {
  // Write Data to InfluxDB if Wifi is connected and Sensor Counted Water Flow
  if ((WiFi.status() == WL_CONNECTED) && lCounter > 0) {
    Serial.print("writing to influx... ");
    Serial.println(millis());
    // Switch LED on to signal InfluxDB writing
    digitalWrite(LED_BUILTIN, LOW);
    // add Counter Value to InfluxDB data and reset Counter to 0
    sensor.clearFields();
    sensor.addField("l", lCounter);
    lCounter = 0;
    // show value to be written to InfluxDB
    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());
    // If no Wifi signal, try to reconnect it
    if ((WiFi.RSSI() == 0) && (WiFi.status() == WL_CONNECTED))
      Serial.println("Wifi connection lost");
    // Write point and check for failure
    if (!client.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
    // Switch LED off
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print("finished influx... ");
    Serial.println(millis());
  }
  // Reconnect WiFi if connection is lost
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.print("WiFi reconnecting: ");
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi connected! IP address: ");
    Serial.println(WiFi.localIP());
  }
  // Reset LastTime variable after millis() overflow after 50 days
  if (LastTime > millis()) {
    Serial.print("Resetting LastTime, old: ");
    Serial.print(LastTime);
    Serial.print(" new: ");
    LastTime = millis();
    Serial.println(LastTime);
  }
  delay(1000);
}
