
const byte ledPin = 13;             // LED digital output pin
const byte switchPin = 2;           // Switch digital input pin

byte state = LOW;                   // State of the LED

volatile boolean newPress = false;  // Flag to indicate a new switch press

unsigned long lastPress = 0;        // Counter to store the last press time
unsigned long debounceDelay = 200;  // Threshold between 2 button presses (to eliminate switch debouncing effect)

void setup() {
  
  // Open the serial port at 9600 bps
  Serial.begin(9600);

  // Declare pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);

  // Attach the pin to the ISR (CHANGE: Respond to any change of the button state)
  attachInterrupt(digitalPinToInterrupt(switchPin), button_press_ISR, CHANGE);
  
}

void loop() {

  // New press detected
  if (newPress) {
    
    // Print to the serial monitor
    Serial.println("Pressed");
    
    // Change LED state 
    state = !state;
    
    // Reset the press flag
    newPress = false;
  }

  // Output state on the LED
  digitalWrite(ledPin, state);
}

void button_press_ISR() {

  // Delay between two presses is bigger than the debounce threshold (i.e: not a change because of bouncing)
  if (millis() - lastPress >= debounceDelay) {

    // Set the press flag
    newPress = true;

    // Store last press timestamp
    lastPress = millis();
  }
}
