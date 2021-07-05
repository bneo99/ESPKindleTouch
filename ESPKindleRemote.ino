#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

//comment out if we dont have ota led (secondary led)
//if undefined we will use the main led to show when we are in ota mode
//#define OTA_LED

// set the IO state when led is on (active low for esp12f module)
#define LED_ACTIVE_STATE 0
#define OTA_LED_ACTIVE_STATE 0

// enable to show debug messages over serial
// this will disable output in the internal led as the led uses the tx pin
const bool debug = false;

// wifi credentials
const char* ssid     = "<your wifi network>";
const char* password = "<your wifi password>";
const char* hostname = "ESPKindleRemote";

// enter the ip address of your kindle below
// kindle IP should be static or setup DHCP reservation
byte ip[] = {192, 168, 0, 10}; // add kindle IP address here too
const char* host = "<your kindle IP adress>";

//2 button only
const int forwardPin = 3; // forward button pin
const int backwardPin = 1; // backward button pin

// uncomment for the remote pinouts
// 4 button
//const int forwardPin = 4; // forward button pin
//const int backwardPin = 14; // backward button pin
//const int forwardPin1 = 12; // forward button pin
//const int backwardPin1 = 13;

const int ledPin = 2; // indicator led

#ifdef OTA_LED
const int otaLed = BUILTIN_LED;
#else
const int otaLed = ledPin; //declare ota led as main led
#endif

const String nextPagePath = "/cgi-bin/next.cgi";
const String prevPagePath = "/cgi-bin/prev.cgi";

WiFiClient client;

void setup() {
  if (debug) {
    // init serial
    Serial.begin(115200);
    delay(10);
  }
  else {
    //we only can use led pin if serial is not used
    pinMode(ledPin, OUTPUT);
  }

  #ifdef OTA_LED
  pinMode(otaLed, OUTPUT);
  #endif
  
  // set pin modes
  pinMode(forwardPin, INPUT_PULLUP);     // forward button pin
  pinMode(backwardPin, INPUT_PULLUP);     // backward button pin
//  pinMode(forwardPin1, INPUT_PULLUP);
//  pinMode(backwardPin1, INPUT_PULLUP);

  // Connect to WiFi network
  if (debug) {
    Serial.println("Debug enabled");
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  //set hostname
  WiFi.hostname(hostname);
  WiFi.begin(ssid, password);

  // wait while we are connecting to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    if (debug) {
      Serial.print(".");
    }
    else {
      digitalWrite(ledPin, !digitalRead(ledPin));
    }
  }

  if (debug) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    //turn led off
    digitalWrite(ledPin, !LED_ACTIVE_STATE);
  }
  
  //check if ota (both buttons 3/4 pressed on power on)
//  if (digitalRead(forwardPin1) == LOW || digitalRead(backwardPin1) == LOW) {
  if (digitalRead(forwardPin) == LOW || digitalRead(backwardPin) == LOW) {
    if (debug) {
      Serial.println("OTA enabled");
    }
    // declare callbacks
    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_FS
        type = "filesystem";
      }

      // NOTE: if updating FS this would be the place to unmount FS using FS.end()
      if (debug) {
        Serial.println("Start updating " + type);
      }
    });
    ArduinoOTA.onEnd([]() {
      if (debug) {
        Serial.println("\nEnd");
      }
      // flash to let user know its complete
      for (int i = 0; i < 10; i++) {
        digitalWrite(otaLed, OTA_LED_ACTIVE_STATE);
        delay(100);
        digitalWrite(otaLed, !OTA_LED_ACTIVE_STATE);
        delay(100);
      }
      ESP.restart();
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      if (debug) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      }
      digitalWrite(otaLed, !digitalRead(otaLed));
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
      if (debug) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
          Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
          Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
          Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
          Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
          Serial.println("End Failed");
        }
      }
      //keep flashing till we get reset
      unsigned long prevMillis = 0;
      unsigned long currMillis;
      bool ledState = !LED_ACTIVE_STATE;

      while (1) {
        ArduinoOTA.handle();

        currMillis = millis();

        if (currMillis - prevMillis > 500) {
          ledState = !ledState;
          digitalWrite(otaLed, ledState);
          prevMillis = currMillis;
        }
        
        yield();
      }
    });

    //begin ota
    ArduinoOTA.begin();
    MDNS.update(); //just in case

    //lock ourselves in a loop while we wait for ota
    unsigned long prevMillis = 0;
    unsigned long currMillis;
    bool ledState = !LED_ACTIVE_STATE;

    while (1) {
      ArduinoOTA.handle();

      currMillis = millis();

      if (currMillis - prevMillis > 1000) {
        ledState = !ledState;
        digitalWrite(otaLed, ledState);
        prevMillis = currMillis;
      }
      yield();
    }
  }
  else {
    //turn ota led off
    digitalWrite(otaLed, !OTA_LED_ACTIVE_STATE);
  }

  delay(1000);
}

void loop() {
//
//  if (digitalRead(forwardPin) == LOW || digitalRead(forwardPin1) == LOW) {
//    PageForward();
//  }
//  if (digitalRead(backwardPin) == LOW || digitalRead(backwardPin1) == LOW) {
//    PageBackward();
//  }

  if (digitalRead(forwardPin) == LOW) {
    PageForward();
  }
  if (digitalRead(backwardPin) == LOW) {
    PageBackward();
  }

}

void PageForward() {
  if (debug) {
    Serial.println("Forward pressed...");
    Serial.printf("\n[Connecting to %s ... ", host);
  }
  else {
    digitalWrite(ledPin, LED_ACTIVE_STATE);
  }

  if (client.connect(ip, 80)) {
    if (debug) {
      Serial.println("connected]");
      Serial.println("[Sending a request]");
    }

    // send the GET request
    client.print(
      String("GET ") + nextPagePath +
      " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection: close\r\n" + "\r\n"
    );

    if (debug) {
      Serial.println("[Response:]");
      while (client.connected() || client.available()) {
        if (client.available())
        {
          String line = client.readStringUntil('\n');
          Serial.println(line);
        }
      }
    }

    client.stop();

    if (debug) {
      Serial.println("\n[Disconnected]");
    }
    else {
      digitalWrite(ledPin, !LED_ACTIVE_STATE);
    }
  }
  else
  {
    // connection failed
    if (debug) {
      Serial.println("connection failed!]");
    }
    else {
      digitalWrite(ledPin, !LED_ACTIVE_STATE);
    }
    client.stop();
  }
  delay(500);
}

void PageBackward() {
  if (debug) {
    Serial.println("Backward pressed...");
    Serial.printf("\n[Connecting to %s ... ", host);
  }
  else {
    digitalWrite(ledPin, LED_ACTIVE_STATE);
  }

  if (client.connect(ip, 80))
  {
    if (debug) {
      Serial.println("connected]");
      Serial.println("[Sending a request]");
    }

    client.print(
      String("GET ") + prevPagePath +
      " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection: close\r\n" + "\r\n"
    );

    if (debug) {
      Serial.println("[Response:]");

      while (client.connected() || client.available()) {
        if (client.available())
        {
          String line = client.readStringUntil('\n');
          Serial.println(line);
        }
      }
    }

    client.stop();

    if (debug) {
      Serial.println("\n[Disconnected]");
    }
    else {
      digitalWrite(ledPin, !LED_ACTIVE_STATE);
    }
  }
  else
  {
    if (debug) {
      Serial.println("connection failed!]");
    }
    else {
      digitalWrite(ledPin, !LED_ACTIVE_STATE);
    }
    client.stop();
  }
  delay(500);
}
