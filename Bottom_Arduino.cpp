# include <Servo.h>
Servo myservo;
int angleOpen = 90;
int angleClose = -90;

int openHot = 0;
int openRoom = 0;
int openCold = 0;

float fsrReadHot, fsrReadCold, fsrReadRoom;
float coldTemp, hotTemp, coldRead, hotRead;
bool coldActive=false, hotActive=false;

const float fsrLimit75 = 800;
const float fsrLimit20 = 594;

void buzzer(float& fsrReadRoom);

void setup()
{
    Serial.begin(9600);
    pinMode(A5, INPUT); pinMode(2, OUTPUT);
    pinMode(A4, INPUT); pinMode(3, OUTPUT);
    pinMode(A3, INPUT); pinMode(4, OUTPUT);
    pinMode(A2, INPUT); pinMode(5, OUTPUT);
    pinMode(A1, INPUT); pinMode(6, OUTPUT);
    pinMode(A0, INPUT); pinMode(7, INPUT);
                        pinMode(8, INPUT);
                        pinMode(9, OUTPUT);
                        pinMode(10,OUTPUT);
                        pinMode(11,OUTPUT);
                        pinMode(12,OUTPUT);
                        pinMode(13,OUTPUT);
    delay(2000);
}

void loop()
{
    fsrReadHot = analogRead(A5);
    fsrReadCold = analogRead(A4);
    fsrReadRoom = analogRead(A3);

    coldRead = analogRead(A1);
    hotRead = analogRead(A0);

    coldTemp = map(((coldRead - 20)*3.04), 0, 1023, -40, 125);
    hotTemp = map(((hotRead - 20)*3.04), 0, 1023, -40, 125);

    // Cek ketinggian air
    if (fsrReadRoom<fsrLimit20)
    {
        buzzer(fsrReadRoom);
        digitalWrite(9,HIGH);
    }
    else
    {
        digitalWrite(9,LOW);
        if (fsrReadHot<fsrLimit75) {digitalWrite(2,HIGH);}
        if (fsrReadCold<fsrLimit75) {digitalWrite(4,HIGH);}
        if (fsrReadHot>=fsrLimit75) {digitalWrite(2,LOW);}
        if (fsrReadCold>=fsrLimit75) {digitalWrite(4,LOW);}

        // Cooling
        if (coldTemp>=10)
        {
            digitalWrite(13, HIGH); //Cooler on
            coldActive=true;
        }
        else if ((coldTemp>=5) and (coldActive==true))
        {
            digitalWrite(13, HIGH); //Cooler still on
        }
        else if (coldTemp<5)
        {
            digitalWrite(13, LOW); //Cooler off
            coldActive=false;
        }

        // Heating
        if (hotTemp<=90)
        {
            digitalWrite(12, HIGH); //Heater on
            hotActive=true;
        }
        else if ((hotTemp<=95) and (hotActive==true))
        {
            digitalWrite(12, HIGH); //Heater still on
        }
        else if (hotTemp>95)
        {
            digitalWrite(12, LOW); //Heater off
            hotActive=false;
        }

        //Katup air
        if ( (digitalRead(A2)==HIGH) && (hotActive == false) )
        {
            digitalWrite(10,HIGH);
            myservo.attach(5);
            myservo.write(angleOpen); //buka air panas
            openHot = 1;
        }
        if (openHot==1)
        {
            if ((digitalRead(A2)==LOW) && (hotActive == true))
            {
                myservo.write(angleClose); //tutup air panas
            }
        }

        if (digitalRead(8)==HIGH)
        {
            digitalWrite(10,HIGH);
            myservo.attach(6);
            myservo.write(angleOpen); //buka air
            openRoom = 1;
        }
        if (openRoom==1)
        {
            if (digitalRead(8)==LOW)
            {
                myservo.write(angleClose); //tutup air
            }
        }

        if ((digitalRead(7)==HIGH) && (coldActive == false))
        {
            digitalWrite(10,HIGH);
            myservo.attach(11);
            myservo.write(angleOpen); //buka air dingin
            openCold = 1;
        }
        if (openCold==1)
        {
            if ((digitalRead(7)==LOW) || (coldActive == true))
            {
                myservo.write(angleClose); //tutup air dingin
            }
        }
    }

delay(100);
}
void buzzer(float& fsrReadRoom)
{
    do
    {
        tone(3, 220, 500);
delay(500);
    fsrReadRoom = analogRead(A3);
    } while (fsrReadRoom<fsrLimit20);
}
