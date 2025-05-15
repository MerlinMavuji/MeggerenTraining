#include <avr/wdt.h> /* Header for watchdog timers in AVR */
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library for ILI9341
#include <SPI.h>
#include <string.h>
#include "button.h"

// Pin definitions for the Arduino Mega
#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9
#define BUTTON_PIN  7 // nav btn
#define CONFIRM_PIN 6
#define SET_PIN 53

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Menu options
bool kort_submenu = false;
bool cont_submenu = false;
bool error = false;
bool setted = false;
bool freez = false;
bool drawed = false;
bool answerSubmitted = false;
bool resultdrawed = false;
bool is_random = false;

const char* menuOptions[] = {"Isolatiefout", "Continuiteit","Goed","Random"};
const char* kortOptions[] = {"N en L1","N en L2","N en L3", "L1 en L2","L1 en L3", "L2 en L3", "Back"};
const char* contOptions[] = {"CN", "CL1", "CL2", "CL3", "Back"};
const char* rps[] = {"N en L1","N en L2","N en L3", "L1 en L2","L1 en L3", "L2 en L3","CN", "CL1", "CL2", "CL3","Goed","Random"};
const char* currentSelectedOption = "";
const char* answer = "";
const char* correctAnswer = "";
const char* detailsAnswer = " blank..";

int rpsSize = sizeof(rps) / sizeof(rps[0]);
int menuSize = sizeof(menuOptions) / sizeof(menuOptions[0]);
int kortmenuSize = sizeof(kortOptions) / sizeof(kortOptions[0]);
int contmenuSize = sizeof(contOptions) / sizeof(contOptions[0]);
int ranpin = 1;
int conpin = 2;
int isopin = 3;
int goedpin = 4;
int buzzer_pin = 11;

const int relayPins[] = {14, 15, 16, 17, 18, 19, 20, 21};
const char* relayLetterPins[] = {"N", "L1","L2", "L3", "N", "L1", "L2", "L3"};
const int contPins[] = {14,15,16,17};
const int isoPins[] = {18,19,20,21};

const int numRelays = 8;

int currentOption = 0;
const char* currentMenu = "";

#define BUZZER_PIN 11

// Fréquences des notes en Hz
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS5 554
#define NOTE_D6  1175
#define NOTE_DS5 622
#define NOTE_FS5 740

void setup() {
  Serial.begin(9600);
  Serial.println("ILI9341 TFT LCD Menu Test");

  for (int i = 0; i < numRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }
  randomSeed(analogRead(0));

  tft.begin();
  Serial.println("Display initialized");

  tft.setRotation(1);
  Serial.println("Rotation set");

  drawLoadingScreen(); // Draw the loading screen

  tft.fillScreen(ILI9341_BLACK);
  Serial.println("Screen cleared");

  pinMode(buzzer_pin, INPUT_PULLUP);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(CONFIRM_PIN, INPUT_PULLUP);

  pinMode(conpin, INPUT_PULLUP);
  pinMode(isopin, INPUT_PULLUP);
  pinMode(goedpin, INPUT_PULLUP);
  pinMode(ranpin, INPUT_PULLUP);

  Serial.println(contmenuSize);
  Serial.println(kortmenuSize);
  Serial.println(menuSize);

  currentMenu = menuOptions[0];
  drawMenu();

  wdt_disable();  /* Disable the watchdog and wait for more than 2 seconds */
  delay(3000);  /* Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration */
  wdt_enable(WDTO_2S);  /* Enable the watchdog with a timeout of 2 seconds */
}

void loop() {
  wdt_reset();  /* Reset the watchdog */

  if(!setted){
    randomBtn.update();
    noTone(buzzer_pin);
    btnBTN.update();
    confBtn.update();
    setBtn.update();
  }

  if (setted) {
    Serial.println("SETTTTED");

    if (digitalRead(conpin) == LOW) {
      Serial.println("goedpin");
      delay(40); // Debounce delay
      if (digitalRead(conpin) == LOW) {
        if (!answerSubmitted && !drawed) {
          AnswerIsCon();
          showResult();
          drawed = true;
          answerSubmitted = true;
        }
        while (digitalRead(conpin) == LOW);
      }
    }

    if (digitalRead(isopin) == LOW) {
      Serial.println("isopin");
      delay(40); // Debounce delay
      if (digitalRead(isopin) == LOW) {
        if (!answerSubmitted && !drawed) {
          AnswerIsIso();
          showResult();
          drawed = true;
          answerSubmitted = true;
        }
        while (digitalRead(isopin) == LOW);
      }
    }

    if (digitalRead(goedpin) == LOW) {
      Serial.println("goedpin");
      delay(40); // Debounce delay
      if (digitalRead(goedpin) == LOW) {
        if (!answerSubmitted && !drawed) {
          AnswerIsGoed();
          showResult();
          drawed = true;
          answerSubmitted = true;
        }
        while (digitalRead(goedpin) == LOW);
      }
    }

    if (answerSubmitted && !resultdrawed) {
      Serial.println(correctAnswer);
      showResult();
      resultdrawed = true;
    }

    if (digitalRead(CONFIRM_PIN) == LOW) {
      delay(40); // Debounce delay
      if (digitalRead(CONFIRM_PIN) == LOW) {
        kort_submenu = false;
        cont_submenu = false;
        error = false;
        setted = false;
        freez = false;
        drawed = false;
        answerSubmitted = false;
        resultdrawed = false;
        detailsAnswer = "";
        currentSelectedOption = menuOptions[0];
        drawMenu();
        while (digitalRead(CONFIRM_PIN) == LOW);
      }
    }
  }
}
