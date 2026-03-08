#include "enterprise.h"

void serialConnect() {
    //Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
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
void sendData(String server, String machineID, String studentID, String action) {
    //stop previous just in case
    client.stop();

    //connect to localhost
    if (client.connect(server.c_str(), 80)) {
        //display sent info for debug 
        Serial.println("Sending: MachineID: " + machineID + ", Student ID: " + studentID + ", Action: " + action);

        //generate url based on delivered info
        String url = "/sqltest/index.php?machineID=" + machineID +
                    "&studentID=" + studentID +
                    "&action=" + action;

        //send info to php
        client.println("GET " + url + " HTTP/1.1");
        client.println("Host: " + server);
        client.println("Connection: close");
        client.println();

        //display what was sent for debug
        Serial.println("Request sent:");
        Serial.println(url);
        client.stop();
    }
    else {
        Serial.println("Connection failed");
    }
}