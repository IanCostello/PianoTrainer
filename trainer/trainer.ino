#include <Wire.h>
#include <neopixel_blit.h>
#include <serial_interface.h>
#include <Arduino.h>
#include <stdio.h>
#include "tones.h"
#include "track-0-voice-1.h"

// songage = { {left_hand_data}, {right_hand_data}};
//int songage[2][4];
int bpm = 120;

int mode;

Pixels blank = {{0}};
Pixels left_hand_pixels = {{2, 0 , 0}, {2, 0 , 0}, {2, 0 , 0}, {2, 0 , 0}, {2, 0 , 0}, {0, 0 , 0}, {0, 0 , 0}, {0, 0 , 0}, {0, 0 , 0}, {0, 0 , 0}};
Pixels right_hand_pixels = {{0, 0 , 0}, {0, 0 , 0}, {0, 0 , 0}, {0, 0 , 0}, {0, 0 , 0}, {2, 0 , 0}, {2, 0 , 0}, {2, 0 , 0}, {2, 0 , 0}, {2, 0 , 0}};

 
void setup() {
  init_speaker();
  init_serial_stdio(); // allows us to call printf
  init_neopixel_blit(); // resets lights

  pinMode(21, INPUT); // switch
  
  mode = (digitalRead(21) == HIGH)? 1 : 0;
}

void loop() {
  static int hand_i;
  static int current_note;
  static int t_0;
  static int left_sum;
  static int right_sum;
  static int current_note_l;
  static int current_note_r;  
  static int user_note;
  static int arduino_note;
  static int user_playing;
  static int pitch;
  static int duration;
  static int previous_time;
  static int read_pitch;
  
  
  if (digitalRead(21) == HIGH){ // if switch is on
    
    // Guided mode
    // Teaches a song one hand at a time (left=0, right=1). The corresponding side of the neopixels will be lit up on the arduino itself
    //    to show the user which hand they should be using
    // It will light a single neopixel, then wait until it hears the corresponding pitch being played (within an acceptable range)
    // Once the pitch matches, it switches to the next note in the song
    // Once the user gets through the whole song, it switches to the other hand
    // this continues until the user switches to practice mode

    if(mode == 0) { // just switched to this mode; setup
      mode = 1;
      
      /*hand_i = 0;   // start with teaching the left hand
      // set neopixels on arduino to the left
      neopixel_blit(left_hand_pixels);
      */
      
      user_note=0;
      arduino_note=0;

      // setup for arduino to play the first ten notes
      user_playing = 0;
      previous_time = millis();
      pitch = 0;
      duration = pgm_read_word_near(songage);
    }

    
    if(user_playing){
      
      read_pitch = 0;
      
      if(read_pitch == pitch){
        user_note+=2;
        pitch = pgm_read_word_near(songage + user_note);

        
        // light neopixels
        lightKey(pitch);

        if(user_note%20 == 0)
          user_playing = 0;
      }
      
    }
    else{
      if(millis() - previous_time >= duration){
        if(arduino_note%20 == 0) {
          user_playing = 1;
          // blank neopixels
          pitch = pgm_read_word_near(songage + user_note);
        }
        else{
          previous_time = millis();
        
          pitch = pgm_read_word_near(songage + arduino_note + 1);
          duration = pgm_read_word_near(songage + arduino_note + 2);
          arduino_note+=2;

          // set neopixels
          lightKey(pitch);
          speaker_tone(pitch, duration);
        }
      }
    }

    /*
    // if they reached the end of the array, switch hands
    // TODO: change 4 to length, or check if they've hit -1 or some other marker
    //       could play a few notes on the arduino to indicate that they successfully got through that hand
    if(current_note==4) {
      if(hand_i == 1) {
        hand_i = 0;
        // set neopixels on arduino to the left
        neopixel_blit(left_hand_pixels);
      }
      else {
        hand_i = 1;
        // set neopixels on arduino to the right
        neopixel_blit(right_hand_pixels);
      }
    }
    */
  }
 /* else { // switch is off
    
    // Practice mode
    // this mode lights the neopixels for both hands in time with the song, so the user can practice at full speed
    // it also acts like a metronome, playing a sound on the beat to keep the user in time
    
    // NOTE: current structure doesn't listen to see how well they played, since it's both hands simultaneously;
    //    but if this is something the Fourier transform can get around, it can be added fairly easily
    
    if(mode == 1) { // just switched to this mode; setup
      mode = 0;
      
      // set starting time, and no time has ellapsed since then so the sums are 0
      t_0 = millis();
      left_sum = 0; // read duration of first note
      right_sum = 0;  // read duration of first note
      
      // variables for parsing the array
      current_note_l = 0;
      current_note_r = 0;
            
      // blank neopixels
      neopixel_blit(blank);

      // set neo_pixels to the first note for left and right
    }

    // metronome
    //    could add a visual element if the arduino neopixels aren't being used
    //    like turning on another neopixel each beat
    // 60,000 (milliseconds per minute) / 120 (beats per minute) = 500 milliseconds per beat
    // TODO: give a range, it might a beat if it has to be the exact millisecond (but don't play it twice)
    if( (millis()-t_0) % (60000/bpm) == 0) {  
      // play sound
    }

    // left hand
    if( t_0+left_sum <= millis() ) {
      // change to the next note
      current_note_l++;
      
      // add the duration to the sum
      left_sum += 0;

      // change neopixels
      // set old note to 0, set new note to the color
    }

    // right hand
    if( t_0+right_sum <= millis() ) {
      // change to the next note
      current_note_r++;
      // add the duration to the sum
      right_sum += 0;
      
      // change neopixels
      // set old note to 0, set new note to the color
    }

  } // end practice mode block
*/
} // end loop()
