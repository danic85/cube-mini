#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    3
#define MOTION_PIN  10
#define SOUND_PIN   A6
#define SOUND_SAMPLE_WINDOW 50

//#define DEBUG

unsigned int sample;
CRGB leds[NUM_LEDS];
int breath;
boolean breathe_in;
unsigned long breath_pause;
unsigned long last_movement;
 
void setup() 
{
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTION_PIN, INPUT);
  pinMode(SOUND_PIN, INPUT);
  #ifdef DEBUG
    Serial.begin(9600);
  #endif

  breath = 0;
  breathe_in = true;
  breath_pause = millis();
  last_movement = millis();
}
 
 
void loop() 
{
  int motion = digitalRead(MOTION_PIN);
  double sound = 0.0;
  if (motion == LOW && last_movement < millis() - 10000) {
    // I'm asleep becauase i haven't felt you move for 10 seconds
    breathe();
    leds[0] = CRGB(0, 0, breath_sin());
    leds[1] = CRGB(0, 0, breath_sin());
    leds[2] = CRGB(0, 0, 0);
    delay(100);
  }
  else {
    // I'm awake and listening to noises
    sound = sample_sound();
    leds[0] = CRGB(0, 0, sound);
    leds[1] = CRGB(0, 0, sound);
    leds[2] = CRGB(0, 0, sound);
    if (motion == HIGH) {
      last_movement = millis();
    }
  }

  #ifdef DEBUG
    Serial.print(motion * 100);
    Serial.print(',');
    Serial.println(sound);
  #endif
  
  FastLED.show();
}

double sample_sound() {
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  
  // collect data for 50 mS
  while (millis() - startMillis < SOUND_SAMPLE_WINDOW)
  {
    sample = analogRead(A6);
    if (sample < 1024)  // toss out spurious readings
    {
       if (sample > signalMax)
       {
          signalMax = sample;  // save just the max levels
       }
       else if (sample < signalMin)
       {
          signalMin = sample;  // save just the min levels
       }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
  
  return peakToPeak;
}

int breath_sin() {
  return (sin(breath * 0.0174533) + 1) * 80 - 80;
}

int breathe() {
  int step = 2;
  int next;
  if (breath_pause > millis() - 2000) {
    return;
  }
  if (breathe_in == true) {
    next = breath = breath + step;
    if (breath > 80) {
      next = 80;
      breathe_in = false;
    }
  }
  else {
    next = breath = breath - step;
    if (breath < 0) {
      next = 0;
      breath_pause = millis();
      breathe_in = true;
    }
  }
  breath = next;
  return breath;
}
