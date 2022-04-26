#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_PHONE_SHIELD
#define INCLUDE_ACCELEROMETER_SENSOR_SHIELD
#define INCLUDE_TWITTER_SHIELD
#define INCLUDE_GPS_SHIELD

/* Include 1Sheeld library. */
#include <OneSheeld.h>

#define trigPin1 9                                   //pin number 9 in arduino Uno ATMEGA328p
#define echoPin1 12                              // we'll use this pin to read the signal from the first sensor
#define LED_first_ping 6                            // I/O digital or analogique ( we will use pin 22 to command an LED (on/off))
#define buzzer_IR 5
//----------------------------------------------------------------------------------------------------------------------

//define the pins that we will use for the second ultrasonic sensor
//----------------------------------------------------------------------------------------------------------------------
#define trigPin2 10
#define echoPin2 11
#define buzzer_u 4

// IR_sensor pins
#define IR_sensor 13
#define vib 2
#define waterPin A3
#define button 8

boolean verify1 = false;
boolean verify2 = false;
int distance = 0;
int del;
//----------------------------------------------------------------------------------------------------------------------

void setup() {
  OneSheeld.begin();
  //setup pins first sensor
  pinMode(trigPin1, OUTPUT);                        // from where we will transmit the ultrasonic wave
  pinMode(echoPin1, INPUT);                         //from where we will read the reflected wave

  //setup pins second sensor
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // Setup pins on PORTD
  // IR_sensor as input and the other one as output
  // then,pins 0, 1, 3 as input and 2, 4, 5, 6 as output

  DDRD = B11110100;

}

void loop() {
  //Serial.print("welcom in the Loop :)\n");
  wassersensor();
  notruf();
  giroscope();

  int ultra1 = calculateDistance(echoPin1, trigPin1, buzzer_u);
  int ultra2 = calculateDistance(echoPin2, trigPin2, buzzer_IR);
  int ir_reagiert = infrarot_sensor();

  //  Serial.print("valeur du ultraschall1:");
  //  Serial.println( ultra1);
  //  Serial.print("valeur du ultraschall2:");
  //  Serial.println(ultra2);

  if (ultra1 < 100 && ultra1 > 0) {
    PORTD = PORTD | (1 << 4); // 0x00010000
    verify1 = true;
    delay(ultra1 * 10);
  }
  if (ultra2 < 100 && ultra2 > 0) {

    PORTD = PORTD | (1 << 5); //0x00100000
    verify2 = true;
    delay(ultra2 * 10);
  }
  // prüfen ob beide Ultraschallsensoren erkennen
  if (verify1 & verify2) {
    if (ultra1 > ultra2) {
      del = ultra1;
    }
    else {
      del = ultra2;
    }
    PORTD = PORTD | (1 << 4);
    PORTD = PORTD | (1 << 5);
    delay(del);

  }

  if (ir_reagiert) {

    PORTD = PORTD | (1 << 2);
    delay(1000);
    PORTD = PORTD & ~(1 << 2);
    delay(1000);
  }
  //  if( (ultra1 & ultra2) & ir_reagiert){
  //    PORTD = PORTD | (1 << 4); // 0x00010000
  //    PORTD = PORTD | (1 << 5);
  //    PORTD = PORTD | (1 << 2);
  //    delay(distance *10);
  //    PORTD = PORTD & ~(1 << 4);
  //    PORTD = PORTD & ~(1 << 5);
  //    PORTD = PORTD & ~(1 << 2);
  //    delay(distance * 10);
  //  }
  //
  else {
    PORTD = PORTD & ~(1 << 4);
    PORTD = PORTD & ~(1 << 5);
    PORTD = PORTD & ~(1 << 2);
  }

  verify1 = false;
  verify2 = false;
  //distance = 0;
}

int calculateDistance(int echoPinSensor, int trigPinSensor, int buzzer) {

  long duration;
  digitalWrite(trigPinSensor, LOW);// put trigpin LOW
  delayMicroseconds(2);// wait 2 microseconds
  digitalWrite(trigPinSensor, HIGH);// switch trigpin HIGH
  delayMicroseconds(10); // wait 10 microseconds
  digitalWrite(trigPinSensor, LOW);// turn it LOW again
  //----------------------------------------------------------------------------------------------------------------------

  //read the distance
  //----------------------------------------------------------------------------------------------------------------------
  duration = pulseIn(echoPinSensor, HIGH);//pulseIn funtion will return the time on how much the configured pin remain the level HIGH or LOW; in this case it will return how much time echoPinSensor stay HIGH
  distance = (duration / 2) / 29.1; // first we have to divide the duration by two
  return distance;
}

int infrarot_sensor() {
  int zustand = 0;
  int status_sensor = digitalRead(IR_sensor);
  if (status_sensor == HIGH) {
    zustand = 0;
  }
  else {
    zustand = 1;
  }
  return zustand;
}

void wassersensor() {
  // Wassersensor
  int value = analogRead(waterPin);
  if (value > 350) {
    PORTD = PORTD | (1 << 2);
    delay(1000);
    PORTD = PORTD & ~(1 << 2);
    delay(1000);
  }
}

void notruf() {
  int status_button = digitalRead(button);
  if (status_button == LOW) {
    Phone.call("004917675867969");
  }
  else {
    PORTD = PORTD & ~(1 << 4);
    PORTD = PORTD & ~(1 << 5);
    PORTD = PORTD & ~(1 << 2);

  }
}
int i = 0;
void giroscope() {
  /* Always get the values of the accelerometer in the 3 dimensions (X, Y and Z). */
  float x, y, z;

  int magnitudeThreshold = 25;
  x = AccelerometerSensor.getX();
  y = AccelerometerSensor.getY();
  z = AccelerometerSensor.getZ();

  /* Check the motion of the smartphone, if it exceeded a limit, turn the LED on. */
  /* We do that by calcuating the magnitude of the 3D vectors. */
  if (sqrt((x * x) + (y * y) + (z * z)) > magnitudeThreshold)
  {
    Phone.call("004917675867969");
    float lat = GPS.getLatitude();
    float lon = GPS.getLongitude();
    i= i +1;
    String sendlat= "Latitude: " + String(lat);
    String sendlon= "Longitude: " + String(lon);
    String msg1 = "Hello fam I need help my GPS Coordinates are:\n";
    String counter = String(i);
    String msg =msg1+sendlat+"\n"+sendlon+"\n"+counter;
    
    Twitter.tweet(msg);
    
    //i= i +1;
    OneSheeld.delay(300);
  }
}
