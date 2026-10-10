#include <LiquidCrystal.h>

// Initialize the LCD
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// Pin definitions
const int POT_PIN = A0;   // Potentiometer to simulate Moisture Sensor
const int MOTOR_PIN = 7;  // Relay Control
const int BUZZER_PIN = 4; // Buzzer Pin
const int BUTTON_PIN = 6; // Pushbutton Pin

// Timing constants
const unsigned long SHORT_PRESS_TIME = 200;
const unsigned long LONG_PRESS_TIME = 2000;
const unsigned long INITIAL_RUNTIME = 5000;
const unsigned long DAILY_RUNTIME = 2000; // Watering duration set to 2 sec
const unsigned long DAY_DURATION = 10000; // 1 day = 10 sec (Simulated)
const unsigned long TWO_DAYS = 2 * DAY_DURATION; // 2 days = 20 sec (Simulated)

// Plant profiles
struct Plant {
  const char *name;
  int criticalLow;
  int normalHigh;
  int criticalHigh;
};

Plant plants[] = {
    {"Cactus", 10, 20, 40},
    {"AloeVera", 10, 30, 50},
    {"ZamioculcasZamifolia", 15, 40, 70}
};

int selectedPlant = 0;
bool plantSelected = false;
bool calibrationDone = false;

// Global variables for motor and buzzer logic
unsigned long motorStartTime = 0;
unsigned long lastDailyRun = 0;
bool motorActive = false;
bool emergencyMode = false;
bool criticalHighTriggered = false;
unsigned long lastBeepTime = 0;
bool buzzerState = false;

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(POT_PIN, INPUT);

  digitalWrite(MOTOR_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.begin(20, 4);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("   AutWat System   ");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Select Plant:");
  while (!plantSelected) {
    lcd.setCursor(0, 1);
    lcd.print(plants[selectedPlant].name);
    lcd.print("        ");
    handleButtonInput();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Confirmed: ");
  lcd.setCursor(0, 1);
  lcd.print(plants[selectedPlant].name);
  delay(2000);
  lcd.clear();

  // **Start Initial Motor Run for Calibration**
  motorActive = true;
  motorStartTime = millis();
  digitalWrite(MOTOR_PIN, HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Iniital Motor ON");

  delay(INITIAL_RUNTIME);

  motorActive = false;
  calibrationDone = true;
  digitalWrite(MOTOR_PIN, LOW);
  lcd.setCursor(0, 1);
  lcd.print("Ready!      ");
  delay(2000);
  lcd.clear();

  lastDailyRun = millis(); // Reset daily watering timer after calibration
}

void loop() {
  if (!calibrationDone) return;

  unsigned long currentTime = millis();

  int potValue = analogRead(POT_PIN);
  int moisture = map(potValue, 0, 1019, 0, 100);

  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(moisture);
  lcd.print("%   ");

  // **Critical High Moisture Handling**


  if (moisture > plants[selectedPlant].criticalHigh) {
    criticalHighTriggered = true;
    lcd.setCursor(0, 1);
    lcd.print("Critical High!     ");
    handleBuzzer(currentTime, 200);
    motorActive = false;
    digitalWrite(MOTOR_PIN, LOW);
    return;
  }

  if (criticalHighTriggered && moisture <= plants[selectedPlant].normalHigh) {
    criticalHighTriggered = false;
    lcd.clear();
  }

  // **Emergency Watering (Critical Low Moisture)**


  if (moisture < plants[selectedPlant].criticalLow) {
    if (!motorActive) {
      motorActive = true;
      emergencyMode = true;
      motorStartTime = currentTime;
      digitalWrite(MOTOR_PIN, HIGH);
    }
    lcd.setCursor(0, 1);
    lcd.print("Critical Low!      ");
    handleBuzzer(currentTime, 200);


  } else {
    stopBuzzer();
  }

  if (emergencyMode && moisture >= plants[selectedPlant].normalHigh) {
    motorActive = false;
    emergencyMode = false;
    digitalWrite(MOTOR_PIN, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Motor: OFF         ");
  }

  // **Scheduled Watering Every 2 Days (Simulated as 20 Seconds)**
  if (!emergencyMode && !motorActive) {
    if (currentTime - lastDailyRun >= TWO_DAYS) {
      motorActive = true;
      motorStartTime = currentTime;
      digitalWrite(MOTOR_PIN, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Watering in Prog  ");
      handleBuzzer(currentTime, 1000);
    }
  }

  // **Stop Motor After Daily Runtime**
  if (motorActive && !emergencyMode && currentTime - motorStartTime >= DAILY_RUNTIME) {
    motorActive = false;
    digitalWrite(MOTOR_PIN, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Motor: OFF         ");
    lastDailyRun = millis();
  }

  delay(100);
}

void handleButtonInput() {
  static unsigned long buttonPressStart = 0;
  static bool buttonHeld = false;

  if (digitalRead(BUTTON_PIN) == LOW) {
    if (buttonPressStart == 0) {
      buttonPressStart = millis();
    } else if (!buttonHeld && (millis() - buttonPressStart > LONG_PRESS_TIME)) {
      plantSelected = true;
      buttonHeld = true;
    }
  } else {
    if (buttonPressStart > 0) {
      unsigned long pressDuration = millis() - buttonPressStart;
      if (pressDuration >= SHORT_PRESS_TIME && pressDuration < LONG_PRESS_TIME && !plantSelected) {
        selectedPlant = (selectedPlant + 1) % (sizeof(plants) / sizeof(plants[0]));
      }
      buttonPressStart = 0;
      buttonHeld = false;
    }
  }
}

void handleBuzzer(unsigned long currentTime, unsigned long interval) {
  if (currentTime - lastBeepTime >= interval) {
    buzzerState = !buzzerState;
    digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
    lastBeepTime = currentTime;
  }
}

void stopBuzzer() {
  digitalWrite(BUZZER_PIN, LOW);
  buzzerState = false;
}
