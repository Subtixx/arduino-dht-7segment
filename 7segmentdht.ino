#include <DHT.h> // Requires AdaFruit's DHT Sensor Library (https://github.com/adafruit/DHT-sensor-library)
#include <DHT_U.h>
#include <SimpleTimer.h> // Requires SimpleTimer (https://playground.arduino.cc/Code/SimpleTimer)
#include <math.h>

#define DHT_UPDATE_SECONDS 10

#define DHTPIN 9
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22

#define PIN_G 2
#define PIN_F 3
#define PIN_E 4
#define PIN_D 5
#define PIN_C 6
#define PIN_B 7
#define PIN_A 8

#define PIN_DIGIT_1 10
#define PIN_DIGIT_2 11
#define PIN_DIGIT_3 12

#define PIN_SWITCH_DISPLAY 13

DHT dht(DHTPIN, DHTTYPE);

const int digits[10][7] = {
  // G     F    E      D     C     B     A
  { HIGH, LOW,  LOW,  LOW,  LOW,  LOW,  LOW },  // 0
  { HIGH, HIGH, HIGH, HIGH, LOW,  LOW,  HIGH }, // 1  
  { LOW,  HIGH, LOW,  LOW,  HIGH, LOW,  LOW },  // 2
  { LOW,  HIGH, HIGH, LOW,  LOW,  LOW,  LOW },  // 3
  { LOW,  LOW,  HIGH, HIGH, LOW,  LOW,  HIGH }, // 4
  { LOW,  LOW,  HIGH, LOW,  LOW,  HIGH, LOW },  // 5
  { LOW,  LOW,  LOW,  LOW,  LOW,  HIGH, LOW },  // 6
  { HIGH, HIGH, HIGH, HIGH, LOW,  LOW,  LOW },  // 7
  { LOW,  LOW,  LOW,  LOW,  LOW,  LOW,  LOW },  // 8
  { LOW,  LOW,  HIGH, LOW,  LOW,  LOW,  LOW }   // 9
};

/**
 * The current number we are displaying
 */
int num = 0;

/**
 * The cached humidity value
 */
float humidity = 0.0f;

/**
 * The cached temperature value
 */
float temperature = 0.0f;

/**
 * The timer for the DHT Update
 */
SimpleTimer dhtUpdateTimer;

/**
 * The current mode we are in
 * 0 = Temperature
 * 1 = Humidity
 */
int mode = 0;

int lastSwitchButtonState = 0;

void updateDisplayNumber()
{
  if(mode == 0){
    if (!isnan(temperature))
      num = floor(temperature);
  }else if(mode == 1) {
    if (!isnan(humidity))
      num = floor(humidity);
  }  
}

void updateDHTValue()
{
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  updateDisplayNumber();
}

void setup() {
  Serial.begin(9600);
  
  dht.begin();
  updateDHTValue();
  dhtUpdateTimer.setInterval(DHT_UPDATE_SECONDS * 1000, updateDHTValue);

  pinMode(PIN_DIGIT_1, OUTPUT);
  pinMode(PIN_DIGIT_2, OUTPUT);
  pinMode(PIN_DIGIT_3, OUTPUT);
  
  digitalWrite(PIN_DIGIT_1, LOW);
  digitalWrite(PIN_DIGIT_2, LOW);
  digitalWrite(PIN_DIGIT_3, LOW);

  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_D, OUTPUT);
  pinMode(PIN_E, OUTPUT);
  pinMode(PIN_F, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  digitReset();
  
  pinMode(PIN_SWITCH_DISPLAY, INPUT);  
}

void loop() {
  dhtUpdateTimer.run();

  int switchButtonState = digitalRead(PIN_SWITCH_DISPLAY);
  if(switchButtonState != lastSwitchButtonState)
  {
    if(switchButtonState == HIGH)
    {
      mode = mode == 0 ? 1 : 0;
      updateDisplayNumber();
      digitReset();      
    }
    delay(50);
  }
  lastSwitchButtonState = switchButtonState;
  
  displayNumber();
}

void displayNumber()
{
  if (num >= 100)
  {
    // Display first digit.
    digitalWrite(PIN_DIGIT_1, HIGH);
    digitWrite(1);
    digitalWrite(PIN_DIGIT_1, LOW);
    digitReset();
  }else{
    if(mode == 1)
    {
      digitalWrite(PIN_DIGIT_1, HIGH);
      digitalWrite(PIN_E, LOW);
      digitalWrite(PIN_DIGIT_1, LOW);
      digitReset();
    }
  }
  
  if (num >= 10)
  {
    // Display second digit.
    digitalWrite(PIN_DIGIT_2, HIGH);
    if (num >= 100)
      digitWrite(floor((num - 100) / 10));
    else
      digitWrite(floor(num / 10));
    digitalWrite(PIN_DIGIT_2, LOW);
    digitReset();
  }
  // Display last digit.
  digitalWrite(PIN_DIGIT_3, HIGH);
  digitWrite(ceil(fmod((num / 10.0f), 1) * 10));
  digitalWrite(PIN_DIGIT_3, LOW);
  digitReset();
}

void digitReset()
{
  int pin = 2;
  for (int j = 0; j < 7; j++) {
    digitalWrite(pin, HIGH);
    pin++;
  }
}

void digitWrite(int number)
{
  int pin = 2;
  for (int j = 0; j < 7; j++) {
    digitalWrite(pin, digits[number][j]);
    pin++;
  }
}
