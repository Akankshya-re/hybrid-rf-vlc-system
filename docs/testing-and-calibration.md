# Testing and calibration

## A. RF-only test
1. Upload a known-good RF24 transmitter and receiver example.
2. Confirm Node B receives multiple consecutive packets.
3. Add a 10 µF capacitor across each NRF24L01 3.3 V/GND supply.

## B. VLC receiver test
1. Temporarily run a continuous-light test on Node B.
2. On Node A, print `analogRead(A0)` continuously.
3. Record at least 20 samples with the VLC LED off and 20 with it on.
4. Compare means and ranges, not just one sample.
5. If the ranges overlap substantially, do not tune code to force success; fix receiver gain, biasing, alignment, shielding, or sensor compatibility.

## C. Integrated test
1. Upload Node B first, then Node A.
2. Open both Serial Monitors at 9600 baud.
3. Confirm Node B prints a received sequence.
4. Confirm Node A detects two optical ON pulses with a return-to-baseline interval.
5. Repeat at multiple distances and ambient-light levels.
