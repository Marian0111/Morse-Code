#include <LiquidCrystal.h>
#include <Bounce2.h>
#include "characters.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Bounce debouncer = Bounce(); // Create Bounce object

int buttonPin = 52;
int redLedPin = 40;
int greenLedPin = 46;
int buzzerPin = 22;
int okGreenLight = 1;

int buttonState;
unsigned long buttonPressStartTime = 0;
unsigned long waitingTime = 0;
unsigned long debounceDelay = 100; // Adjust this value based on your requirements

int character[5] = {-1, -1, -1, -1, -1};
int offset = 0;

enum ButtonState { IDLE, WAIT_FOR_RELEASE };

ButtonState currentState = IDLE;

const int noteA4 = 440;

void playNote(int frequency, int duration) {
  tone(buzzerPin, frequency, duration);
  delay(duration);  // Add a small delay to separate notes
  noTone(buzzerPin);
}

void setup() {
  lcd.clear();
  lcd.createChar(0, point);
  lcd.createChar(1, line);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  debouncer.attach(buttonPin);
  debouncer.interval(debounceDelay);

  lcd.setCursor(0, 0);
}

void loop() {
  debouncer.update();
  buttonState = debouncer.read();

  switch (currentState) {
    case IDLE:
      if (buttonState == LOW) { // Button is pressed
        buttonPressStartTime = millis();
        digitalWrite(redLedPin, HIGH);
        currentState = WAIT_FOR_RELEASE;
        if(okGreenLight == 1){
          lcd.clear();
          lcd.setCursor(0, 0);
          for(int i = 0; i < 5; i++){
            character[i] = -1;
          }
          okGreenLight = 0;
          offset = 0;
        }
        waitingTime = 0;
      }
      break;

    case WAIT_FOR_RELEASE:
      if (buttonState == HIGH) { // Button is released
        digitalWrite(redLedPin, LOW);
        //digitalWrite(buzzerPin, LOW);
        unsigned long buttonPressDuration = millis() - buttonPressStartTime;

        waitingTime = millis();

        if (buttonPressDuration < 500 && offset < 5) {
          playNote(noteA4, 50);
          lcd.write(byte(0));
          character[offset] = 0;
          offset++;
        } else if(offset < 5){
          playNote(noteA4, 500);
          lcd.write(byte(1));
          character[offset] = 1;
          offset++;
        }

        currentState = IDLE;
      }
      break;
  }
  
  if(okGreenLight == 1){
    digitalWrite(greenLedPin, HIGH);
  }else{
    digitalWrite(greenLedPin, LOW);
    unsigned long waitingDuration = millis() - waitingTime;
    if(waitingDuration >= 2000 && waitingTime != 0){
      int charcaterExists = 0;
      for(int i = 0; i < 36; i++){
        int charcaterFound = 1;
        for(int j = 0; j < 5; j++){
          if(decoder[i][j] != character[j]){
            charcaterFound = 0;
          }
        }
        if(charcaterFound == 1){
          lcd.createChar(2, customChars[i]);
          lcd.setCursor(6, 0);
          lcd.write(byte(2));
          charcaterExists = 1;
          break;
        }
      }
      if(charcaterExists == 0){
        lcd.setCursor(6, 0);
        lcd.print("UNDEFINED");
      }
      okGreenLight = 1;
    }
  }
}
