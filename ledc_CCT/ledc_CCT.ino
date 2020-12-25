/*
  CCT LED-Controller using PWM Dimming
  
  Utilizing the esp32 buildin ledc function
  https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.c

  todo: use  map(val, 0, 1, 0, 4095) to convert floats from api?
  
 */
 
// Set up the led names
uint8_t ledWhite = 27;
uint8_t ledWarm = 25; 

// Set up the potis as digital input
uint8_t potiWhite = 34;
uint8_t potiWarm = 33;

uint8_t ledArray[3] = {1, 2}; // we have two led channels

bool fadedIn = false;
uint16_t iLedWhiteTarget = 4095; // the target brightness of White LEDs
uint16_t iLedWarmTarget = 4095; // the target brightness of Warm White LEDs
uint8_t iLedWhite = 0; 
uint8_t iLedWarm = 0; 

// software smooting pot inputs
const int numReadings = 20;

uint16_t readingsWhite[numReadings];      // the readings from the analog input
uint16_t averageWhite = 0;                // the average
uint32_t totalWhite = 0;                  // the running total

uint16_t readingsWarm[numReadings];            
uint16_t averageWarm = 0;         
uint32_t totalWarm = 0;   

uint8_t readIndex = 0;

// the setup routine runs once when you press reset:
void setup() 
{            
  Serial.begin(19200);
  delay(10); 
  
  ledcAttachPin(ledWhite, 1);
  ledcAttachPin(ledWarm, 2);
  
  pinMode(potiWhite, INPUT_PULLUP);
  pinMode(potiWarm, INPUT_PULLUP);
  
  // Initialize channels 
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(1, 32000, 12); // 32 kHz PWM, 12-bit resolution, means 4096 dimming steps
  ledcSetup(2, 32000, 12);
  // we utilize here 12 bits, since the analog read from out poti is also in 12 bits.
  // means we could use the input directly to drive our PWM duty.
  // but could cause flickering due to small changes on the value

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readingsWhite[thisReading] = 0;
    readingsWarm[thisReading] = 0;
  }
  
}

void loop() 
{

  /*
   *  Fade in
   * 
   */
  // on a fresh start, fade ma LEDs in to the correct poti value
  if(!fadedIn){
    Serial.println("Fading LEDs in");
    for (int i = 0; i <= 4095; i+=8) {
      if(i < analogRead(potiWhite)) ledcWrite(1, i);
      if(i < analogRead(potiWarm)) ledcWrite(2, i);
      delay(1);
    }
  }
  fadedIn = true;


  /*
   *  Software Smooting
   * 
   */
  // subtract the last reading:
  totalWhite = totalWhite - readingsWhite[readIndex];
  totalWarm = totalWarm - readingsWarm[readIndex];

  // read from the sensor:
  readingsWhite[readIndex] = analogRead(potiWhite);
  readingsWarm[readIndex] = analogRead(potiWarm);
  // add the reading to the total:
  totalWhite = totalWhite + readingsWhite[readIndex];
  totalWarm = totalWarm + readingsWarm[readIndex];

  // advance to the next position in the array:
  readIndex = readIndex + 1;
  delay(1);

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  averageWhite = round(totalWhite / numReadings);
  averageWarm = round(totalWarm / numReadings);


  /*
   *  Debug
   * 
   */
  Serial.print("White: ");
  Serial.print(averageWhite);
  Serial.print("\nWarm: ");
  Serial.print(averageWarm);
  
  Serial.println("\n---\n");

  /*
   *  Write it
   * 
   */
  // directly writing that value, but it may be flickering due to small changes on these values.. we should smooth that out
  ledcWrite(1, averageWhite);
  ledcWrite(2, averageWarm);
  delay(1);
 
}
