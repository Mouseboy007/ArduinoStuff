// -------------------------------------------------------------------------------------------------------------------------------------
// Atari Dual Paddle's to USB Mouse
//
// This sketch enables you to take an original Dual-Paddle Atari controller that terminates to a regular 9-Pin connector
// A small (reversible) modification needs to be performed on each Paddle which is to connect the unused potentiometer terminal to the (black) ground cable
// used by the fire button
//
// This sketch will work in Windows Emulators and also is compatible with the DE10 Nano (MiSTer FPGA) board.
// I suspect it will work with Linux, Mac, and Raspberry Pi's (and more) but haven't tested. However if your device can read a USB mouse, 
// it can probably use this paddle sketch and modification
// 
// I encourage (and challenge!) anyone who is vastly more capable than me to make improvements to this sketch
// I don't know the tolerances of the potentiometers used by Atari in the paddles and whether for your individual paddles you will need to make small
// tweaks to the sketch anyway. However, I've tried to make the sketch as verbose and descriptive as possible to make it clear to even the most novice user
// what each of the values do to enable anyone to tweak as required.
// 
// Arduino Sketch by Andy Pyne 15th July 2020 - If improve on this, please do let me know! 
// I'll be posting a video to my YouTube channel MouseboyMania in which I'll demonstrate the process of uploading the sketch and (reversibly) 
// modifying an original Atari 2600 paddle-pair.
//
// I hope this is of help, and I'd also like to say thanks to:
// - James Roberts (for his original advice and support regards using an Arduino to get retro controllers working via USB) https://github.com/Bastich-666/Multi-Joy
// - Lord Vek (I borrowed bits of your code that I came across via Googling) https://www.instructables.com/member/Lord_Vek/
// 
// -------------------------------------------------------------------------------------------------------------------------------------
//  MIT License
//  
//  Copyright (c) [year] [fullname]
//  
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//  
// -------------------------------------------------------------------------------------------------------------------------------------

#include <Mouse.h>

int PercentDivider = 1000;                   // 100 would be 1%, 1000 would be 0.1% etc
                                             // The bigger the number the smaller the 'fudge' in terms of adding or removing a little extra to the position-value

const int NumberOfReadings= 110;             // How many readings we make of a controller's position to then use in calculating an average to smooth out jitter
                                             // The bigger this number, the more readings are performed of the controller value before any movement is made
                                             // In other words, bigger numbers will add more smoothing to the movement but at the expense of added latency

int InputPaddle1[NumberOfReadings];          // Values read from (Analogue Input) Paddle1
int InputPaddle2[NumberOfReadings];          // Values read from (Analogue Input) Paddle2

int IndexOfCurrentReading = 0;               // As per variable, the Index of the Current Reading

int RunningTotalPaddle1 = 0;                 // The running total number of readings made for Paddle1
int RunningTotalPaddle2 = 0;                 // The running total number of readings made for Paddle2

int AverageValueForPaddle1 = 0;              // As per the variable, the average value calculated for Paddle 1
int AverageValueForPaddle2 = 0;              // As per the variable, the average value calculated for Paddle 2

int PreviousValuePaddle1 = 0;                // Values previously received/calculated for Paddle 1
int PreviousValuePaddle2 = 0;                // Values previously received/calculated for Paddle 2

int CursorSpeedHorizontal = 4;               // How fast the cursor will travel in the Horizontal Axis
int CursorSpeedVertical = 4;                 // How fast the cursor will travel in the Vertical Axis

int ArduinoPinForPaddle1Horizontal = A2;     // The Pin on the Arduino to which we have Paddle1 connected
int ArduinoPinForPaddle2Vertical = A0;       // The Pin on the Arduino to which we have Paddle2 connected
int ArduinoPinForPaddle1Fire = 10;           // The Pin on the Arduino to which we have Paddle1's Fire Button connected
int ArduinoPinForPaddle2Fire = 14;           // The Pin on the Arduino to which we have Paddle1's Fire Button connected

int RegisterMouseLeftClick = 0;              // Setting the Left Mouse Click as zero/null (untill we press it)
int RegisterMouseRightClick = 0;             // Setting the Right Mouse Click as zero/null (untill we press it)

void setup()
{
  Mouse.begin();
  pinMode(ArduinoPinForPaddle1Horizontal, INPUT_PULLUP);    // Sets Paddle1's Movement Axis as the Horizontal Analogue Input
  pinMode(ArduinoPinForPaddle2Vertical, INPUT_PULLUP);      // Sets Paddle2's Movement Axis as the Vertical Analogue Input
  pinMode(ArduinoPinForPaddle1Fire, INPUT);                 // Sets Paddle1's Fire as an Analogue Input
  pinMode(ArduinoPinForPaddle2Fire, INPUT);                 // Sets Paddle2's Fire as an Analogue Input
  digitalWrite(ArduinoPinForPaddle1Fire, HIGH);             // Sets Paddle1's Fire to High
  digitalWrite(ArduinoPinForPaddle2Fire, HIGH);             // Sets Paddle2's Fire to High
  delay(30);                                                // A short delay to let the outputs settle down
}

void loop()
{

RunningTotalPaddle1 = RunningTotalPaddle1 - InputPaddle1[IndexOfCurrentReading];    // Set a Running Total value for Paddle1
RunningTotalPaddle2 = RunningTotalPaddle2 - InputPaddle2[IndexOfCurrentReading];    // Set a Running Total value for Paddle2

InputPaddle1[IndexOfCurrentReading] = analogRead(ArduinoPinForPaddle1Horizontal);   // Set the current value for Paddle1 as the value that's read from the the Horizontal Axis
InputPaddle2[IndexOfCurrentReading] = analogRead(ArduinoPinForPaddle2Vertical);     // Set the current value for Paddle2 as the value that's read from the the Vertical Axis

RunningTotalPaddle1 = RunningTotalPaddle1 + InputPaddle1[IndexOfCurrentReading];    // Add the current value for Paddle1 to the Paddle1 Running Total
RunningTotalPaddle2 = RunningTotalPaddle2 + InputPaddle2[IndexOfCurrentReading];    // Add the current value for Paddle1 to the Paddle2 Running Total

IndexOfCurrentReading = IndexOfCurrentReading + 1;                                  // Add +1 to the Index so that the next loop a new Index is taken for the Paddle values

if (IndexOfCurrentReading >= NumberOfReadings) {                                    // If we've gotten to the end of the loop (because we're performed all of the readings,
    IndexOfCurrentReading = 0;                                                      // then we set the Reading value back to zero so we can capture the next set of readings
  }

AverageValueForPaddle1 = RunningTotalPaddle1 / NumberOfReadings;                    // We can now calculate an average value for Paddle1's recent inputs
AverageValueForPaddle2 = RunningTotalPaddle2 / NumberOfReadings;                    // We can now calculate an average value for Paddle2's recent inputs                    
delay(1);                                                                           // A short delay between reads to add further stability/smoothing
   

  // We check to see if the previous Paddle1 value (minus a small percent) is bigger than the average movement value we calculated
  // If it is, we then move the cursor in the speed-steps we declared earlier in the X,Y axis heading to the right
  if ((PreviousValuePaddle1 - PreviousValuePaddle1 / PercentDivider) > AverageValueForPaddle1) {
    Mouse.move(-CursorSpeedHorizontal, 0, 0);
  }
  
  // We check to see if the previous Paddle1 value (plus a small percent) is smaller than the average movement value we calculated
  // If it is, we then move the cursor in the speed-steps we declared earlier in the X,Y axis heading to the left
  if ((PreviousValuePaddle1 + PreviousValuePaddle1 / PercentDivider) < AverageValueForPaddle1) {
    Mouse.move(CursorSpeedHorizontal, 0, 0);
  }

  // We check to see if the previous Paddle2 value (minus a small percent) is bigger than the average movement value we calculated
  // If it is, we then move the cursor in the speed-steps we declared earlier in the X,Y axis heading up
  if ((PreviousValuePaddle2 - PreviousValuePaddle2 / PercentDivider) > AverageValueForPaddle2) {
    Mouse.move(0, CursorSpeedVertical, 0);
  }

// We check to see if the previous Paddle2 value (plus a small percent) is bigger than the average movement value we calculated
  // If it is, we then move the cursor in the speed-steps we declared earlier in the X,Y axis heading down
  if ((PreviousValuePaddle2 + PreviousValuePaddle2 / PercentDivider) < AverageValueForPaddle2) {
    Mouse.move(0, -CursorSpeedVertical, 0);
}

  if ((digitalRead(ArduinoPinForPaddle1Fire) == 0) && (!RegisterMouseLeftClick))          // If Paddle1's Fire Button is pressed, we enact a Left Mouse Click
  { RegisterMouseLeftClick = 1;
    Mouse.press(MOUSE_LEFT);
  }
  
  else if ((digitalRead(ArduinoPinForPaddle1Fire)) && (RegisterMouseLeftClick))           // And if Paddle1's Fire Button is not pressed, we release the Left Mouse button
  { RegisterMouseLeftClick = 0;
    Mouse.release(MOUSE_LEFT);
  }

  if ((digitalRead(ArduinoPinForPaddle2Fire) == 0) && (!RegisterMouseRightClick))          // If Paddle2's Fire Button is pressed, we enact a Right Mouse Click
  { RegisterMouseRightClick = 1;
    Mouse.press(MOUSE_RIGHT);
  }
  else if ((digitalRead(ArduinoPinForPaddle2Fire)) && (RegisterMouseRightClick))           // And if Paddle2's Fire Button is not pressed, we release the Right Mouse button
  { RegisterMouseRightClick = 0;
    Mouse.release(MOUSE_RIGHT);
  }

  PreviousValuePaddle1 = AverageValueForPaddle1;                  // We set the previous value for Paddle1 to be the calculated average before restarting the loop again
  PreviousValuePaddle2 = AverageValueForPaddle2;                  // We set the previous value for Paddle2 to be the calculated average before restarting the loop again

}
