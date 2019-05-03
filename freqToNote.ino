#include "tones.h"
#include "track-0-voice-1.h"
#include <neopixel_blit.h>
#include <Wire.h>
#include <serial_interface.h> 
#include <stdint.h>
#define LEFT_BUTTON 4
#define RIGHT_BUTTON 19
#define LOG_2 log(2)

/** Global Vars */
/** Delay till next note*/
/** Song start triggered */
int started;
int currentNote;
#define A4_FREQUENCY 440

int currDelay;
long last_time_millis;

void setup() {
    /** Output Init */
    Serial.begin(9600);

    /** Init LEDs */
    init_neopixel_blit();
    init_serial_stdio();

    /** Speaker Init */
    init_speaker();
    started = 0;
    currentNote = 0;

    /** Get first delay */
    currDelay = pgm_read_word_near(songage);
    last_time_millis = millis();
}

void loop()  {
    static int pitch;
    
    if (started) {
        /** Get time of lat  */
        long current_time = millis();
        long diff_time = current_time - last_time_millis;

        if (diff_time >= currDelay) {
            // Update the time
            last_time_millis = current_time;
            
            // Read in next note
            pitch = pgm_read_word_near(songage + 2*currentNote + 1);
            currDelay = pgm_read_word_near(songage + 2*currentNote + 2);

            int semitones_above_a4 = 12 * (double) log((double) pitch/A4_FREQUENCY)/log(2);

            printf("Curr Pitch %d Semitone Above %d \n", pitch, semitones_above_a4);
            if (pitch == 0) {
              
            } else {
//              speaker_tone(pitch, currDelay);
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

void togglePixels(int chords[]) {
//  Pixels pix = {{0}};
//  int blue = diff / 3;
//  int red = (100 - diff) / 3;
//  int green = ((diff * diff) % 100) / 3;
//  for (int i = 0; i < 10; i++) {
//      pix[i][PIX_BLU] = blue;
//      pix[i][PIX_RED] = red;
//      pix[i][PIX_GRN] = green;
//  }
//  neopixel_blit(pix);
}
