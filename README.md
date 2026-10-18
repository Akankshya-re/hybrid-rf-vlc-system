# Hybrid-RF-VLC-System
This is an ongoing research project on a Hybrid RF-VLC Communication System

A two-node Arduino prototype that combines **radio-frequency (RF) communication** with a **visible-light communication (VLC) acknowledgement link**. Node A sends a data packet over an NRF24L01 radio link; Node B receives it and responds by modulating an LED, and Node A attempts to detect that optical acknowledgement using a photodiode receiver. A bidirectional hybrid communication link using two Arduino Uno nodes:
- Node A: NRF24L01 RF transmitter + IR photodiode/LM358 VLC receiver.
- Node B: NRF24L01 RF receiver + LED/MOSFET VLC transmitter.

> **Development status:** RF packet reception has been observed during bench testing. The VLC acknowledgement receiver is still being calibrated, so the complete RF-to-VLC acknowledgement loop should be considered a work in progress until repeated end-to-end tests succeed.

## Overview

Conventional wireless systems often use the same radio channel for both data and acknowledgement. This project explores a hybrid arrangement in which the forward data packet is transmitted over RF and the acknowledgement is returned through light. The prototype uses two Arduino Uno boards, NRF24L01 radio modules, and a photodiode-based optical receiver.

The current sketches provide a starting point for testing the communication sequence and receiver threshold. They are intended for controlled bench experiments, not as a finished or deployment-ready communication system.

## Problem Statement

RF links are useful for sending data without line-of-sight, while optical communication can provide a separate return channel in suitable lighting and alignment conditions. A hybrid system introduces practical challenges such as synchronising the two channels, detecting a relatively small optical signal, rejecting ambient light, and ensuring that an acknowledgement corresponds to the packet that was sent.

This project investigates a basic two-node design: send a packet from Node A to Node B over RF, then return an acknowledgement from Node B to Node A using an LED and photodiode receiver.

## Key Features

- **Hybrid communication path:** Uses RF for the forward data packet and VLC for the return acknowledgement.
- **Two Arduino nodes:** Separates the transmitter/receiver roles across Node A and Node B.
- **NRF24L01 link:** Uses the RF24 library by TMRh20 with matching radio settings on both nodes.
- **LED-based optical transmitter:** Node B emits a timed light-pulse pattern when a valid RF packet is received.
- **Photodiode receiver:** Node A samples an LM358-based analogue front end and checks for a light-pulse pattern.
- **Serial diagnostics:** Reports packet reception, sequence values, and acknowledgement outcomes for bench testing.
- **Calibration guidance:** Documents receiver threshold settings, wiring checks, and test steps.
- **Modular Arduino sketches:** Keeps Node A and Node B firmware in separate folders.

## Prototype Preview
<img width="1536" height="527" alt="44cd1a71-4062-4a1e-b220-971d502cef52" src="https://github.com/user-attachments/assets/0c651d51-9b2f-49bf-bb07-3fb15c749ff7" />

<!-- Add a real photograph or diagram of the assembled hardware here when available. -->
<!-- Suggested path: docs/images/hybrid-rf-vlc-prototype.png -->

*No verified hardware photograph is included in this repository yet.*

## Technology Stack

| Component | Technology |
|---|---|
| Microcontrollers | 2 × Arduino Uno or compatible boards |
| RF transceiver | NRF24L01 modules |
| RF software library | RF24 by TMRh20 |
| Optical transmitter | LED module driven through a MOSFET/appropriate LED driver |
| Optical receiver | Photodiode and LM358 analogue front end |
| Firmware language | Arduino C++ |
| Interface for testing | Arduino IDE Serial Monitor |
| Version control | Git and GitHub |

## System Architecture

The RF link carries the application packet from Node A to Node B. After receiving a valid packet, Node B temporarily stops RF listening and emits an optical acknowledgement pulse pattern. Node A monitors the photodiode receiver output for that pattern.

<img width="1536" height="1024" alt="f5a45e6d-0769-491c-8ce7-790b9d1203e0" src="https://github.com/user-attachments/assets/d5d22486-4ee2-4d7d-a42b-66bb85b87af6" />


*This is the intended logical flow. Actual end-to-end VLC acknowledgement detection is still under calibration.*

## Application Workflow

### 1. Initialise both nodes

Upload the Node A and Node B sketches to their respective Arduino boards. Both radio modules must use matching RF channel, data rate, address, and packet structure settings.

### 2. Send a packet over RF

Node A sends a packet containing a marker, node identifier, and sequence number through its NRF24L01 module.

### 3. Receive and validate the packet

Node B listens for RF packets and checks the packet marker and sender identifier before initiating the optical acknowledgement.

### 4. Transmit the VLC acknowledgement

Node B pauses RF listening and drives the LED with a timed two-pulse pattern. Use an appropriate MOSFET and LED current driver for the specific LED module.

### 5. Detect the optical response

Node A samples the analogue receiver output and compares it with an ambient baseline. It looks for the expected pulse pattern before declaring an acknowledgement. The threshold and receiver circuit require calibration under the actual test conditions.

### 6. Review serial output and measurements

Use the Serial Monitor on both boards to inspect RF reception, sequence values, acknowledgement detections, and timeouts. Record LED-off and LED-on readings before changing the detection thresholds.

## Project Structure

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

### Key Components

| File / Directory | Responsibility |
|---|---|
| `Node_A/Node_A.ino` | Sends RF packets and monitors the photodiode receiver for VLC acknowledgements |
| `Node_B/Node_B.ino` | Receives RF packets and transmits an LED pulse pattern as an optical acknowledgement |
| `docs/circuit-connections.md` | Summarizes the main wiring connections and hardware cautions |
| `docs/testing-and-calibration.md` | Provides bench-test and receiver-calibration guidance |
| `docs/results.md` | Records current observations and results that still need verification |
| `LICENSE` | Repository license text |
| `.gitignore` | Excludes common generated and local files from version control |

## Getting Started

### Prerequisites

- 2 × Arduino Uno or compatible boards
- 2 × NRF24L01 radio modules
- 1 × LM358 op-amp and 1 × photodiode for Node A
- 1 × LED module, suitable MOSFET or gate driver, and appropriate LED current driver for Node B
- Jumper wires, a multimeter, and suitable regulated power supplies
- Arduino IDE
- **RF24 library by TMRh20**, installed through the Arduino IDE Library Manager

### 1. Clone the Repository

Replace the placeholder URL with your GitHub repository URL.

```bash
git clone <YOUR_REPOSITORY_URL>
cd hybrid-rf-vlc
```

### 2. Wire the NRF24L01 Modules

Use the following SPI connections on both Arduino Uno boards:

| NRF24L01 pin | Arduino Uno pin |
|---|---|
| VCC | 3.3 V only |
| GND | GND |
| CE | D9 |
| CSN | D10 |
| SCK | D13 |
| MOSI | D11 |
| MISO | D12 |
| IRQ | Not connected |

Place a 10 µF capacitor directly across each NRF24L01 module's 3.3 V and GND pins, observing polarity. **Never power an NRF24L01 module from 5 V.**

### 3. Connect Node A's Optical Receiver

The current prototype uses a photodiode and LM358 amplifier, with the amplifier output connected to Arduino A0. The intended circuit includes a reference near 2.5 V and a feedback resistor/capacitor network.

Refer to [`docs/circuit-connections.md`](docs/circuit-connections.md). Verify photodiode polarity, amplifier topology, and the actual LM358 pin connections before powering the circuit. A connection description alone does not guarantee that the analogue front end is correctly biased or stable.

### 4. Connect Node B's LED Driver

The sketch uses Arduino D5 as the LED control output. Use a gate resistor and a gate-to-ground pull-down, and ensure the LED is connected through a suitable current-limiting arrangement or a driver designed for the exact module.

**Important:** A 12 V, 2.4 W load draws approximately 0.2 A at its rated operating point. Do not connect a bare LED directly to 12 V. Confirm whether the part is a complete 12 V module with built-in current regulation; if not, use a suitable constant-current driver. The IRFZ44N is not guaranteed to turn fully on from a 5 V Arduino output, so use a logic-level MOSFET or an appropriate gate driver if required. Keep the supply grounds common for a low-side MOSFET arrangement.

### 5. Install the RF24 Library

In Arduino IDE, open **Library Manager**, search for **RF24 by TMRh20**, and install it.

### 6. Upload the Sketches

1. Connect Node B to the computer and open `Node_B/Node_B.ino`.
2. Select the correct board and port, then upload the sketch.
3. Connect Node A and upload `Node_A/Node_A.ino`.
4. Open a Serial Monitor for each board at **9600 baud**.
5. Confirm that Node B reports received packet sequence numbers before attempting to evaluate VLC acknowledgements.

### 7. Test and Calibrate

Follow [`docs/testing-and-calibration.md`](docs/testing-and-calibration.md). First test RF reception on its own. Then measure the receiver output with the LED off and on, align the optical path, and confirm that the signal difference is stable before adjusting software thresholds.

## Results and Performance

Current bench observations indicate that the NRF24L01 modules were detected and that Node B received RF packets. The optical receiver has not yet shown a sufficiently stable, repeatable difference between LED-off and LED-on states to confirm reliable end-to-end VLC acknowledgement.

Previously observed analogue readings overlapped substantially (roughly 450–490 ADC counts with the LED off and around 470, occasionally rising near 500, with the LED on). These measurements are preliminary and depend on the setup, ambient lighting, and alignment.

| Metric | Current status |
|---|---|
| NRF24L01 module initialization | Observed during bench testing |
| RF packet reception at Node B | Observed during bench testing |
| Stable LED-off / LED-on separation | Not yet established |
| Reliable VLC acknowledgement detection | Not yet verified |
| End-to-end hybrid link reliability | Not yet verified |

Do not report a successful integrated link, range, packet success rate, or latency until it has been measured repeatedly and documented. Add new measurements to `docs/results.md` with test conditions and dates.

## Current Limitations and Future Improvements

- **Optical receiver calibration:** The LED-on and LED-off readings currently overlap; the analogue front end and optical alignment need further investigation.
- **Ambient-light sensitivity:** Simple amplitude thresholds can be affected by room lighting, sunlight, and slow output drift.
- **Basic acknowledgement pattern:** The pulse sequence is a bench-test pattern, not a robust framed VLC protocol.
- **RF auto-ack disabled:** The sketches disable NRF24L01 hardware auto-ack because the intended acknowledgement path is optical.
- **Packet/ACK association:** A future protocol should include the received sequence number in the optical acknowledgement so Node A can verify which packet was acknowledged.
- **Hardware validation:** Confirm the photodiode amplifier topology, stable reference, LED current driver, and MOSFET gate-drive requirements.
- **Reliability testing:** Measure successful acknowledgements, timeouts, range, alignment tolerance, ambient-light performance, and latency over repeated trials.

## Safety and Measurement Notes

- Disconnect power before rewiring.
- Never apply 12 V to Arduino I/O pins, LM358 inputs, or NRF24L01 power pins.
- Power NRF24L01 modules from 3.3 V only and add local supply decoupling.
- Confirm the LED's current-control requirements before connecting a 12 V supply.
- Verify common ground and MOSFET pinout against the exact device datasheet.
- Measure the LM358 output and supply rails before connecting or relying on the Arduino analogue input.
- Record unsuccessful tests and uncertain measurements honestly; do not claim reliable VLC acknowledgement until it has been demonstrated repeatedly.

## Contributions


This project is developed collaboratively as part of a research hardware development project. The team is working on the design, implementation, integration, and testing of the Hybrid RF-VLC System.

Key areas of contribution included hybrid RF-VLC system design, Arduino firmware development, NRF24L01-based RF communication, LED-based optical acknowledgement transmission, photodiode receiver calibration, hardware integration, testing, troubleshooting, and project documentation.

This repository presents the collaborative project, with individual contributions acknowledged according to the work performed by each team member.
