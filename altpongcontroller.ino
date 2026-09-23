#include <WiFi.h>
#include <WiFiUdp.h>

const char* ap_ssid = "altpong";
const char* ap_password = "pumpitup";



WiFiUDP udp;
const int udpPort = 4210;
IPAddress clientIP(192, 168, 4, 2);
const int mic = 10; // mic on adc pin 10
const int sampleWin = 100; // change if a lound room

//sample code from Neurodesign project that creates a smoothing/sampling window for mic
float envelope = 0;
float prevEnvelope = 0;
const float attackRate = 0.6;
const float decayRate = 0.05;
const float noiseFloor = 2200;
const float maxAmp = 4000; 



void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  WiFi.softAP(ap_ssid, ap_password);
  //Serial.print("Access Point started. IP: ");
  Serial.println(WiFi.softAPIP()); // will almost always be 192.168.4.1
  //while (WiFi.status() != WL_CONNECTED) {
    //delay(300);
    //Serial.print("wifi loading");
  //}
  //Serial.println("connected wifi" + WiFi.localIP().toString());
  pinMode(mic, INPUT);
}

void loop() {
  unsigned long start = millis();
  int sMax = 0, sMin = 4095;
  while (millis() - start < sampleWin) {
    int s = analogRead(mic);
    if (s > sMax) sMax = s;
    if (s < sMin) sMin = s;
  }
  int amplitude = sMax - sMin;

  if (amplitude > envelope) envelope += (amplitude - envelope) * attackRate;
  else envelope += (amplitude - envelope) * decayRate;
  //Serial.println(amplitude);
  // this part only reads the increases in sound 
  if (envelope > prevEnvelope && envelope > noiseFloor) {
    float normalized = constrain(envelope / maxAmp, 0.0, 1.0);
    udp.beginPacket(IPAddress(192, 168, 4, 255), udpPort);
    udp.printf("%.4f", normalized);
    udp.endPacket();
  }

  prevEnvelope = envelope;
}
