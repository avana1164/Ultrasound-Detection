#include <WiFiS3.h>

char ssid[] = "";
char pass[] = "";

int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    ;
  }

  Serial.println("Starting Wi-Fi...");

  // Check that the Wi-Fi hardware is available
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Wi-Fi module not detected!");
    while (true);
  }

  // Connect to Wi-Fi
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);

    delay(5000);
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected!");
  }

  delay(5000);
}
