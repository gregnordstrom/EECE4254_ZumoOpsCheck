/**************************************************************************
 * Test program for the Zumo Robot for Arduino, v 1.2. This program tests
 * the Zumo's motors, line sensors, and buzzer, and is intended to work
 * on a Sumo ring playfield that has a WHITE BACKGROUND and BLACK LINES.
 * Note that you may have to adjust the QTR_THRESHOLD depending on lighting
 * conditions, but a value of 900 seems to work well in the lab.
 * 
 * Once the program is loaded, turn on the Zumo and check that the red and
 * blue rear LED running lights are on, as well as the yellow/orange LED
 * near the right rear running lights. Next, place the Zumo inside the
 * Sumo ring and press the pushbutton switch near the On/Off switch. The
 * yellow/orange LED should immediately go out, and after a short pause
 * four beep tones should be heard. Immediately after the fourth tone,
 * the Zumo should begin moving, heading towards the edge of the ring.
 * Once it crosses onto the black outer ring it should immediately back
 * up, turn, and head towards another part of the ring. Pressing the
 * pushbutton while the Zumo is moving will immediately stop the Zumo
 * and re-light the yellow/orange LED, indicating that Zumo is waiting
 * for a pushbutton press to repeat the tones and begin moving again.
 **************************************************************************/

#include <Wire.h>
#include <ZumoShield.h>

#define LED 13

// this might need to be tuned for different lighting conditions, surfaces, etc.
//#define QTR_THRESHOLD  1500 // microseconds (default)
#define QTR_THRESHOLD  900 // microseconds

// these might need to be tuned for different motor types
#define REVERSE_SPEED     200 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     200
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     300 // ms

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];

ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

void waitForButtonAndCountDown()
{
  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);

  // play audible countdown
  for (int i = 0; i < 3; i++)
  {
    delay(700);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }
  delay(700);
  buzzer.playNote(NOTE_G(4), 500, 15);
  delay(700);
}

void setup()
{
  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  pinMode(LED, HIGH);

  waitForButtonAndCountDown();
}

void loop()
{
  if (button.isPressed())
  {
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    button.waitForRelease();
    waitForButtonAndCountDown();
  }


  sensors.read(sensor_values);

//  if (sensor_values[0] < QTR_THRESHOLD) // detect white
  if (sensor_values[0] > QTR_THRESHOLD) // detect black
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
//  else if (sensor_values[5] < QTR_THRESHOLD) // detect white
  else if (sensor_values[5] > QTR_THRESHOLD) // detect black
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // otherwise, go straight
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
}
