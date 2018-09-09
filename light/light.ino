String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX

const int button_HIGH_pin = 2;
const int button_read_pin = 3;

#include <Servo.h>
int servo_degree = 0;
int servo_degree_old = 0;
Servo myservo;  // create servo object to control a servo
const int myservo_pin = 9;
const int myservo_power_pin = 8;

#include <EEPROM.h>
const int eeAddress = 0;   //Location we want the data to be put.

const int test_LED_power_pin = 5;
const int test_LED_ground_pin = 6;

const int IF_reciever_power_pin = 12;
const int IF_reciever_sign_pin = 13;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  Serial.println("hello");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  mySerial.println("Hello, world?");

  pinMode(button_HIGH_pin, OUTPUT);
  digitalWrite(button_HIGH_pin, LOW);
  pinMode(button_read_pin, INPUT_PULLUP);  // 上拉，普通状态为HIGH

  myservo.attach(myservo_pin);  // attaches the servo on pin 9 to the servo object

  EEPROM.get(eeAddress, servo_degree);
  servo_degree_old = servo_degree;

  pinMode(myservo_power_pin, OUTPUT);
  digitalWrite(myservo_power_pin, LOW);

  pinMode(test_LED_power_pin, OUTPUT);
  digitalWrite(test_LED_power_pin, LOW);
  pinMode(test_LED_ground_pin, OUTPUT);
  digitalWrite(test_LED_ground_pin, LOW);

  pinMode(IF_reciever_power_pin, OUTPUT);
  digitalWrite(IF_reciever_power_pin, HIGH);
  pinMode(IF_reciever_sign_pin, INPUT_PULLUP);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }

  if (LOW == digitalRead(button_read_pin)) {
    ++servo_degree;
  }
  else {
    --servo_degree;                          // waits for the servo to get there
  }
  if (160 < servo_degree) {                 // 死区问题
    servo_degree = 160;
  } else if (20 > servo_degree) {
    servo_degree = 20;
  }
  if (servo_degree_old != servo_degree) {
    digitalWrite(myservo_power_pin, HIGH);
    Serial.println(servo_degree);
    myservo.write(servo_degree);                  // sets the servo position according to the scaled value
    EEPROM.put(eeAddress, servo_degree);
    servo_degree_old = servo_degree;
  }
  delay(80);
  digitalWrite(myservo_power_pin, LOW);

  if (LOW == digitalRead(IF_reciever_sign_pin)) {
    digitalWrite(test_LED_power_pin, HIGH);
    delay(1000);
  } else {
    digitalWrite(test_LED_power_pin, LOW);
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      // add it to the inputString:
      inputString += inChar;
    }
  }
}
