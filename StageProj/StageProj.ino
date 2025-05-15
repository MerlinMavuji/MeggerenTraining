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

void playWrongAnswer() {
    tone(BUZZER_PIN, NOTE_C5, 200);
    delay(250);
    tone(BUZZER_PIN, NOTE_G4, 200);
    delay(250);
    tone(BUZZER_PIN, NOTE_E4, 400);
    delay(500);
    noTone(BUZZER_PIN);
}

void playGoodAnswer() {
    int melody[] = {NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_B5};
    int tempo = 105;
    int notes = sizeof(melody) / sizeof(melody[0]) / 2;
    int wholenote = (60000 * 4) / tempo;
    int divider = 0, noteDuration = 0;

    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
        divider = melody[thisNote + 1];
        if (divider > 0) {
            noteDuration = (wholenote) / divider;
        } else {
            noteDuration = (wholenote) / abs(divider) * 1.5;
        }
        tone(BUZZER_PIN, melody[thisNote], noteDuration * 0.9);
        delay(noteDuration);
        noTone(BUZZER_PIN);
    }
}

// Function prototypes
void SET();

void StuRandom(){
    SET();
}

void drawSetted()
{
    tft.fillScreen(ILI9341_MAROON);
        tft.fillScreen(ILI9341_MAROON);
    tft.setCursor(20, 20);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.println(currentSelectedOption);
    tft.setCursor(10, 90);
    tft.setTextSize(2);
    tft.println("Annuleren met:");
    tft.setCursor(10, 120);
    tft.println("Drukknop 'Confirm'");
    setted = true;
}

void AnswerIsCon(){
  answer = "Continuiteit";
  answerSubmitted = true;
}

void AnswerIsIso(){
  answer = "Isolatiefout";
  answerSubmitted = true;
}

void AnswerIsGoed(){
  answer = "Goed";
  answerSubmitted = true;
}

void turnOffAllRelays() {
  for (int i = 0; i < numRelays; i++) {
    digitalWrite(relayPins[i], LOW);
  }
}

void turnOnAllRelays() {
  for (int i = 0; i < numRelays; i++) {
        digitalWrite(relayPins[i], HIGH);
  }
}

const char* getTxt(int pin){
  switch(pin){
    case 14:
        return "N";
    case 15:
        return "L1";
    case 16:
        return "L2";
    case 17:
        return "L3";
    case 18:
        return "N";
    case 19:
        return "L1";
    case 20:
        return "L2";
    case 21:
        return "L3";
  }
}

void Randomize() {
  is_random = true;
  int as = random(3);
  if(as == 0){
      turnOffAllRelays();
      int randomRelay1 = random(4) + 18;
      int randomRelay2;
      do {
        randomRelay2 = random(4) + 18;
      } while (randomRelay2 == randomRelay1);

      const char* temp = getTxt(randomRelay1);
      const char* temp2 = getTxt(randomRelay2);

      if (detailsAnswer != nullptr) {
          free(detailsAnswer);
      }
      detailsAnswer = (char*)malloc(strlen(temp) + strlen(temp2) + 1);
      if (detailsAnswer == nullptr) {
          Serial.println("Memory allocation failed");
          return;
      }

      // Concatenate temp and temp2 into detailsAnswer
      strcpy(detailsAnswer, temp);
      strcat(detailsAnswer, " en ");
      strcat(detailsAnswer, temp2);

      digitalWrite(randomRelay1, HIGH);
      digitalWrite(randomRelay2, HIGH);
      correctAnswer = "Isolatiefout";

      is_random = false;
  }
  else if(as == 1){
      turnOffAllRelays();
      int randomRelay = random(4) + 14;

      detailsAnswer = getTxt(randomRelay);

      digitalWrite(randomRelay, HIGH);
      correctAnswer = "Continuiteit";

      is_random = false;
  }
  else if(as == 2){
    turnOffAllRelays();
    correctAnswer = "Goed";
    is_random = false;
    detailsAnswer = "Goed";
  }
}

void SET(){
  Serial.println("SET button pressed");
      turnOffAllRelays();

  if(currentSelectedOption == "Random"){
    Randomize();
  }
  else if(currentSelectedOption == "Goed" )
  {
    correctAnswer = "Goed";
        detailsAnswer = "Goed";

  } else if(currentSelectedOption == "N en L1"){
        digitalWrite(isoPins[0], HIGH);
        digitalWrite(isoPins[1], HIGH);
                            correctAnswer = "Isolatiefout";
                                detailsAnswer = "N en L1";

  } else if(currentSelectedOption == "N en L2"){
            digitalWrite(isoPins[0], HIGH);
        digitalWrite(isoPins[2], HIGH);
                    correctAnswer = "Isolatiefout";
                                                    detailsAnswer = "N en L2";

  } else if(currentSelectedOption == "N en L3"){
            digitalWrite(isoPins[0], HIGH);
        digitalWrite(isoPins[3], HIGH);
                            correctAnswer = "Isolatiefout";
                                                            detailsAnswer = "N en L3";

  } else if(currentSelectedOption == "L1 en L2"){
            digitalWrite(isoPins[1], HIGH);
        digitalWrite(isoPins[2], HIGH);
                            correctAnswer = "Isolatiefout";
                                                            detailsAnswer = "L1 en L2";

  } else if(currentSelectedOption == "L1 en L3"){
            digitalWrite(isoPins[1], HIGH);
        digitalWrite(isoPins[3], HIGH);
                            correctAnswer = "Isolatiefout";
                                                                                        detailsAnswer = "L1 en L3";

detailsAnswer = "L1 en L3";
  } else if(currentSelectedOption == "L2 en L3"){
            digitalWrite(isoPins[2], HIGH);
        digitalWrite(isoPins[3], HIGH);
                            correctAnswer = "Isolatiefout";
                                                                                        detailsAnswer = "L2 en L3";

detailsAnswer = "L2 en L3";
  } else if(currentSelectedOption == "CN"){
            digitalWrite(contPins[0], HIGH);
                        correctAnswer = "Continuiteit";
                        detailsAnswer = "N";

  } else if(currentSelectedOption == "CL1"){
                digitalWrite(contPins[1], HIGH);
                            correctAnswer = "Continuiteit";
                                                                                        detailsAnswer = "L1";

detailsAnswer = "CL1";
  } else if(currentSelectedOption == "CL2"){
            digitalWrite(contPins[2], HIGH);
                        correctAnswer = "Continuiteit";
                            detailsAnswer = "L3";

detailsAnswer = "CL2";

  } else if(currentSelectedOption == "CL3"){
                digitalWrite(contPins[3], HIGH);
                            correctAnswer = "Continuiteit";
                            detailsAnswer = "L3";
  }
  setted = true;
  drawSetted();
}

bool checkAnswer(){
if (strcmp(answer, correctAnswer) == 0){
    Serial.println("Correct Answer");
    return true;
  }else{
        Serial.println("Wrong Answer");
    return false;
  }
}

void showResult(){
  Serial.println("Showing result");
  bool check = checkAnswer();
  if(check){
    tft.fillScreen(ILI9341_OLIVE);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 30);
    tft.println("Juiste antwoord:");
    tft.setCursor(20, 80);
        tft.setTextSize(3);
    tft.println(detailsAnswer);
    tft.setCursor(20, 150);
        tft.setTextSize(2);
    tft.println("Antwoord van student is:");
        tft.setCursor(20, 180);
            tft.setTextSize(3);
    tft.println(answer);
    playGoodAnswer();
  }
  else
  {
    tft.fillScreen(ILI9341_RED);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.setCursor(20, 30);
    tft.println("Juiste antwoord:");
    tft.setCursor(20, 80);
        tft.setTextSize(3);
    tft.println(detailsAnswer);
    tft.setCursor(20, 150);
        tft.setTextSize(2);
    tft.println("Antwoord van student is:");
        tft.setCursor(20, 180);
            tft.setTextSize(3);
    tft.println(answer);
    playWrongAnswer();
  }
}

void drawMenu() {
  if (!kort_submenu && !cont_submenu && !error) {
  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(20, 20);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
    tft.println("Menu:");
      tft.setTextSize(4);

    for (int i = 0; i < menuSize; i++) {
      tft.setCursor(20, 100);
      if (i == currentOption) {
        tft.setTextColor(ILI9341_BLACK);
                 currentSelectedOption = menuOptions[i];
      tft.println(menuOptions[i]);
      }
    }
  } else if (kort_submenu && !error) {
  tft.fillScreen(ILI9341_DARKCYAN);
  tft.setCursor(20, 20);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
    tft.println("Iso menu:");
          tft.setTextSize(4);

    for (int i = 0; i < kortmenuSize; i++) {
      tft.setCursor(20, 100);
      if (i == currentOption) {
        tft.setTextColor(ILI9341_WHITE);
         currentSelectedOption = kortOptions[i];
               tft.println(kortOptions[i]);

      }
    }
    correctAnswer = "Isolatiefout";
  } else if (cont_submenu && !error) {
  tft.fillScreen(ILI9341_CYAN);
  tft.setCursor(20, 20);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
    tft.println("Cont menu:");
          tft.setTextSize(4);

    for (int i = 0; i < contmenuSize; i++) {
      tft.setCursor(70, 100);
      if (i == currentOption) {
        tft.setTextColor(ILI9341_BLACK);
                 currentSelectedOption = contOptions[i];
                       tft.println(contOptions[i]);

      }
          correctAnswer = "Continuiteit";
    }
  } else if(error) {
    tft.fillScreen(ILI9341_RED);
    tft.setCursor(50, 50);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(5);
    tft.println("ERROR!");
  }
}

void conBtn(){
              Serial.println("conpin");

        AnswerIsCon();
        showResult();
        drawed = true;
        answerSubmitted = true;
}

void isoBtn(){
              Serial.println("isopin");

        AnswerIsIso();
        showResult();
        drawed = true;
        answerSubmitted = true;
}

void goedBtn(){
              Serial.println("goedpin");

        AnswerIsGoed();
        showResult();
        drawed = true;
        answerSubmitted = true;
}


void setfunction(){
    for (int i = 0; i < rpsSize; i++)
    {
        if(rps[i] == currentSelectedOption){
          SET();
          }
    }
}

void confirmbtn(){
          if (strcmp(contOptions[currentOption], "Back") == 0 || strcmp(kortOptions[currentOption], "Back") == 0) {
          cont_submenu = false;
          kort_submenu = false;
          currentOption = 0;
          currentMenu = menuOptions[0];
          drawMenu();
        } else {
          currentOption = 0;
          if (strcmp(currentMenu, "Isolatiefout") == 0) {
            cont_submenu = false;
            kort_submenu = true;
                      drawMenu();

          } else if (strcmp(currentMenu, "Continuiteit") == 0) {
            kort_submenu = false;
            cont_submenu = true;
                      drawMenu();

          }
        }
}

void buttonpin(){
          if (kort_submenu) {
          currentOption = (currentOption + 1) % kortmenuSize;
        } else if (cont_submenu) {
          currentOption = (currentOption + 1) % contmenuSize;
        } else {
          currentOption = (currentOption + 1) % menuSize;
        }
        currentMenu = menuOptions[currentOption];

        drawMenu();
        while (digitalRead(BUTTON_PIN) == LOW);
}

ButtonHandler randomBtn(1,StuRandom);

ButtonHandler btnBtn(BUTTON_PIN,buttonpin);

ButtonHandler confBtn(CONFIRM_PIN,confirmbtn);

ButtonHandler setBtn(SET_PIN,setfunction);

void drawLoadingScreen() {
  tft.fillScreen(ILI9341_NAVY); // Background color
  tft.setTextColor(ILI9341_GREENYELLOW); // Text color
  tft.setTextSize(6);
  tft.setCursor(60, 100);
  tft.println("Fluvius");
  delay(2000); // Display the loading screen for 2 seconds
}

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
}

void loop() {
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
      delay(40); // Debounce delay
                  Serial.println("isopin");

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
            //delay(5000);
            //drawed = false;
            //setted = false;
            //answer = "";
            //answerSubmitted = false;
            //drawMenu();
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

void drawOption() {
  currentOption = 0;
  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(20, 20);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.println(currentMenu);

  if (strcmp(currentMenu, "Isolatiefout") == 0) {
    cont_submenu = false;
    kort_submenu = true;
  } else if (strcmp(currentMenu, "Continuiteit") == 0) {
    kort_submenu = false;
    cont_submenu = true;
  }
}
