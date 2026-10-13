# NTUA ECE Microcomputer Laboratory Course

This repository showcases the laboratory exercises, peripheral drivers, and embedded project implementations I completed during the Microcomputer Laboratory course at the National Technical University of Athens (NTUA). Through this coursework, I developed practical expertise in bare-metal embedded systems programming on the Microchip AVR ATmega328PB microcontroller using the custom `ntuAboard_G1` development platform. I gained hands-on experience developing low-level AVR Assembly and embedded C routines, handling hardware timers and external interrupts, driving analog sensors through ADC conversion, implementing serial bus communication protocols (TWI/I²C, 1-Wire, USART), and engineering an end-to-end IoT patient monitoring system with Wi-Fi telemetry.

---

## Laboratory Exercises

### Lab 1: AVR Assembly Fundamentals, Arithmetic & Logic Operations
*Introduction to low-level assembly instruction sets, precise timing loops, and state machine simulation.*

* **Key Tasks:**
  * **Ex 1 (Calibrated Delay Subroutine):** Implemented an accurate, parameterizable millisecond delay routine (`wait_x_msec`) utilizing register pair arithmetic (`sbiw` on `r24:r25`) and nested instruction cycles, verifying timing accuracy using the MPLAB X Stopwatch simulator tool.
  * **Ex 2 (Boolean Function Evaluation):** Programmed bitwise logic instructions to evaluate Boolean equations $F_0 = (A \cdot B' + B' \cdot D)'$ and $F_1 = (A + C') \cdot (B + D')$ over successive multi-variable increment cycles, monitoring output transitions via register watches.
  * **Ex 3 (Bidirectional Trolley Automation):** Modeled an automated cart traversing a rail by shifting an active bit on PORTD across LSB and MSB boundaries with 1-second transit delays, track boundary dwell intervals (2 seconds), and directional tracking via the `T` flag in `SREG`.
* **Tech Stack:** AVR Assembly, ATmega328PB, Microchip MPLAB X IDE.

### Lab 2: External Interrupts & Contact Debouncing
*Asynchronous event handling, push-button contact bounce filtering, and ISR-driven state actuation.*

* **Key Tasks:**
  * **Ex 1 (Edge-Triggered Counting & Debounce):** Configured external interrupt `INT1` on rising edges (`EICRA`, `EIMSK`) and developed an Assembly ISR with contact bounce elimination logic to drive a 6-bit modulo-64 binary counter on PORTC with freeze capability via input pin monitoring.
  * **Ex 2 (Context-Preserved Dynamic Counting):** Implemented an `INT0` ISR with full `SREG` and working register stack protection, dynamically calculating set bits from PINB and updating active output LED arrays.
  * **Ex 3 (Retriggerable Timed Actuation in ASM & C):** Designed dual Assembly and C implementations of an interrupt-driven actuator on `INT1` that illuminates an indicator LED for 5 seconds and handles re-trigger events within the active window by extending illumination and triggering secondary strobe alert patterns.
* **Tech Stack:** AVR Assembly, Embedded C, External Interrupts (`INT0`, `INT1`), Debouncing Logic.

### Lab 3: Hardware Timers, Fast PWM & ADC Sampling
*16-bit Timer/Counter1 configurations, pulse-width modulation, and analog voltage acquisition.*

* **Key Tasks:**
  * **Ex 1 (Assembly Fast PWM Generation):** Configured Timer1 in 8-bit Fast PWM mode (non-inverted output on `OC1A`, prescaler clock division to 62.5 kHz) with duty cycles dynamically selectable across an array table via external push-buttons.
  * **Ex 2 (PWM Control & Polled ADC Averaging in C):** Integrated Fast PWM motor speed adjustment alongside ADC conversion on channel `ADC1` ($V_{\text{ref}} = 5\text{V}$, $125\text{ kHz}$ ADC clock), implementing 16-sample rolling average smoothing via polling to drive 5-level LED status bars.
  * **Ex 3 (Dual-Mode PWM & Potentiometer Tracking):** Implemented runtime mode switching between manual digital button stepping (Mode 1) and continuous potentiometer tracking via left-adjusted ADC reading on channel `ADC0` (Mode 2).
* **Tech Stack:** Embedded C, AVR Assembly, Timer1 Fast PWM, Analog-to-Digital Converter (ADC).

### Lab 4: Alphanumeric 2×16 LCD Driver & Gas Detection Sensor
*HD44780-compatible LCD protocol, 4-bit nibble transmission, and toxic gas monitoring.*

* **Key Tasks:**
  * **Ex 1 (Assembly LCD Driver & Digital Voltmeter):** Authored low-level 4-bit nibble communication subroutines (`write_2_nibbles`, `lcd_command`, `lcd_data`) to drive an HD44780 LCD, sampled analog voltages from potentiometer `POT2`, performed binary-to-decimal string conversion via repeated subtraction, and displayed live voltages.
  * **Ex 2 (C LCD Voltmeter via Polling):** Ported LCD initialization, nibble packing, and instruction timing routines to C, sampling `ADC1` via polling to refresh a live digital voltmeter display.
  * **Ex 3 (Carbon Monoxide Sensor Emulation):** Interfaced channel `ADC2` to emulate an electrochemical CO sensor (ULPSM-CO-968-001), mapping PPM concentrations to sensor millivolts and triggering visual LED alarms and `"GAS DETECTED"` warnings when thresholds exceeded 70 PPM.
* **Tech Stack:** Embedded C, AVR Assembly, 2×16 Character LCD (HD44780), ADC, Sensor Interfacing.

### Lab 5: TWI / I²C Bus Architecture & PCA9555 Port Expander
*Master TWI bus communication, peripheral register mapping, and I/O expansion.*

* **Key Tasks:**
  * **Ex 1 (Bus Addressing & Logic Evaluation):** Configured hardware TWI0 to communicate with an NXP PCA9555 GPIO expander at slave address `0x40`, evaluated multi-variable logic functions from input PORTB, and wrote output states directly over I²C to expander port `IO0`.
  * **Ex 2 (Expander Keypad Interfacing):** Configured PCA9555 register maps (`REG_CONFIGURATION_1`, `REG_OUTPUT_1`, `REG_INPUT_1`) to isolate and decode active-low keypresses from the bottom keypad row, routing outputs to LED indicators.
  * **Ex 3 (I²C-Driven Alphanumeric LCD):** Re-engineered the 4-bit LCD driver to operate entirely through PCA9555 expander port `IO0`, incorporating DDRAM cursor positioning routines to alternate student names across lines 1 and 2.
* **Tech Stack:** Embedded C, TWI / I²C Protocol, PCA9555 I/O Expander, Character LCD.

### Lab 6: 4×4 Matrix Keypad Scanning & Passcode Authentication
*Matrix row-column scanning, rising-edge debounce filtering, and access control verification.*

* **Key Tasks:**
  * **Ex 1 (Keypad Driver Implementation):** Implemented row-by-row scanning (`scan_row`, `scan_keypad`) over the PCA9555 expander, created a debounce detection filter (`scan_keypad_rising_edge`) using the majority Boolean function $(A \cdot B) + (B \cdot C) + (A \cdot C)$ across 20 ms sampling intervals, and translated keys to ASCII codes.
  * **Ex 2 (Interactive Keypad-to-LCD Display):** Integrated keypad scanning with the character LCD to continuously display pressed keys.
  * **Ex 3 (Passcode Security System):** Engineered a multi-digit passcode authentication system (group key: `"42"`), handling press and release states to trigger a 3-second authorization LED pulse on success or a 5-second flashing alert on invalid combinations.
* **Tech Stack:** Embedded C, 4×4 Matrix Keypad, PCA9555 Expander, Debouncing Filters.

### Lab 7: Maxim 1-Wire Protocol & DS18B20 Temperature Sensor
*Microsecond bus timing, bit-banging communication, and signed fixed-point temperature conversion.*

* **Key Tasks:**
  * **Ex 1 (1-Wire Protocol Timing & Scratchpad Acquisition):** Implemented exact bit-banged 1-Wire master routines on pin `PD4` (`one_wire_reset`, `one_wire_transmit_bit`, `one_wire_receive_bit`), transmitted `Skip ROM` (`0xCC`) and `Convert T` (`0x44`) commands, polled completion, and fetched the 16-bit scratchpad output (`0xBE`).
  * **Ex 2 (Sensor Formatting & Display):** Handled two's complement sign conversion for negative temperatures, extracted integer and fractional components, eliminated trailing decimal zeros, and rendered real-time calibrated readings (`"±XX.XXX °C"`) on the I²C-driven LCD.
* **Tech Stack:** Embedded C, 1-Wire Protocol, DS18B20 Digital Thermometer, Bit-Banging, LCD.

### Lab 8: IoT Telemetry System with ESP8266 Wi-Fi & Multi-Sensor Integration
*Complete embedded IoT node integrating USART communication, analog sensing, 1-Wire, and HTTP telemetry.*

* **Key Tasks:**
  * **Ex 1 (USART Initialization & ESP8266 AT Control):** Configured hardware USART0 for 9600 baud communication (`8-N-1`), transmitted network control commands (`ESP:connect`, server endpoints), and parsed status responses to display connectivity feedback on the LCD.
  * **Ex 2 (Multi-Sensor Telemetry & Emergency Status Logic):** Acquired patient body temperature via the DS18B20 (1-Wire) and physiological pressure via `POT0` (ADC calibrated to $0$–$20\text{ cm H}_2\text{O}$), scanned the matrix keypad to handle emergency `"NURSE CALL"` alerts, and monitored abnormal vitals (`CHECK PRESSURE`, `CHECK TEMP`).
  * **Ex 3 (JSON Payload Formatting & Cloud Transmission):** Assembled structured JSON telemetry packets containing temperature, pressure, and operational status, transmitted data payloads via the ESP8266 over Wi-Fi to a remote HTTP server (`http://192.168.1.250:5000/data`), and confirmed execution via incoming HTTP `200 OK` responses.
* **Tech Stack:** Embedded C, ATmega328PB, ESP8266 Wi-Fi Module, USART, 1-Wire, TWI/I²C, HTTP/JSON Telemetry.

---

## Technologies Used

* **Languages:** AVR Assembly, Embedded C (AVR-GCC)
* **Target Hardware:** Microchip AVR ATmega328PB, `ntuAboard_G1` Development Platform
* **Peripherals & ICs:** NXP PCA9555 I/O Expander, Maxim DS18B20 Digital Thermometer, Espressif ESP8266 Wi-Fi Module, HD44780 2×16 Character LCD, 4×4 Matrix Keypad, Potentiometers
* **Buses & Protocols:** TWI / I²C, 1-Wire, USART / UART, Fast PWM, ADC
* **Tools & Toolchains:** Microchip MPLAB X IDE, Microchip Studio (Atmel Studio), AVR Libc
