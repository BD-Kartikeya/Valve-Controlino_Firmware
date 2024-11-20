# Fullstack Arduino Project

## Description

The "Fullstack" Arduino project is designed for a full-stack Regenerate printer, which will serve as the standard printer starting with the Minho printer (22.11.2024). It supports the following specifications:

### Printer Specifications

- **Printerheads:**
  - A: DOD/Pneumatic
  - B: DOD/Pneumatic
  - C: DOD/UV
- **Pneumatic Valves:** A, B
- **ZHub:**
  - A, B, C

### Pin Configuration

The components are connected to the Controlino Mini as follows:

| Component            | Pin  | Pin in code |   
|----------------------|------|-------------|  
| DOD Printerhead A    | D0   |      4      |  
| DOD Printerhead B    | D1   |      5      |  
| DOD Printerhead C    | D2   |      6      |  
| ZHub A               | D3   |      7      |   
| ZHub B               | D4   |      8      |    
| ZHub C               | D5   |      9      |  
| Pneumatic Valve A    | D6   |     A4      |  
| Pneumatic Valve B    | D7   |     A5      |  

**Note:** Refer to the Controlino Mini pinout documentation for the exact pin locations.

---

## Installation

1. Install the Arduino IDE from [arduino.cc](https://www.arduino.cc/).  
2. Open the provided `.ino` file in the Arduino IDE.  
3. Connect your Arduino board (Controlino Mini) to the computer via USB.  
4. Verify and upload the sketch to the board.  

---

This format organizes the information more effectively and makes the pin configuration easier to read.