
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

void drawLoadingScreen() {
  tft.fillScreen(ILI9341_NAVY); // Background color
  tft.setTextColor(ILI9341_GREENYELLOW); // Text color
  tft.setTextSize(6);
  tft.setCursor(60, 100);
  tft.println("Fluvius");
  delay(2000); // Display the loading screen for 2 seconds
}


ButtonHandler randomBtn(1,StuRandom);

ButtonHandler btnBtn(BUTTON_PIN,buttonpin);

ButtonHandler confBtn(CONFIRM_PIN,confirmbtn);

ButtonHandler setBtn(SET_PIN,setfunction);