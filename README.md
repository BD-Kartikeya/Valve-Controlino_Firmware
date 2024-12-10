# Fullstack Arduino Project
# Uploaded 22.11.24 on minho printer and everything works, Zhubs connected to DC-DC converter instead of Controlino Relay

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

# Valve Controlino

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

**Note:** Refer to the Controlino Mini pinout documentation for the exact pin locations and make sure you have the Controlino library installed in Arduino IDE and choose controlino mini @
https://www.controllino.com/board-library-setup-in-arduino-ide/?srsltid=AfmBOoos7H7Gvr0hwL9I-0P8VIP5qRC5WvGeUovFKAbZsvlEV49jvcsT

---

# Temperature Controlino

Controls/monitors the temperature readings of the printerheads using ds1820 sensors using onewire library(make sure to unstall it in Arduino IDE)
Also sends signals to relays for Peltier heating/cooling 

Pins for Temperature reading changed
1. A: 12 (PB4)(MISO)
2. B: 11 (PB3)(MOSI)
3. C: 10 (/SS)
check pinout pdf




