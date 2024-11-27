#include <Wire.h>
#include <U8g2lib.h>

// Initialize U8g2 for 128x32 OLED using I2C
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define triacPulse 11     // Triac control for Load
#define SW1 7               // Button for Load
#define ZVC1 3              // Zero-crossing detection for Load

int x1 = 8000;                 // Delay value for Load
int counter1 = 0;           // Power level counter
bool lastButtonState1 = HIGH;
unsigned long lastDebounceTime1 = 0;
unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);

  // Initialize the OLED
  u8g2.begin();

  // Pin setup
  pinMode(ZVC1, INPUT_PULLUP);
  pinMode(triacPulse, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);

  // Attach interrupt for zero-crossing detection
  attachInterrupt(digitalPinToInterrupt(ZVC1), acon1, FALLING);
}

void loop() {
  handleButton(SW1, lastButtonState1, counter1, x1);

  // Update OLED display
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr); // Select a readable font
  u8g2.setCursor(0, 10);
  u8g2.print("Load Power Level: ");
  u8g2.print(counter1);
  u8g2.sendBuffer();
}

// Function to handle button presses with debounce and counter updates
void handleButton(int buttonPin, bool &lastState, int &counter, int &x) {
  int reading = digitalRead(buttonPin);

  if (reading != lastState) {
    unsigned long currentTime = millis();
    if ((currentTime - lastDebounceTime1) > debounceDelay) {
      if (reading == LOW) {
        counter = (counter + 1) % 10; // Cycle from 0 to 9
        x = map(counter, 0, 9, 8000, 0);  // Map counter to delay for power control
        Serial.print("Power Level Counter: ");
        Serial.println(counter);
      }
      lastDebounceTime1 = currentTime;
    }
  }
  lastState = reading;
}

// Interrupt Service Routine for Load
void acon1() {
  delayMicroseconds(x1);
  digitalWrite(triacPulse, HIGH);
  delayMicroseconds(50);
  digitalWrite(triacPulse, LOW);
}
hvgjvjj