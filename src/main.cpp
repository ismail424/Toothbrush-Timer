#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "pitches.h" 

Adafruit_ST7735 tft = Adafruit_ST7735(10, 8, 9); // CS, DC, RST

const int buttonPinIncrease = 2; // Button to increase timer
const int buttonPinStartPause = 3; // Button to start/pause timer

volatile int countdownTime = 30; // Start with a 30-second countdown
bool timerRunning = true; // To track timer state

const int buzzerPin = 4; // Piezo buzzer connected to pin 4

// Function prototypes
void increaseTime();
void toggleStartPause();
void displayTime();
void setAuthor();
void playMelody();
void setup(void) {
  pinMode(buttonPinIncrease, INPUT_PULLUP); // Enable internal pull-up
  pinMode(buttonPinStartPause, INPUT_PULLUP); // Enable internal pull-up
  pinMode(buzzerPin, OUTPUT); // Set the buzzer pin as output

  tft.initR(INITR_GREENTAB); // Initialize the display with the correct settings
  tft.fillScreen(ST7735_BLACK); // Clear the screen with a black background
  // Add author att bottom
  setAuthor(  );
  attachInterrupt(digitalPinToInterrupt(buttonPinIncrease), increaseTime, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPinStartPause), toggleStartPause, FALLING);
  
  displayTime(); // Display initial time
}

void loop() {
  static unsigned long lastMillis = 0;

  if (timerRunning && millis() - lastMillis >= 1000) { // Update every second
    lastMillis = millis();
    countdownTime = max(0, countdownTime - 1); // Decrease timer and ensure it's not negative
    displayTime();

    if (countdownTime == 0) {
      timerRunning = false; // Stop the timer when it reaches 0
      playMelody(); // Play a melody when the timer reaches 0
    }
  }
}

// Function to increase the timer by 30 seconds
void increaseTime() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis(); // Current time in milliseconds

  // If interrupts come faster than 200ms, assume it's bounce and ignore
  if (interruptTime - lastInterruptTime > 200) {
    countdownTime += 30; // Increase timer by 30 seconds
    displayTime();
  }
  lastInterruptTime = interruptTime; // Update the time of the last interrupt
}
void playMelody() {
  // Notes in the melody (in Hz). Example: C4, D4, E4...
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  // Note durations: 4 = quarter note, 8 = eighth note, etc.
  int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

  // Iterate over the notes of the melody
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    // Calculate the note duration
    int noteDuration = 1000 / noteDurations[thisNote];
    // Play the note
    tone(buzzerPin, melody[thisNote], noteDuration);
    // Pause between notes
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // Stop the tone playing
    noTone(buzzerPin);
  }
}

// Function to toggle the start/pause state of the timer
void toggleStartPause() {
  timerRunning = !timerRunning; // Toggle timer state
}

// Function to display the current timer value
// Function to display the current timer value with style changes
void displayTime() {
  int minutes = countdownTime / 60;
  int seconds = countdownTime % 60;

  // Determine the style based on the remaining time
  if (countdownTime > 15) {
    tft.fillRect(0, 0, tft.width(), tft.height()- 15, ST7735_BLACK); // Clear the screen
    tft.setTextColor(ST7735_GREEN); // Green text color
  } else {
    tft.fillRect(0, 0, tft.width(), tft.height()- 15, ST7735_BLUE); // Clear the screen
    tft.setTextColor(ST7735_WHITE); // White text for contrast
  }

  tft.setTextSize(3); // Increase text size to 3

  // Calculate the width of the text to center it
  String timeText = String(minutes < 10 ? "0" : "") + String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(timeText, 0, 0, &x1, &y1, &w, &h); // Get bounds of the text
  tft.setCursor((tft.width() - w) / 2, (tft.height() - h) / 2); // Center the text on the screen

  tft.print(timeText);
  setAuthor( );
}


void setAuthor( ) {
  tft.setCursor(15, 150);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(0.5);
  tft.println("By: Emin");
}
