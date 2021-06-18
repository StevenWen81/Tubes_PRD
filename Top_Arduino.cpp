#include <Adafruit_NeoPixel.h>
#define PIN 2
#define NUMPIXELS 20
# include <Servo.h>
Servo myservo;
int angleOpen = 90; //buka katup servo 90 derajat
int angleClose = -90; // tutup kembali katup servo 90 derajat

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int trigPin = 9;      const int echoPin = 10;
const int buzzerPin = 7;    const int startPin = 4;
const int push1Pin = 3;     const int push2Pin = 5;
const int push3Pin = 6;     const int push4Pin = 8;

const int L = 40; //base to top distance
int ori_input, input, value;
float duration, dist, adjDist;
bool Set=false, potSet=false, tempSet=false, error=false;
int setLed, currentHeight = 0, currentLed = 0;
int alarm = 5;
float multiplier = 0 ;
bool buzz=true;

void initialSetting(int ori_input, bool& Set, int& setLed);
void setup()
{
    Serial.begin(9600);
    pixels.begin();
    pinMode(trigPin, OUTPUT);   pinMode(echoPin, INPUT);
    pinMode(buzzerPin, OUTPUT); pinMode(startPin, INPUT);
    pinMode(push1Pin, INPUT);   pinMode(push2Pin, INPUT);
    pinMode(push3Pin, INPUT);   pinMode(push4Pin, INPUT);
    delay(2000);
    ori_input = analogRead(A5);
}

void loop()
{
    if (!Set)
    {
        initialSetting(ori_input, Set, setLed);
    }

    if (Set)
    {
        Serial.println(currentLed);
        Serial.println(setLed);
        Serial.println();
        if (currentLed<setLed)
        {
            for (int i=currentLed+1; i<=setLed; i++)
            {pixels.setPixelColor(i, pixels.Color(0,0,255));}
            pixels.show();

            // Servo
            myservo.attach(11);
            myservo.write(angleOpen);

            // Ultrasonic
            digitalWrite(trigPin, LOW);
            delayMicroseconds(5);
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10);
            digitalWrite(trigPin, LOW);
            pinMode(echoPin, INPUT);
            duration = pulseIn(echoPin, HIGH);
            pixels.setPixelColor(currentLed, pixels.Color(0,0,0));
            pixels.show();
            dist = ((duration/2) / 29.1 ); // Data tersimpan dalam variabel cm
            adjDist = (dist-16)/6;
            currentHeight = L - adjDist;
            currentLed = currentHeight/2;

            for (int i=0; i<=currentLed; i++)
            {pixels.setPixelColor(i, pixels.Color(255,0,0));}
            pixels.show();
        }
        else
        {
            if (buzz==true)
            {
                myservo.write(angleClose); // tutup servo
                for (int i=0; i<=setLed; i++)
                {pixels.setPixelColor(i, pixels.Color(0,255,0));}
                for (int i=setLed+1; i<20; i++)
                {pixels.setPixelColor(i, pixels.Color(0,0,0));}
                pixels.show();
                while (alarm>0)
                {
                    // Buzzer
                    alarm = alarm - 1;
                    tone(7, 220, 100);
                    delay(900);
                }
                buzz=false;
            }
            else
            {
                for (int i=0; i<=setLed; i++)
                {pixels.setPixelColor(i, pixels.Color(0,0,0));}
                pixels.show();
            }
        }
    }
    delay(100);
}

void initialSetting(int ori_input, bool& Set, int& setLed)
{
    int relayInfo = analogRead(A0);
    input = analogRead(A5);
    if (ori_input!=input) {potSet=true;}
    value = map(input, 0, 1023, 0, 19);

    // Info from arduino 2
    if (digitalRead(A0) == HIGH) {tempSet=true;}
    else if (digitalRead(A1) == HIGH) {error=true;}
    else if (digitalRead(A1) == LOW) {error=false;}

    // NeoPixel
    if (potSet==true)
    {
        for (int i=value+1; i<20; i++)
        {pixels.setPixelColor(i, pixels.Color(0, 0, 0));}
        for (int i=0; i<=value; i++)
        {pixels.setPixelColor(i, pixels.Color(255,0,0));}
        pixels.show();
    }

    if ( digitalRead(push1Pin) == HIGH ) { multiplier = 0.25;}
    else if ( digitalRead(push2Pin) == HIGH ) { multiplier = 0.5;}
    else if ( digitalRead(push3Pin) == HIGH ) { multiplier = 0.75;}
    else if ( digitalRead(push4Pin) == HIGH ) { multiplier = 1;}

    if (!error)
    {
        if ( digitalRead(startPin) == HIGH )
        {
            Serial.println(multiplier);
            Serial.println(potSet);
            Serial.println(tempSet); Serial.println();
            if (( multiplier == 0 ) || ( potSet == false ) || ( tempSet == false))
            {
                delay(900);
                tone(7, 220, 100);
            }
            else
            {
                Set=true;
                setLed=multiplier*value;
                for (int i=0; i<=value; i++)
                {pixels.setPixelColor(i, pixels.Color(0,0,0));}
                pixels.show();
            }
        }
    }
}
