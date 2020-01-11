//Doncey Albin
//(12/12/2019)


// Include the AccelStepper library:
#include <AccelStepper.h>
#include "pitches.h" 

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
// Create a new instance of the AccelStepper class: (Motor Interface Type, Step, Direction)
AccelStepper stepper1 = AccelStepper(1, 3, 2); // Stepper 1
AccelStepper stepper2 = AccelStepper(1, 5, 4); // Stepper 2
AccelStepper stepper3 = AccelStepper(1 , 7, 6); // Stepper 3

// Include LED Display library for I2C
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7);

//Constants
const int PhotoResistor1 = A0; // Photoresistor at Arduino analog pin A0
const int PhotoResistor2 = A1;
const int PhotoResistor3 = A2;
const int RESTART = A7;   
const int LinActuator =  13;
const int BottomOut = 8;
const int Buzzer = 22;


const int Melody2[] PROGMEM = {

// At Doom's Gate (E1M1)
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //1
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,
};

//Variables`
int value; // Store value from photoresistor (0-1023)
int Palue; //Store value from button being pressed
int Bottom; //Store HIGH/LOW value from bottom buttom
int tempo = 225;
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int notes = sizeof(Melody2) / sizeof(Melody2[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;


// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};






void setup() {
  Serial.begin(9600);
  Serial.println("Throw the ball");

//Open Melody
    for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(Buzzer, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(Buzzer);
  }

  // activate LCD module
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  digitalWrite(LinActuator, LOW);

  lcd.home (); // set cursor to 0,0
  lcd.print("Throw the            ");
  lcd.setCursor (0,1); 
  lcd.print("damn ball!                    ");

  // Set the maximum speed in steps per second and accel in steps/s^2:
  stepper1.setMaxSpeed(1000);
  //stepper1.setAcceleration();
  stepper2.setMaxSpeed(1000);
  //stepper2.setAcceleration(10);
  stepper3.setMaxSpeed(1000);
 // stepper3.setAcceleration(10);
  
 Serial.begin(9600);
 pinMode(PhotoResistor1, INPUT); //Set pResistor - A0 pin as an input
 pinMode(PhotoResistor2, INPUT); //Set pResistor - A1 pin as an input 
 pinMode(PhotoResistor3, INPUT); // Set pResistor - A2 pin as an input
 pinMode(RESTART, INPUT); //Set Button- A3 as an input
 pinMode(LinActuator, OUTPUT); //Pin 12 is output pin now
 pinMode(BottomOut, INPUT_PULLUP); //Pin 11 is an input pin for bottom out button
 pinMode(Buzzer, OUTPUT); //Pin 9 is output for Buzzer tone

 digitalWrite(LinActuator, HIGH);

 stepper1.setCurrentPosition(0);
 stepper2.setCurrentPosition(0);
 stepper3.setCurrentPosition(0);

}



void loop() { 
/////////////////////////////////////////////////////////////////////////////////////Initialize Variables//////////////////////////////////////////////////////////////
int  PR1 = analogRead(PhotoResistor1);
int  PR2 = analogRead(PhotoResistor2);
int  PR3 = analogRead(PhotoResistor3);
int  Bottom = digitalRead(BottomOut);


//////////////////////////////////////////////////////////////////////////////////Serial Monitor Read-outs//////////////////////////////////////////////////////////
  Serial.println("PhotoResistor1");
  Serial.println(PR1);
  Serial.println("PhotoResistor2");
  Serial.println(PR2);
  Serial.println("PhotoResistor3");
  Serial.println(PR3);
  Serial.println("RESTART");
  Serial.println(analogRead(RESTART)); 
  Serial.println("Bottom Out");
  Serial.println(Bottom);
  delay(1000);

/////////////////////////////////////////////////////////////////////////////// If statement to flip first cup//////////////////////////////////////////////////////
if (PR1 < 150 && Bottom == 0 && stepper1.currentPosition() == 0){
     for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
  
      // calculates the duration of each note
      divider = pgm_read_word_near(Melody2+thisNote + 1);
      if (divider > 0) {
        // regular note, just proceed
        noteDuration = (wholenote) / divider;
      } else if (divider < 0) {
        // dotted notes are represented with negative durations!!
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5; // increases the duration in half for dotted notes
      }
  
      // we only play the note for 90% of the duration, leaving 10% as a pause
      tone(Buzzer, pgm_read_word_near(Melody2+thisNote), noteDuration * 0.9);
  
      // Wait for the specief duration before playing the next note.
      delay(noteDuration);   
    }   
     noTone(Buzzer);
     
  lcd.home (); // set cursor to 0,0
  lcd.print("Wow                     ");
  lcd.setCursor (0,1); 
  lcd.print("Nice Shot");
  
  digitalWrite(LinActuator, LOW);
  }
  else if (PR1 < 150 && Bottom == 1 && stepper1.currentPosition() == 0) {  
     for (int SA = 0; SA = 200; SA++) {
     stepper1.setSpeed(-SA);
     stepper1.runSpeed();
      if (stepper1.currentPosition() == -100){
      stepper1.setSpeed(0);
      stepper1.runSpeed();
      break;
      }
     }
  }
else if (PR1 < 325 && Bottom == 1 && stepper1.currentPosition() != 0) {
     digitalWrite(LinActuator, HIGH);
     }
    //delay(1000);


////////////////////////////////////////////////////////////////////////////////////// If statement to flip 2nd cup/////////////////////////////////////////////////////////////////////////////////
 if (PR2 < 200 && Bottom==0 && stepper2.currentPosition() == 0){   
 for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
  
      // calculates the duration of each note
      divider = pgm_read_word_near(Melody2+thisNote + 1);
      if (divider > 0) {
        // regular note, just proceed
        noteDuration = (wholenote) / divider;
      } else if (divider < 0) {
        // dotted notes are represented with negative durations!!
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5; // increases the duration in half for dotted notes
      }
  
      // we only play the note for 90% of the duration, leaving 10% as a pause
      tone(Buzzer, pgm_read_word_near(Melody2+thisNote), noteDuration * 0.9);
  
      // Wait for the specief duration before playing the next note.
      delay(noteDuration);   
    }
    noTone(Buzzer);
  lcd.home (); // set cursor to 0,0
  lcd.print("Wow                     ");
  lcd.setCursor (0,1); 
  lcd.print("Nice Shot");
  
  digitalWrite(LinActuator, LOW);
  }
  else if (PR2 < 200 && Bottom == 1 && stepper2.currentPosition() == 0) {  
     for (int SA = 0; SA = 200; SA++) {
     stepper2.setSpeed(-SA);
     stepper2.runSpeed();
      if (stepper2.currentPosition() == -100){
      stepper2.setSpeed(0);
      stepper2.runSpeed();
      break;
      }
     }
  }

  else if (PR2 < 200 && Bottom == 1 && stepper2.currentPosition() != 0) { 
  digitalWrite(LinActuator, HIGH);
  }
  //delay(100);



/////////////////////////////////////////////////////////////////////////////////// If statement to flip 3rd cup //////////////////////////////////////////////////////////////////
 if (PR3 < 220 && Bottom == 0 && stepper3.currentPosition() == 0){   
 for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
  
      // calculates the duration of each note
      divider = pgm_read_word_near(Melody2+thisNote + 1);
      if (divider > 0) {
        // regular note, just proceed
        noteDuration = (wholenote) / divider;
      } else if (divider < 0) {
        // dotted notes are represented with negative durations!!
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5; // increases the duration in half for dotted notes
      }
  
      // we only play the note for 90% of the duration, leaving 10% as a pause
      tone(Buzzer, pgm_read_word_near(Melody2+thisNote), noteDuration * 0.9);
  
      // Wait for the specief duration before playing the next note.
      delay(noteDuration);   
    }
      noTone(Buzzer);
  lcd.home (); // set cursor to 0,0
  lcd.print("Wow                     ");
  lcd.setCursor (0,1); 
  lcd.print("Nice Shot");
  
  digitalWrite(LinActuator, LOW);
  }
  else if (PR3 < 220 == 1 && Bottom == 1 && stepper3.currentPosition() == 0) {  
     for (int SA = 0; SA = 200; SA++) {
     stepper3.setSpeed(SA);
     stepper3.runSpeed();
      if (stepper3.currentPosition() == 100){
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
      }
     }
  }

  else if (PR3 < 200 && Bottom == 1 && stepper3.currentPosition() != 0) { 
  digitalWrite(LinActuator, HIGH);
  }
  //delay(1000);

///////////////////////////////////////////////////////////////// If statement to restart the game ///////////////////////////////////////////////////////////////////////////////////////////////
 if (analogRead(RESTART) > 1000 &&  stepper1.currentPosition() != 0 && stepper3.currentPosition() != 0 && Bottom == 0){
  //LCD Screen output
  lcd.home (); // set cursor to 0,0
  lcd.print("GAME RESET");
  lcd.setCursor (0,1); 
  lcd.print("                       ");
  digitalWrite(LinActuator, LOW);
  delay(3000);
  while(stepper1.currentPosition() != 0 && stepper3.currentPosition() != 0){
  stepper1.setSpeed(100);
  stepper1.runSpeed();
  //stepper2.setSpeed(100);
  //stepper2.runSpeed();
  stepper3.setSpeed(-100);
  stepper3.runSpeed();
  }
  delay(2000);
  digitalWrite(LinActuator, HIGH);
  delay(1000);
 }
}
