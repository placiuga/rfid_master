#include "enterprise.h"

WS2812 pixels(10, 13);

void serialConnect() {
    //Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
}

void LEDsetup() {
    pixels.begin();
    pixels.clear();
    pixels.setBrightness(255);
    pixels.setPixelColor(0, 255, 255, 0);
    pixels.setPixelColor(1, 255, 255, 0);
    pixels.show();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void enterpriseConnect(const char* ssid, const char* user, const char* pass) {

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
    }

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA2 enterprise network:
        // - You can optionally provide additional identity and CA cert (string) parameters if your network requires them:
        //      WiFi.beginEnterprise(ssid, user, pass, identity, caCert)
        status = WiFi.beginEnterprise(ssid, user, pass);

        // wait 10 seconds for connection:
        delay(10000);
    }

    if(status == WL_CONNECTED) {
        Serial.println("Connected");
        digitalWrite(LED_BUILTIN, HIGH);
        pixels.setBrightness(255);
        pixels.setPixelColor(0, 0, 0, 255);
        pixels.show();
    }

    // you're connected now, so print out the data:
    Serial.print("You're connected to the network");
    printCurrentNet();
    printWifiData();

    Serial.println("Connecting to server...");
}

void serverConnect(const char* server)
{
    //Code to connect to "server" (my laptop for now)
    if (client.connect(server, 80)) {
        Serial.println("Connected to server:");
        client.print("GET / HTTP/1.1\r\n");
        client.print("Host: ");
        client.print(server);
        client.print("\r\n");
        client.print("Connection: close\r\n");
        client.print("\r\n");

    }
    else {
        Serial.println("Connection failed");
    }

}

void printWifiData() {
    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print your MAC address:
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC address: ");
    printMacAddress(mac);
}

void printCurrentNet() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print the MAC address of the router you're attached to:
    byte bssid[6];
    WiFi.BSSID(bssid);
    Serial.print("BSSID: ");
    printMacAddress(bssid);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);

    // print the encryption type:
    byte encryption = WiFi.encryptionType();
    Serial.print("Encryption Type:");
    Serial.println(encryption, HEX);
    Serial.println();
}

void printMacAddress(byte mac[]) {
    for (int i = 5; i >= 0; i--) {
        if (mac[i] < 16) {
            Serial.print("0");
        }
        Serial.print(mac[i], HEX);
        if (i > 0) {
            Serial.print(":");
        }
    }
    Serial.println();
}


//note: in sendData, server will be set to SECRET_IP (which will probably be pi). For testing, put your own IP in
//arduino_secrets.h, or replace server[] in enterprise.ino
void sendData(String server, String machineID, String rfid_uid, String action) {
    //stop previous just in case
    client.stop();

    //connect to localhost
    if (client.connect(server.c_str(), 80)) {
        //display sent info for debug 
        Serial.println("Sending: MachineID: " + machineID + ", RFID_UID: " + rfid_uid + ", Action: " + action);

        //generate url based on delivered info
        String url = "/rfid/index.php?machineID=" + machineID +
                    "&rfid_uid=" + rfid_uid +
                    "&action=" + action;

        //send info to php
        client.println("GET " + url + " HTTP/1.1");
        client.println("Host: " + server);
        client.println("Connection: close");
        client.println();

        //display what was sent for debug
        Serial.println("Request sent: " + url);
        unsigned long startTime = millis();
        while (client.connected() && !client.available()) {
            if (millis() - startTime > 3000) {
                Serial.println("Timeout waiting for response");
                client.stop();
                return;
            }
            delay(10);
        }

        //read the response from php script
        String response = "";
        while (client.available()) {
            char c = client.read();
            response += c;
        }
        client.stop();
        Serial.println(response);

        //look for a blank line to know when to end
        int bodyIndex = response.indexOf("\r\n\r\n");
        String body = (bodyIndex != -1) ? response.substring(bodyIndex + 4) : response;
        body.trim(); //remove white space + newlines

        //check for authorization response (or denial, or error)
        if (body.indexOf("AUTHORIZED") != -1) {
            Serial.println("Access granted");
            digitalWrite(LED_BUILTIN, HIGH);           
            if(action == "start") {
                pixels.setPixelColor(1, 0, 255, 0);         //green LED
                pixels.show();
            }
            else if(action == "end" ) {
                pixels.setPixelColor(1, 255, 255, 0);       //yellow LED
                pixels.show();
            }
        } 
        else if (body.indexOf("DENIED") != -1) {
            Serial.println("Access denied");
            pixels.setPixelColor(1, 255, 0, 0);             //red LED
            pixels.show();
            delay(1000);
            pixels.setPixelColor(1, 255, 255, 0);           
            pixels.show();
        } 
        else if (body.indexOf("ERROR") != -1) {
            pixels.setPixelColor(1, 255, 0, 0); 
            pixels.show();
            delay(1000);
            pixels.setPixelColor(1, 255, 255, 0);      
            pixels.show();
            
        } 
        else {
            Serial.println("Unknown response");
            pixels.setPixelColor(1, 255, 0, 0); 
            pixels.show();
            delay(1000);
            pixels.setPixelColor(1, 255, 255, 0); 
            pixels.show();
        }
    }
    else {
        Serial.println("Connection failed");
    }
}