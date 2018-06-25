
const byte ledPin = 13;             // LED digital output pin
const byte switchPin = 2;           // Switch digital input pin
const byte tempSensorPin = 0;       /* The analog pin of LM35 with resolution = 10 mV / degree centigrade 
                                       with a 500 mV offset to allow for negative temperatures */
                                       
byte state = LOW;                   // State of the LED

volatile boolean newPress = false;  // Flag to indicate a new switch press
volatile boolean readTemp = false;  // Flag to enable reading temperature from sensor

unsigned long lastPress = 0;        // Counter to store the last press time
unsigned long debounceDelay = 200;  // Threshold between 2 button presses (to eliminate switch bouncing effect)


void setup() {
  
  // Open the serial port at 9600 bps
  Serial.begin(9600);

  // Declare pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);

  // Attach the pin to the ISR (CHANGE: Respond to any change of the button state)
  attachInterrupt(digitalPinToInterrupt(switchPin), button_press_ISR, CHANGE);

  init_timer1();

}


void loop() {

  // 
  if (readTemp) {

    // Reset temp flag
    readTemp = false;
    
    // Getting the voltage reading from the temperature sensor
    int reading = analogRead(tempSensorPin);  
    
    // Converting that reading to voltage
    float mv = (reading * 5.0) / 1024.0;
    
    // Converting to degrees from 10 mv per degree with 500 mV offset 
    float temperatureC = (mv - 0.5) * 100;
    
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" degrees C");
    
  }

  // New switch press detected
  if (newPress) {
    
    // Change LED state 
    state = !state;
    
    // Reset the press flag
    newPress = false;

    // Print to the serial monitor
    Serial.println("Pressed");
    
  }

  // Output state on the LED
  digitalWrite(ledPin, state);
}


void init_timer1()
{
  // Disable all interrupts
  noInterrupts();

  // Reset CCR registers
  TCCR1A = 0;               
  TCCR1B = 0;

  // Initialize counter with 0
  TCNT1  = 0;     
     
  // Compare match register 16MHz/256/(1/3Hz)
  OCR1A = 46875;

  // CTC mode
  TCCR1B |= (1 << WGM12); 

  // 1024 prescaler 
  TCCR1B |= (1 << CS12) | (1 << CS10);  

  // Enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);  

  // Enable all interrupts
  interrupts();
}


// Timer1 compare ISR
ISR(TIMER1_COMPA_vect) 
{
  // Set flag to read temperature from sensor
  readTemp = true;
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
