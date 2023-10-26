#include <DHT.h>
#include <EspMQTTClient.h>
#include "mqtt_secrets.h"

#define DHTPIN D1 //pin where the dht11 is connected (D1)
#define DHTTYPE DHT11

unsigned long delayTime = 10000;

DHT dht(DHTPIN, DHTTYPE);
float h = 0.0;
float t = 0.0;

EspMQTTClient client (
  SECRET_WIFI_NAME,
  SECRET_WIFI_PASSWORD,
  MQTT_BROKER_SERVER_IP,
  SECRET_MQTT_USERNAME,
  SECRET_MQTT_PASSWORD, 
  SECRET_MQTT_CLIENT_ID
);


void onConnectionEstablished() {
      // Subscribe to "channels/""/subscribe" and display received message to Serial
      client.subscribe("channels/" + String(CHANNEL_ID) + "/subscribe", [](const String & payload) {
        Serial.println(payload);  
      });  

      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" degrees Celcius, Humidity: ");
      Serial.print(h);
      Serial.println("%");
      Serial.println("Send to Thingspeak.");

      String message = "field1=" + String(t) + 
                   "&field2=" + String(h);

      client.publish("channels/"+ String(CHANNEL_ID) +"/publish", message);

      // Excute delayed instructions
      client.executeDelayed(5 * 1000, [](){});


}

void setup() {
  Serial.begin(115200);
  dht.begin();
  client.enableDebuggingMessages();
  client.enableHTTPWebUpdater();
  client.enableOTA();
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");
}

void loop() {
  h = dht.readHumidity();
  t = dht.readTemperature();

  if (isnan(t) || isnan(h)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
  }

  client.loop();
  
  delay(delayTime);
}
