#include "tones.h"
#include "track-0-voice-1.h"
#include <Adafruit_NeoPixel.h>
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
    static int started;
    static int currentNote;
    
    if (started) {
        static long start_time_millis = millis();
        /** Start next note */
        if (start_time_millis + delta_time_millis <= millis()) {            
            // Read in next note
            unsigned int pitch = pgm_read_word_near(songage + 2*currentNote + 1);
            unsigned int currDelay = pgm_read_word_near(songage + 2*currentNote + 2);
            delta_time_millis += currDelay;

            /** Calculate discrete note to be played */
            int semitones_above_c4 = 12 * (double) log((double) pitch/C4_FREQUENCY)/log(2);
            printf("Curr Pitch %d Semitone Above %d \n", pitch, semitones_above_c4);

            
            /** Play The Note For Reference*/
            if (pitch != 0) {
              speaker_tone(pitch, currDelay);
              lightKey(semitones_above_c4);
            }
            currentNote += 1;
        } 
    } else {
        int buttonsPressed = digitalRead(4) && digitalRead(19);
        if (buttonsPressed) {
          //Start Locally
          started = 1;
        }
    }   
}

/** Lights the correct key for a given pitch */
void lightKey(int pitch) {
  /** Hard coded bin sizes tell how to truncate incidental values into lower key*/
  static int binSizes[] = {2, 4, 5, 7, 9, 11, 12, 14, 16, 20, 19, 21, 23, 25, 26, 28};

  /** Turn frequency into discrete key*/
  int semitones_above_c4 = 12 * (double) log((double) pitch/C4_FREQUENCY)/log(2);

  /** Truncate incidentals */
  int key = 0;
  while (binSizes[key+1] < semitonesAboveC4 && key < 7) {
    key++;
  }

  /** Map to correct key */
  key = key * 2;

  /** Light neopixel */
  strip.clear();
  if (isIncidental(semitonesAboveC4)) {
    strip.setPixelColor(key, 0, 255, 0);
  } else {
    strip.setPixelColor(key, 255, 0, 0);
  }
  strip.show();
}

/** Returns if a given semitone corresponds to a incidental relative to c3 */
int isIncidental(int semitonesAboveC4) {
    static int incidentalKeys[] = {1, 3, 6, 8,11, 13, 15, 18, 20, 23, 25, 27};
    for (int i = 0; i < 12; i += 1) {
      if (incidentalKeys[i] == semitonesAboveC4) {
        return 1;
      }
    }
    return 0;
}
