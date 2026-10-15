# Hybrid-RF-VLC-System
This is an ongoing research project on Hybrid RF-VLC Communication System

A two-node prototype using an NRF24L01 radio link for data transfer and a visible-light link for acknowledgement.

## 1. System architecture

- **Node A:** Arduino Uno, NRF24L01 RF transmitter, IR photodiode + LM358 receiver.
- **Node B:** Arduino Uno, NRF24L01 RF receiver, MOSFET-driven LED VLC transmitter.
- **Forward channel:** Node A → Node B over RF.
- **Acknowledgement channel:** Node B → Node A over VLC.
- The application alternates between RF packet transmission and VLC acknowledgement; Node B turns off RF listening while transmitting the optical ACK pattern.

> **Status note:** RF packet reception has been observed during testing. VLC acknowledgement detection is still under calibration. The reported ADC values (LED off ~450–490; LED on ~470, occasionally ~500) do not yet demonstrate a sufficiently stable optical signal. Do not describe the full integrated link as verified until repeated tests succeed.

## 2. Repository layout

```text
hybrid-rf-vlc/
├── README.md
├── LICENSE
├── .gitignore
├── Node_A/
│   └── Node_A.ino
├── Node_B/
│   └── Node_B.ino
└── docs/
    ├── circuit-connections.md
    ├── testing-and-calibration.md
    └── results.md
```

## 3. Required components

- 2 × Arduino Uno (or compatible 5 V boards)
- 2 × NRF24L01 modules
- 1 × LM358 op-amp
- 1 × IR photodiode
- 1 × LED module suitable for the selected supply
- 1 × IRFZ44N MOSFET or a suitable logic-level MOSFET/driver
- 220–330 Ω gate resistor, 10 kΩ gate pull-down
- Appropriate LED current limiting/driver, if not built into the LED module
- 2 × 10 µF capacitors for NRF24L01 supply decoupling (one per module)
- LM358 feedback components and a stable reference network
- Jumper wires, breadboards, multimeter, and regulated supplies

## 4. Wiring summary

### Both NRF24L01 modules

| NRF24L01 | Arduino Uno |
|---|---|
| VCC | 3.3 V only |
| GND | GND |
| CE | D9 |
| CSN | D10 |
| SCK | D13 |
| MOSI | D11 |
| MISO | D12 |
| IRQ | Not connected |

Place a 10 µF capacitor directly across each NRF module's 3.3 V and GND pins, observing electrolytic polarity. NRF24L01 modules must not be powered from 5 V.

### Node A receiver

| Component/pin | Connection |
|---|---|
| LM358 pin 8 | +5 V |
| LM358 pin 4 | GND |
| LM358 pin 1 | Arduino A0; feedback network output |
| LM358 pin 2 (−) | Photodiode cathode and feedback network |
| LM358 pin 3 (+) | Stable reference near 2.5 V |
| Feedback resistor | Pin 1 ↔ Pin 2 |
| Feedback capacitor | Parallel with feedback resistor, Pin 1 ↔ Pin 2 |
| Photodiode anode | GND, only if this matches the verified receiver topology |

The pin-2 photodiode connection above is the current reported wiring, not a guarantee that the analog front end is optimally biased. Verify the exact photodiode polarity and amplifier topology against its datasheet and a proper transimpedance-amplifier design before final testing. The 100 µF and 10 µF capacitors previously placed across LM358 pins 8 and 4 are supply decoupling; they do not replace NRF24L01 decoupling.

### Node B VLC transmitter

| Component/pin | Connection |
|---|---|
| Arduino D5 | 220–330 Ω resistor → MOSFET gate |
| MOSFET gate | 10 kΩ pull-down to GND |
| MOSFET source | GND |
| MOSFET drain | LED module negative, if using a low-side driver |
| LED module positive | Correct supply positive through its required driver/current limiter |
| Supply GND | Arduino GND |

**Important LED safety:** The 12 V, 2.4 W LED is rated at about 0.2 A at 12 V. Do not connect it directly to 12 V without confirming that it is a complete 12 V module with built-in current regulation. If it is a bare LED, use a suitable constant-current driver; a small-signal 5 mm LED resistor calculation does not apply. The IRFZ44N is not guaranteed to turn fully on from a 5 V Arduino gate drive; use a logic-level MOSFET or a proper gate driver if needed. Do not connect or disconnect LED wiring while powered.

## 5. Arduino library

Install **RF24 by TMRh20** through Arduino IDE Library Manager.

Both sketches use the same radio channel, data rate, address, packet structure, and disabled RF auto-ack setting.

## 6. Upload order

1. Wire Node A's NRF24L01 and Node B's NRF24L01; add the 10 µF decoupling capacitors.
2. Test each radio separately with a basic RF example.
3. Upload `Node_B/Node_B.ino` to Node B.
4. Upload `Node_A/Node_A.ino` to Node A.
5. Open the correct Serial Monitor for each board at **9600 baud**.
6. Confirm Node B prints `Received sequence=...`.
7. Test the LED and receiver separately with a continuous-light test.
8. Record stable LED-OFF and LED-ON A0 values before tuning the threshold.
9. Run the integrated code and record successful ACKs, timeouts, distance, and ambient-light conditions.

## 7. Calibration

Node A currently uses:
- `ACK_DELTA = 40`: required increase above ambient baseline for an ON pulse.
- `LOW_DELTA = 15`: OFF level must fall to baseline + 15 or below.
- `ACK_TIMEOUT_MS = 2500`: maximum wait for the acknowledgement.

These are starting values, not universal settings. Tune only after measurements show a repeatable difference between LED OFF and ON. If the receiver output does not change consistently, fix the analog front end or optical alignment first; lowering the threshold excessively can create false acknowledgements from ambient light.

## 8. Known limitations

- The receiver currently uses simple amplitude thresholding, not a complete VLC physical-layer decoder.
- Ambient light and slow LM358 output drift can cause false detections or timeouts.
- RF auto-ack is disabled; delivery confirmation is intended to be supplied by VLC.
- The current baseline sketches are for controlled bench testing, not a robust deployed network.
- Duplicate suppression, CRC/application-level integrity checks, and a more formal ACK frame are recommended improvements.

## 9. Safety and measurement notes

- Disconnect power before rewiring.
- Verify LED module ratings and current limiting before applying 12 V.
- Never feed 12 V into Arduino pins, LM358 inputs, or NRF24L01 power pins.
- Keep all grounds common where required by the MOSFET driver.
- Use a multimeter to verify supply rails and LM358 output before connecting A0.
- Record measurements honestly; do not report a successful VLC ACK unless it is repeatedly observed with Node B's LED as the source.

## 10. Suggested future improvements

- Add a proper photodiode transimpedance amplifier and stable bias/reference.
- Use a logic-level MOSFET or dedicated LED driver.
- Encode the ACK with a preamble and timing-based pulse decoder.
- Add packet CRC, sequence matching in the ACK, retry limits, and duplicate suppression.
- Test range, alignment, ambient-light rejection, packet success rate, and latency.
