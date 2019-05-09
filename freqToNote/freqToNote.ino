#include "tones.h"
#include "kirby.h"
#include <Adafruit_NeoPixel.h>
#include <Adafruit_CircuitPlayground.h>
#include <serial_interface.h> 
#include <stdint.h>

#define LEFT_BUTTON 4
#define RIGHT_BUTTON 19
#define LOG_2 log(2)
#define C4_FREQUENCY 261.63
#define LED_PIN 6
#define LED_COUNT 30

/** Global Vars */
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN);

/** Funcs */
void lightKey(int pitch);
int isIncidental(int semitonesAboveC4);
int notePlayed(int expectedPitch);

/** FFT Setup */
#include "arduinoFFT.h"
 
#define SAMPLES 128             //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];


void setup() {
    /** Init Serial Output */
    init_serial_stdio();
    Serial.begin(9600);

    /** Neopixel setup */
    strip.begin();
    strip.show();
    strip.setBrightness(40);
    
    /** Speaker Init */
    init_speaker();
}

void loop()  {
    static long delta_time_millis = pgm_read_word_near(songage);
    static unsigned long start_time_practice;
    static int started;
    static int arduinoNote;
    static int userNote;
    static int mode = 1;
    
    if (started) {
        static long start_time_millis = millis();
        if (mode) {
          /** Start next note */
          if (start_time_millis + delta_time_millis <= millis()) {            
              // Read in next note
              unsigned int pitch = pgm_read_word_near(songage + 2*arduinoNote + 1);
              unsigned int currDelay = pgm_read_word_near(songage + 2*arduinoNote + 2);
              delta_time_millis += currDelay;
              
              /** Play The Note For Reference*/
              if (pitch != 0) {
                speaker_tone(pitch, currDelay);
                lightKey(pitch);
              }
              arduinoNote += 1;

              if ((arduinoNote + 1)% 20 == 0) {
                start_time_practice = millis();
                mode = 0;
              }
            } 
        } else {
          int expectedPitch = pgm_read_word_near(songage + 2*userNote + 1);
//          expectedPitch = 587;
          if (expectedPitch != 0) {
            lightKey(expectedPitch);
            if (notePlayed(expectedPitch)) {
              unsigned int currDelay = pgm_read_word_near(songage + 2*userNote + 2);
              userNote += 1;
              delay(currDelay);
            }
          } else {
            userNote += 1;
          }
          
          if ((userNote + 1)% 20 == 0) {
            unsigned long end_practice = millis();
            delta_time_millis += (end_practice - start_time_practice);
            mode = 1;
          }
        }
    } else {
        int buttonsPressed = digitalRead(4) && digitalRead(19);
        if (buttonsPressed) {
          //Start Locally
          started = 1;
        }
    }   
}

int notePlayed(int expectedPitch) {
//  /*SAMPLING*/
//  for(int i=0; i<SAMPLES; i++)
//  {
//    microseconds = micros();    //Overflows after around 70 minutes!
// 
//    vReal[i] = analogRead(0);
//    vImag[i] = 0;
// 
//    while(micros() < (microseconds + sampling_period_us)){
//    }
//  }
//  /*FFT*/
//  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
//  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
//  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
//  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
//  printf("Peak: %d", peak);
//  return peak > 100;
  int level = CircuitPlayground.mic.soundPressureLevel(10);
//  printf("Level %d\n", level);
  return level > 80;
}

/** Lights the correct key for a given pitch */
void lightKey(int pitch) {
  /** Hard coded bin sizes tell how to truncate incidental values into lower key*/
  static int binSizes[] = {1, 3, 4, 6, 8, 10, 11, 13, 15, 19, 20, 22, 24, 25, 27};

  int semitones_above_c4; 
  
  if (pitch < 261) {
      semitones_above_c4 = 0;
  } else if (pitch < 277) {
      semitones_above_c4 = 1;
  } else if (pitch < 293) {
      semitones_above_c4 = 2;
  } else if (pitch < 311) {
      semitones_above_c4 = 3;
  } else if (pitch < 329) {
      semitones_above_c4 = 4;
  } else if (pitch < 349) {
      semitones_above_c4 = 5;
  } else if (pitch < 369) {
      semitones_above_c4 = 6;
  } else if (pitch < 391) {
      semitones_above_c4 = 7;
  } else if (pitch < 415) {
      semitones_above_c4 = 8;
  } else if (pitch < 440) {
      semitones_above_c4 = 9;
  } else if (pitch < 466) {
      semitones_above_c4 = 10;
  } else if (pitch < 494) {
      semitones_above_c4 = 11;
  } else if (pitch < 523) {
      semitones_above_c4 = 12;
  } else if (pitch < 554) {
      semitones_above_c4 = 13;
  } else if (pitch < 587) {
      semitones_above_c4 = 14;
  } else if (pitch < 622) {
      semitones_above_c4 = 15;
  } else if (pitch < 659) {
      semitones_above_c4 = 16;
  } else if (pitch < 698) {
      semitones_above_c4 = 17;
  } else if (pitch < 740) {
      semitones_above_c4 = 18;
  } else if (pitch < 784) {
      semitones_above_c4 = 19;
  } else if (pitch < 830) {
      semitones_above_c4 = 20;
  } else if (pitch < 880) {
      semitones_above_c4 = 21;
  } else if (pitch < 932) {
      semitones_above_c4 = 22;
  } else if (pitch < 987) {
      semitones_above_c4 = 23;
  } else {
      semitones_above_c4 = 24;
  }

  /** Turn frequency into discrete key*/
//  inxt semitones_above_c4 = (14 * ((double) log((double) pitch/C4_FREQUENCY)/log(2))) - 1;
  printf("Tones Above %d\n", semitones_above_c4);

  /** Truncate incidentals */
  int key = 0;
  while (binSizes[key+1] < semitones_above_c4 && key < 7) {
    key++;
  }

  /** Map to correct key */
  key = key * 2 + 3;

  /** Light neopixel */
  strip.clear();
  if (isIncidental(semitones_above_c4)) {
    strip.setPixelColor(key, 0, 255, 0);
  } else {
    strip.setPixelColor(key, 255, 0, 0);
  }
  strip.show();
}

/** Returns if a given semitone corresponds to a incidental relative to c3 */
int isIncidental(int semitonesAboveC4) {
//    static int incidentalKeys[] = {1, 3, 6, 8,11, 13, 15, 18, 20, 23, 25, 27};
//    for (int i = 0; i < 12; i += 1) {
//      if (incidentalKeys[i] == semitonesAboveC4) {
//        return 1;
//      }
//    }
    int relPos = semitonesAboveC4 % 14;
    return (relPos == 1 || relPos == 3 || relPos == 6 || relPos == 8 || relPos == 11 ||
    relPos == 13 || relPos == 15);
}
