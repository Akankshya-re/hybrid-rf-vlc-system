/*
  Hybrid RF-VLC Link — Node B
  Arduino Uno + NRF24L01 + MOSFET-driven LED transmitter

  Operation:
    1. Listen for an RF packet.
    2. Validate packet format.
    3. Pause RF listening and send a two-pulse VLC acknowledgement.
    4. Resume RF listening.

  LED_PIN drives the MOSFET gate through a 220–330 ohm resistor.
*/

#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte ADDRESS[6] = "00001";
const byte LED_PIN = 5;

struct Packet {
  uint8_t magic;
  uint8_t nodeId;
  uint16_t sequence;
};

const uint8_t PACKET_MAGIC = 0xA7;
Packet rxPacket;

void sendVlcAck() {
  Serial.println(F("Preparing VLC ACK pattern..."));

  // Let Node A finish its RF transmission and enter VLC detection first.
  delay(600);
  Serial.println(F("Sending VLC ACK pattern..."));

  // Two ON pulses separated by OFF intervals.
  digitalWrite(LED_PIN, HIGH);
  delay(250);
  digitalWrite(LED_PIN, LOW);
  delay(250);
  digitalWrite(LED_PIN, HIGH);
  delay(250);
  digitalWrite(LED_PIN, LOW);

  Serial.println(F("VLC ACK pattern complete."));
}

void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println(F("\n=== NODE B: RF RX + VLC TX ==="));
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  if (!radio.begin()) {
    Serial.println(F("ERROR: NRF24L01 not detected. Check wiring/power."));
    while (true) {}
  }

  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setAutoAck(false);
  radio.disableCRC(); // same setting must be used on Node A
  radio.openReadingPipe(1, ADDRESS);
  radio.startListening();

  Serial.println(F("NRF24L01 detected; receiver ready."));
}

void loop() {
  if (!radio.available()) {
    return;
  }

  radio.read(&rxPacket, sizeof(rxPacket));

  if (rxPacket.magic != PACKET_MAGIC || rxPacket.nodeId != 1) {
    Serial.println(F("Ignored invalid packet."));
    return;
  }

  Serial.println(F("\n--------------------------------"));
  Serial.print(F("Received sequence="));
  Serial.println(rxPacket.sequence);

  // Do not listen during the VLC acknowledgement pulse pattern.
  radio.stopListening();
  sendVlcAck();
  radio.startListening();
}
