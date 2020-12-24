/*
  CCT LED-Controller using PWM Dimming
  
  Utilizing the esp32 buildin ledc function
  https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.c
 */
 
// Set up the led names
uint8_t ledWhite = 27;
uint8_t ledWarm = 25; 

uint8_t ledArray[3] = {1, 2}; // we have two led channels

bool fadedIn = false;
uint8_t iLedWhiteTarget = 255; // the target brightness of White LEDs
uint8_t iLedWarmTarget = 255; // the target brightness of Warm White LEDs
uint32_t iLedWhite = 0; 
uint32_t iLedWarm = 0; 

// the setup routine runs once when you press reset:
void setup() 
{            
  Serial.begin(115200);
  delay(10); 
  
  ledcAttachPin(ledWhite, 1);
  ledcAttachPin(ledWarm, 2);
  
  // Initialize channels 
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(1, 78125, 10); // 78.125 kHz PWM, 10-bit resolution
  ledcSetup(2, 78125, 10);
}

void loop() 
{

  // on a fresh start, fade ma LEDs in
  Serial.println("Fading LEDs in");
  if(!fadedIn){
    for (int i = 0; i <= 255; i++) {
      ledcWrite(1, i);
      ledcWrite(2, i);
      delay(10);
    }
  }
  fadedIn = true;

  Serial.println("Fading LEDs in");
  ledcWrite(1, 255);
  ledcWrite(2, 255);
  delay(2000);
  Serial.println("Send all LEDs a 0 and wait 2 seconds.");
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  delay(2000);
 
}
