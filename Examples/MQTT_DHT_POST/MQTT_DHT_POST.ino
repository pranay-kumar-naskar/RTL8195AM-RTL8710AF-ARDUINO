#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <avr/dtostrf.h>


#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11

#define BOARD_ID "ameba_4000"



// Update these with values suitable for your network.

char ssid[] = "IOT_MERCURY_59E0";     // your network SSID (name)
char pass[] = "1234567890";  // your network password
int status  = WL_IDLE_STATUS;    // the Wifi radio's status

char mqttServer[] = "iot.eclipse.org";

char clientId[]       = BOARD_ID;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

DHT dht(DHTPIN, DHTTYPE);
void setup()
{
  Serial.begin(38400);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  dht.begin();
  client.setServer(mqttServer, 1883);

  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  
  // Wait a few seconds between measurements.
  delay(10000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();


  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  static char humidity[10];
  static char temperature[10];
  dtostrf(h, 5, 3, humidity);
  dtostrf(t, 5, 3, temperature);
  static char buffer[128];
  snprintf(buffer, 20, "%s,%s", humidity, temperature);
  Serial.println(buffer);
  client.publish(BOARD_ID, buffer);
  delay(5000);
  client.loop();
}


