/*
  Hybrid RF-VLC Link — Node A
  Arduino Uno + NRF24L01 + LM358/photodiode receiver

  Operation:
    1. Measure ambient receiver level.
    2. Send a sequence-numbered packet over RF.
    3. Wait for Node B's two-pulse VLC acknowledgement.
    4. Report ACK or timeout.

  IMPORTANT:
  Calibrate ACK_DELTA using real LED-OFF and LED-ON measurements.
  This sketch cannot compensate for a receiver output that does not
  change reliably with the transmitted light.
*/

#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte ADDRESS[6] = "00001";
const byte VLC_RX_PIN = A0;

const unsigned long ACK_TIMEOUT_MS = 2500;
const unsigned long ACK_START_GUARD_MS = 300;

// Tune after measuring stable LED-OFF and LED-ON ADC values.
const int ACK_DELTA = 40;       // LED ON must rise at least this far above baseline
const int LOW_DELTA = 15;       // LED OFF must return close to baseline
const byte BASELINE_SAMPLES = 40;
const unsigned int SAMPLE_GAP_MS = 3;
const unsigned long PULSE_MAX_GAP_MS = 800;

struct Packet {
  uint8_t magic;
  uint8_t nodeId;
  uint16_t sequence;
};

const uint8_t PACKET_MAGIC = 0xA7;
uint16_t sequenceNumber = 0;

int readAverage(byte pin, byte samples) {
  long sum = 0;
  for (byte i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(SAMPLE_GAP_MS);
  }
  return (int)(sum / samples);
}

void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println(F("\n=== NODE A: RF TX + VLC RX ==="));
  pinMode(VLC_RX_PIN, INPUT);

  if (!radio.begin()) {
    Serial.println(F("ERROR: NRF24L01 not detected. Check wiring/power."));
    while (true) {}
  }

  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setAutoAck(false);
  radio.disableCRC(); // same setting must be used on Node B
  radio.openWritingPipe(ADDRESS);
  radio.stopListening();

  Serial.println(F("NRF24L01 detected; ready."));
}

bool waitForVlcAck(int baseline) {
  const int highThreshold = baseline + ACK_DELTA;
  const int lowThreshold = baseline + LOW_DELTA;

  byte state = 0; // 0 wait high1, 1 wait low, 2 wait high2
  unsigned long start = millis();
  unsigned long lastTransition = start;

  Serial.print(F("Ambient baseline="));
  Serial.print(baseline);
  Serial.print(F(", HIGH threshold="));
  Serial.println(highThreshold);

  while (millis() - start < ACK_TIMEOUT_MS) {
    int value = analogRead(VLC_RX_PIN);

    if (state != 0 && millis() - lastTransition > PULSE_MAX_GAP_MS) {
      state = 0;
    }

    if (state == 0) {
      if (value >= highThreshold) {
        state = 1;
        lastTransition = millis();
        Serial.println(F("VLC pulse 1 ON"));
      }
    } else if (state == 1) {
      if (value <= lowThreshold) {
        state = 2;
        lastTransition = millis();
        Serial.println(F("VLC pulse 1 OFF"));
      }
    } else {
      if (value >= highThreshold) {
        Serial.println(F("VLC pulse 2 ON: ACK detected"));
        return true;
      }
    }

    delay(5);
  }
  return false;
}

void loop() {
  // Estimate ambient level before Node B starts its ACK pulses.
  int baseline = readAverage(VLC_RX_PIN, BASELINE_SAMPLES);

  Packet packet;
  packet.magic = PACKET_MAGIC;
  packet.nodeId = 1;
  packet.sequence = sequenceNumber++;

  Serial.println(F("\n--------------------------------"));
  Serial.print(F("Sending RF packet, sequence="));
  Serial.println(packet.sequence);

  radio.stopListening();
  radio.write(&packet, sizeof(packet)); // RF auto-ACK is intentionally disabled
  Serial.println(F("RF packet transmission attempted."));

  // Gives Node A time to enter VLC detection before Node B's light pattern.
  delay(ACK_START_GUARD_MS);
  Serial.println(F("Waiting for VLC ACK..."));

  if (waitForVlcAck(baseline)) {
    Serial.println(F("SUCCESS: VLC ACK received."));
  } else {
    Serial.println(F("FAILURE: VLC ACK timeout."));
  }

  delay(1000);
}
