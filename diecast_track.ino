// required library for servo operations
#include <Servo.h>


// servo
Servo myServo;              // servo object
int servoPin = A1;          // servo read pin
int servoPos = 0;           // servo position


// LED lights
int LEDRed1 = A2;
int LEDRed2 = A3;
int LEDRed3 = A4;
int LEDGreen = A5;


// button
int buttonPin = 0;          // button input pin
bool buttonPressed = false; // used so button press only occurs 1 time


// 4 digit 7 segment display
int pinA = 12;              // pins for each segment
int pinB = 8; 
int pinC = 4;  
int pinD = 6;               
int pinE = 7;  
int pinF = 11;
int pinG = 3;
int pinDot = 5;

int D1 = 13;                // pins for each digit
int D2 = 10;   
int D3 = 9;    
int D4 = 2;    

bool raceStarted = false;   // identifies if timer is running

unsigned long startMillis;  // time in mil seconds when timer starts 
unsigned long timePassed;   // current time. used to calculate time that has passed

int tens = 0;               // storage locations for the 4 digits of the display
int ones = 0;
int tenths = 0;
int hundredths = 0;


// IR sensor
int IRPin = A0;             // analog pin to read IR value (0 - 1000)
int IRRead;                 // stores the read value from sensor
int IRTrip = 700;            // value we want our sensor to trip the timer


// this runs when script is started
void setup() 
{
  // allows for output to screen
  Serial.begin(9600);

  // SERVO
  myServo.attach(servoPin);
  OpenStartgate();
  
  // LEDS
  pinMode(LEDRed1, OUTPUT);
  pinMode(LEDRed2, OUTPUT);
  pinMode(LEDRed3, OUTPUT);
  pinMode(LEDGreen, OUTPUT);

  // BUTTON
  pinMode(buttonPin, INPUT);

  // DISPLAY
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT);
  pinMode(pinF, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinDot, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  // SENSOR
  pinMode(IRPin, INPUT);
}


// main loop. this function continues to run over and over
void loop() 
{
  ButtonPressed();  

  // race is either started and the timer is running or not
  // if started, we update the timer and check the sensor
  // if race is over we idle the timer to display the end time
  if (raceStarted)
  {
    RunTimer();
    ReadIRSensor();
  }
  else
  {
    IdleTimer();
  }
}


// checks if button has been pressed and handles what to do
void ButtonPressed()
{
  // read button value is either a 0 or 1
  bool buttonRead = digitalRead(buttonPin);

  if (buttonRead == 0) // button is pressed
  {
    // if buttonPressed is true, then we already pressed the button. Skip code
    // once button is released, this value resets
    if (!buttonPressed)
    {     
      buttonPressed = true;

      // if the race is going and we press button, then reset timer and servo
      // otherwise we want to begin race and start the light sequence
      if (raceStarted)
      {
        PrintNum(0,0);
        Reset();
      }
      else
      {
        raceStarted = true;
        LightSequence();
      }
    }
  }
  else  // button is not pressed
  {
    // if buttonPressed is set, then we have released the button
    // otherwise skip code
    if(buttonPressed)
    {
      buttonPressed = false;
    }
  }
}


// Beginning light sequence, once complete open start gate
void LightSequence()
{
  PrintNum(0,0);  
  digitalWrite(LEDRed1, HIGH);
  delay(3000);
  digitalWrite(LEDRed2, HIGH);
  delay(400);
  digitalWrite(LEDRed3, HIGH);
  delay(400);
  digitalWrite(LEDGreen, HIGH);

  OpenStartgate();

  // slight delay for start gate to begin opening
  delay(200); 

  // set our start time for timer
  startMillis = millis();
}


// If race is over, just idel the timer at current value
void IdleTimer()
{
  PrintNum(D1, tens);
  PrintNum(D2, ones);
  PrintNum(D3, tenths);
  PrintNum(D4, hundredths);  
}


// run our timer and calculuate the time that has passed
// here we calculate what each digit should be on timer
void RunTimer()
{
  // calculate how much time has passed since startMillis
  timePassed = millis() - startMillis;

  // tens digit. 10000 mil seconds is 10 seconds
  tens = timePassed / 10000;
  // we get the remaider here to pass to next digit
  int leftOvers = timePassed % 10000;

  // out of leftovers each 1000  mil seconds is a second
  ones = leftOvers / 1000;
  leftOvers = leftOvers % 1000;

  // out of leftovers each 100 mil seconds is a tenth of a second
  tenths = leftOvers / 100;
  leftOvers = leftOvers % 100;

  // out of leftovers, you get the picture :)
  hundredths = leftOvers / 10;

  // print out our numbers on the display
  PrintNum(D1, tens);
  PrintNum(D2, ones);
  PrintNum(D3, tenths);
  PrintNum(D4, hundredths);
}


// this reads our sensor and if we hit our trip value we stop the timer
void ReadIRSensor()
{
  // read value (0 - 1000)
  IRRead = analogRead(IRPin);

  // print out value (for testing)
  Serial.println(IRRead);

  if (IRRead > IRTrip)
  {
    Reset();
  }
}


// opens start gate
void OpenStartgate()
{
  // if raceStarted is true, then set to 90 which will open the start gate
  // if not, then reset and open start gate
  if (raceStarted)
  {
    myServo.write(90);
  }
  else
  {
    myServo.write(0);
  }
}

// reset LEDs and opens start gate
void Reset()
{
  digitalWrite(LEDRed1, LOW);
  digitalWrite(LEDRed2, LOW);
  digitalWrite(LEDRed3, LOW);
  digitalWrite(LEDGreen, LOW);

  raceStarted = false;
  
  OpenStartgate();
}


//////////
// LED DISPLAY
//////////

// main function for printing number to display
void PrintNum(int digit, int displayNumber)
{
  // for my display you set all digits to HIGH except for the digit
  // you want to set. Any digit set to LOW will display the displayNumber
  switch (digit)
  {
    case 13: // digit 1
      digitalWrite(D1, LOW);
      digitalWrite(D2, HIGH);
      digitalWrite(D3, HIGH);
      digitalWrite(D4, HIGH);
      break;
    case 10: // digit 2
      digitalWrite(D1, HIGH);
      digitalWrite(D2, LOW);
      digitalWrite(D3, HIGH);
      digitalWrite(D4, HIGH);
      break;
    case 9: // digit 3
      digitalWrite(D1, HIGH);
      digitalWrite(D2, HIGH);
      digitalWrite(D3, LOW);
      digitalWrite(D4, HIGH);
      break;
    case 2: // digit 4
      digitalWrite(D1, HIGH);
      digitalWrite(D2, HIGH);
      digitalWrite(D3, HIGH);
      digitalWrite(D4, LOW);
      break;
    default:
      digitalWrite(D1, LOW);
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
      digitalWrite(D4, LOW);
      break;
  }

  // 10 is the pin for Digit 2. I want the dot to always be displayed
  // for the decimal point
  if (digit == 10)
  {
    digitalWrite(pinDot, HIGH);
  }
  else
  {
    digitalWrite(pinDot, LOW);
  }

  // here we set the segments to display the number we want
  // remember only the digits with LOW set will display the segments
  switch (displayNumber)
  {
    case 0:
      Print0();
      break;
    case 1:
      Print1();
      break;
    case 2:
      Print2();
      break;
    case 3:
      Print3();
      break;
    case 4:
      Print4();
      break;
    case 5:
      Print5();
      break;
    case 6:
      Print6();
      break;
    case 7:
      Print7();
      break;
    case 8:
      Print8();
      break;
    case 9:
      Print9();
      break;
    default:
      break;
  }

  // here we need a small delay when we switch between digits
  // we are esssentially mutliplexing the digits
  delay(3);
}


// functions to print each digit 0 - 9
// Here we set HIGH for the segments that make up each number
void Print0()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, LOW);
}


void Print1()
{
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, LOW);
}


void Print2()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, HIGH);
}


void Print3()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, HIGH);
}


void Print4()
{
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}


void Print5()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}


void Print6()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}


void Print7()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, LOW);
}


void Print8()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}


void Print9()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}
