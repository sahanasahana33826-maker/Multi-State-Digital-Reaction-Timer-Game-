#include <LiquidCrystal.h>

// --------------------------------------------------
// LCD CONNECTIONS
// RS = D12
// E  = D11
// D4 = D5
// D5 = D4
// D6 = D3
// D7 = D2
// --------------------------------------------------

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// --------------------------------------------------
// PUSH BUTTONS
// --------------------------------------------------

const int START_BUTTON = 7;
const int REACTION_BUTTON = 8;
const int RESET_BUTTON = 9;

// --------------------------------------------------
// RGB LED
// --------------------------------------------------

const int RED_LED = 10;
const int GREEN_LED = A0;
const int BLUE_LED = A1;

// --------------------------------------------------
// GAME VARIABLES
// --------------------------------------------------

unsigned long reactionStartTime = 0;
unsigned long reactionTime = 0;

unsigned long bestTime = 0;

int roundNumber = 0;

enum GameState
{
  READY,
  WAITING,
  REACTION,
  RESULT,
  EARLY
};

GameState state = READY;

// --------------------------------------------------
// RGB LED FUNCTIONS
// --------------------------------------------------

void red()
{
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
}

void green()
{
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);
}

void blue()
{
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
}

void off()
{
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
}

// --------------------------------------------------
// DISPLAY READY SCREEN
// --------------------------------------------------

void showReady()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("REACTION GAME");

  lcd.setCursor(0, 1);
  lcd.print("Press START");

  green();
}

// --------------------------------------------------
// SETUP
// --------------------------------------------------

void setup()
{
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(REACTION_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  lcd.begin(16, 2);

  randomSeed(analogRead(A5));

  showReady();
}

// --------------------------------------------------
// WAIT FOR BUTTON RELEASE
// --------------------------------------------------

void waitForRelease(int buttonPin)
{
  while (digitalRead(buttonPin) == LOW)
  {
    delay(10);
  }
}

// --------------------------------------------------
// MAIN PROGRAM
// --------------------------------------------------

void loop()
{

  // ------------------------------------------------
  // RESET / NEW GAME BUTTON
  // ------------------------------------------------

  if (digitalRead(RESET_BUTTON) == LOW)
  {
    roundNumber = 0;
    bestTime = 0;

    showReady();

    waitForRelease(RESET_BUTTON);

    state = READY;

    delay(300);

    return;
  }


  // =================================================
  // READY STATE
  // =================================================

  if (state == READY)
  {

    if (digitalRead(START_BUTTON) == LOW)
    {
      waitForRelease(START_BUTTON);

      roundNumber++;

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("WAIT...");

      lcd.setCursor(0, 1);
      lcd.print("Don't press!");

      blue();

      state = WAITING;

      // Random waiting time:
      // 2000 ms to 5000 ms

      unsigned long waitingTime = random(2000, 5001);

      unsigned long waitStart = millis();

      // ----------------------------------------------
      // WAITING STATE
      // ----------------------------------------------

      while (millis() - waitStart < waitingTime)
      {

        // Check for early button press

        if (digitalRead(REACTION_BUTTON) == LOW)
        {
          waitForRelease(REACTION_BUTTON);

          lcd.clear();

          lcd.setCursor(0, 0);
          lcd.print("TOO EARLY!");

          lcd.setCursor(0, 1);
          lcd.print("Try again");

          red();

          state = EARLY;

          delay(2000);

          showReady();

          state = READY;

          return;
        }

        // Allow reset during waiting

        if (digitalRead(RESET_BUTTON) == LOW)
        {
          roundNumber = 0;
          bestTime = 0;

          waitForRelease(RESET_BUTTON);

          showReady();

          state = READY;

          return;
        }
      }


      // =================================================
      // REACTION STATE
      // =================================================

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("GO!!!");

      lcd.setCursor(0, 1);
      lcd.print("PRESS NOW");

      red();

      // IMPORTANT:
      // Start timing exactly when GO appears.

      reactionStartTime = millis();

      state = REACTION;


      // ----------------------------------------------
      // WAIT FOR REACTION BUTTON
      // ----------------------------------------------

      while (digitalRead(REACTION_BUTTON) == HIGH)
      {

        // Reset button can still restart game

        if (digitalRead(RESET_BUTTON) == LOW)
        {
          roundNumber = 0;
          bestTime = 0;

          waitForRelease(RESET_BUTTON);

          showReady();

          state = READY;

          return;
        }
      }


      // Calculate reaction time in milliseconds

      reactionTime = millis() - reactionStartTime;

      waitForRelease(REACTION_BUTTON);


      // =================================================
      // RESULT STATE
      // =================================================

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Reaction:");

      lcd.setCursor(10, 0);
      lcd.print(reactionTime);
      lcd.print("ms");

      // Determine best time

      if (bestTime == 0 || reactionTime < bestTime)
      {
        bestTime = reactionTime;
      }

      lcd.setCursor(0, 1);
      lcd.print("Best:");
      lcd.print(bestTime);
      lcd.print("ms");

      green();

      state = RESULT;

      delay(3000);


      // Prepare next round

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Round ");
      lcd.print(roundNumber + 1);

      lcd.setCursor(0, 1);
      lcd.print("Press START");

      green();

      state = READY;
    }
  }
}
