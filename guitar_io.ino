#include <Joystick.h>

Joystick_ Joystick = Joystick_(
  0x03,                    // reportid
  JOYSTICK_TYPE_GAMEPAD,   // type
  5,                       // button count
  0,                       // hat switch count
  false,                    // x axis enable
  false,                    // y axis enable
  false,                   // z axis enable
  false,                   // right x axis enable
  false,                   // right y axis enable
  false,                   // right z axis enable
  false,                   // rudder enable
  false,                   // throttle enable
  false,                   // accelerator enable
  false,                   // brake enable
  false                    // steering enable
  );

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);

  Joystick.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  Joystick.setButton(0, !digitalRead(3));
  Joystick.setButton(1, !digitalRead(4));
  Joystick.setButton(2, !digitalRead(5));
  Joystick.setButton(3, !digitalRead(11));
  Joystick.setButton(4, !digitalRead(9));

}
