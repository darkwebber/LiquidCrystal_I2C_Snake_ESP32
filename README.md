# LiquidCrystal_I2C_Snake_ESP32

A simple Snake game on an ESP32 with an I2C LCD display. The game operates at a **pixel level** within each 16x2 LCD cell (5x8 pixels per cell), utilizing **4x2 cells** due to the 8 custom character limit.

## Features
- Play Snake on a 16x2 LCD—because why not?
- Directional control via hardware buttons.
- **4x2 cell** playable area (because LCD character limits are fun!).
- Random food placement and an ever-growing snake.
- Smooth and responsive controls using hardware interrupts.

## Hardware Requirements
- **ESP32 Development Board**
- **16x2 LCD Display**
- **I2C Module**
- **4 Push Buttons** (Up, Down, Left, Right)
- **Jumper Wires**
- **Breadboard** (optional)

## Software Requirements
- **Arduino IDE** (or PlatformIO)
- **ESP32 Board Support Package**
- **LiquidCrystal_I2C Library**
- **cppQueue Library**

## Wiring
| ESP32 Pin | Button Function |
|-----------|----------------|
| GPIO 13   | Move Right      |
| GPIO 27   | Move Left       |
| GPIO 12   | Move Up         |
| GPIO 14   | Move Down       |

**LCD Connections:**
- **VCC** → **3.3V**
- **GND** → **GND**
- **SDA** → **GPIO 21**
- **SCL** → **GPIO 22**

## Setup & Installation
1. Follow the [ESP32 Arduino Setup Guide](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).
2. Clone this repository:
   ```sh
   git clone https://github.com/darkwebber/LiquidCrystal_I2C_Snake_ESP32.git
   cd LiquidCrystal_I2C_Snake_ESP32
   ```
3. Open the project in **Arduino IDE**.
4. Install required libraries (**LiquidCrystal_I2C**, **cppQueue**).
5. Select the correct **board** and **COM port**, then upload the code.

## Gameplay
- No walls—space is already limited enough.
- The snake moves continuously in the set direction unless changed.
- Collecting food increases the snake's length.
- **Game Over**: The game ends if the snake collides with itself.
- **Restart**: Restart after a game-over/power-off while keeping the high score.
  - Press `R` then `U` after game-over to restart.
  - Then if you want to reset the high score too, press `R` else `L`. 
- **No direct reversal** of movement (e.g., Left to Right is ignored).

## Future Plans
- [ ] Add an option to pause in betwen the game.
- [x]  ~~Add self-collision detection.~~
- [x]  ~~Improve food placement so it doesn’t spawn inside the snake.~~
- [x]  ~~Implement a proper score display.~~
- [x]  ~~Add a restart option after game over.~~
- [x]  ~~Store and display high scores persistently on the device.~~

## Video Demo
Curious to see it in action before you build? Check out this demo: [Snake On 16x2 LCD with ESP32 Dev Kit](https://youtu.be/IDjFa7bcI9M?feature=shared)

## Contributing
Fork this repository and submit pull requests for enhancements or bug fixes!

## License
MIT License.

---
Enjoy playing Snake on your ESP32! 🐍