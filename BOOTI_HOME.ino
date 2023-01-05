#include <ESP8266WiFi.h> // https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <string.h>

ESP8266WebServer server(80);
HTTPClient http;
int salida = 0;
int entrada = 2;
WiFiClient client;
//IPAddress sta_ip = IPAddress(192,168,0,20);
//IPAddress sta_gw = IPAddress(192,168,0,1);
//IPAddress sta_sn = IPAddress(255,255,255,0);
void handle_not_found()
{
    Serial.println("404 Not Found ");
    server.send(404, "text/plain", "Not Found. You requested \"" + server.uri() + "\"");
    delay(100);
    Serial.println("Request handled.");
}


void handle_relay()
{
    if (server.uri() == "/api/salida")
    {
        if (server.method() == HTTP_POST)
        {
            for (uint8_t i = 0; i < server.args(); i++)
            {
                if (server.argName(i) == "state")
                {
                    digitalWrite(salida, server.arg(i).toInt());
                    server.send(200, "application/json", "{\"state\": " + String(server.arg(i).toInt()) + "}");
                }
            }
        }
        else
        {
            server.send(501, "text/json", "{\"error\": \"Not Implemented: " + String(server.method()) + "\"}");
        }
    }
    else if (server.uri() == "/api/entrada")
    {
        if (server.method() == HTTP_GET)
        {
            server.send(200, "application/json", "{\"state\": " + String(digitalRead(entrada)) + "}");
        }
        else
        {
            server.send(501, "text/json", "{\"error\": \"Not Implemented: " + String(server.method()) + "\"}");
        }
    }
    else
    {
        server.send(501, "text/json", "{\"error\": \"Not Implemented: " + String(server.method()) + "\"}");
    }

    delay(100);
}

void setup()
{

    // Set up the pins and apply the default state.
    pinMode(salida, OUTPUT);
    pinMode(entrada, INPUT_PULLUP );
    digitalWrite(salida, LOW);

    Serial.begin(115200);

    WiFiManager wifiManager;
    //wifiManager.resetSettings();
    wifiManager.setTimeout(10000);
    //wifiManager.setSTAStaticIPConfig(sta_ip, sta_gw, sta_sn);
    WiFi.hostname("Superbooti");
    if (!wifiManager.autoConnect("Estiwifi"))
    {
        Serial.println("Failed to connect and hit timeout");
        delay(3000);
        ESP.reset();
        delay(5000);
    }

    Serial.println("Connected to Wifi...");
    server.on("/api/salida", handle_relay);
    server.on("/api/entrada", handle_relay);
    server.onNotFound(handle_not_found);

    Serial.println("Server Started!");
}
void loop()
{
    server.handleClient();
    digitalWrite(salida, LOW);
    delay(1000);
    digitalWrite(salida, HIGH);
}
