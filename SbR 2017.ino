#include <QTRSensors.h>

#define rightDirection 12 //Rotationsrichtung des rechten Motors: LOW = vorwärts; HIGH = rückwärts
#define rightPWM 3
#define leftDirection 13  //Rotationsrichtung des linken Motors: HIGH = vorwärts; LOW = rückwärts
#define leftPWM 11

#define rightMotor 'A'
#define leftMotor 'B'

#define trig A2//COMP/TRIG on the US
#define pwm A3//PWM on the US


#define NUM_SENSORS   8     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   0     // this defines the emitter and needs to be here for the sensor to work, i gave it the same pin as one of the sensor pins since it dosent matter (Edit v Leo: it matters if yuo want to switch the IR LEDs off)
QTRSensorsRC qtrrc((unsigned char[]) {
  A5, A4, 2, 4, 5, 6, 7, 10
}, NUM_SENSORS, TIMEOUT, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
#define black 1200
boolean starting;
boolean linefound;
boolean amhindernissvorbei;
int lastSensor;
int searchCounter;
int fahrcounter;

void setup()
{
  Serial.begin(9600);

  pinMode(rightPWM, OUTPUT);
  pinMode(leftPWM, OUTPUT);
  analogWrite(rightPWM, 0);
  analogWrite(leftPWM, 0);
  pinMode(rightDirection, OUTPUT);
  pinMode(leftDirection, OUTPUT);

  pinMode(trig, OUTPUT);
  pinMode(pwm, INPUT);
  digitalWrite(trig, HIGH);
  starting =true;
  linefound=true;
  fahrcounter=0;
}




void brakeMotor(char motor)//chose a motor rightMotor or leftMotor => brakes the motor to a speed of 0
{
  if (motor == rightMotor)
  {
    analogWrite(rightPWM, 0);
  }
  else
  {
    analogWrite(leftPWM, 0);
  }
}

void setMotorSpeed(char motor, int motorspeed)//choose a motor rightMotor or leftMotor and then choose a speed between 255 and -255 => motor gets set to that speed
{
  if (motor == rightMotor)
  {
    if (motorspeed > 0)
    {
      digitalWrite(rightDirection, HIGH);
      analogWrite(rightPWM, motorspeed);
    }
    else
    {
      digitalWrite(rightDirection, LOW);
      analogWrite(rightPWM, abs(motorspeed));
    }
  }
  else
  {
    if (motorspeed > 0)
    {
      digitalWrite(leftDirection, HIGH);
      analogWrite(leftPWM, motorspeed);
    }
    else
    {
      digitalWrite(leftDirection, LOW);
      analogWrite(leftPWM, abs(motorspeed));
    }
  }
}


void readSensors()//reads the sensor
{
  qtrrc.read(sensorValues);
}

void testSensors()
{
  qtrrc.read(sensorValues);
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println();

  delay(250);
}

int readUS() {                             // a low pull on pin COMP/TRIG  triggering a sensor reading
  digitalWrite(trig, LOW);
  digitalWrite(trig, HIGH);               // reading Pin PWM will output pulses
  
  unsigned long distanceMeasured = pulseIn(pwm, HIGH);
  //unsigned int distance = 0; // not needed anymore
  if (distanceMeasured >= 10200)
  { // the reading is invalid.
    return 300;
  }
  else
  {
    //distance = distanceMeasured/50;           // every 50us low level stands for 1cm
    //return distance;
    return distanceMeasured / 50; //better
  }
}

void search()
{ 
 linefound = false;
 while( linefound == false)
 {
  setMotorSpeed(rightMotor,-100);
  setMotorSpeed(leftMotor,100);
  delay(1000);
  setMotorSpeed(rightMotor,0);
  setMotorSpeed(leftMotor,0);
  readSensors();
  if (sensorValues[0] > black || sensorValues[1] > black || sensorValues[2] > black || sensorValues[3] > black || sensorValues[4] > black || sensorValues[5] > black || sensorValues[6] > black || sensorValues[7] > black)
  {
    linefound=true;
    break;
  }
  for(int i=0; i<=6; i=i+1)
  {
    setMotorSpeed(rightMotor,-100);
    setMotorSpeed(leftMotor,-100);
    delay(500);
    setMotorSpeed(rightMotor,0);
    setMotorSpeed(leftMotor,0);
    readSensors();
    if (sensorValues[0] > black || sensorValues[1] > black || sensorValues[2] > black || sensorValues[3] > black || sensorValues[4] > black || sensorValues[5] > black || sensorValues[6] > black || sensorValues[7] > black)
     {
      linefound=true;
      searchCounter=0;
      return;
     }
  }
  for(int i=0; i<=12; i=i+1)
  {
    setMotorSpeed(rightMotor,100);
    setMotorSpeed(leftMotor,100);
    delay(500);
    setMotorSpeed(rightMotor,0);
    setMotorSpeed(leftMotor,0);
    readSensors();
    if (sensorValues[0] > black || sensorValues[1] > black || sensorValues[2] > black || sensorValues[3] > black || sensorValues[4] > black || sensorValues[5] > black || sensorValues[6] > black || sensorValues[7] > black)
     {
      linefound=true;
      searchCounter=0;
      return;
      }
  }
  setMotorSpeed(rightMotor,-100);
  setMotorSpeed(rightMotor,-100);
  delay(3000);
  
 }
}

void hinderniss()
{
  amhindernissvorbei=false;
  setMotorSpeed(rightMotor,100);
  setMotorSpeed(leftMotor,100);
  delay(4000);
  setMotorSpeed(rightMotor,0);
  setMotorSpeed(leftMotor,0);
  setMotorSpeed(rightMotor,-100);
  setMotorSpeed(leftMotor,100);
  delay(6000);
  while( amhindernissvorbei==false)
  {
    setMotorSpeed(rightMotor,-100);
    setMotorSpeed(leftMotor,-100);
    delay(4000);
    int fahrcounter = fahrcounter+1;
    if (readUS()> 10)
    {
      amhindernissvorbei=true;
      break;
    }
    else
    {
       setMotorSpeed(rightMotor,100);
       setMotorSpeed(leftMotor,100);
       delay(4000);
       setMotorSpeed(rightMotor,0);
       setMotorSpeed(leftMotor,0);
       setMotorSpeed(rightMotor,-100);
       setMotorSpeed(leftMotor,100);
       delay(5000);
    }
    
  }
   setMotorSpeed(rightMotor,-100);
   setMotorSpeed(leftMotor,100);
   delay(15000);
   amhindernissvorbei=false;

  while( amhindernissvorbei==false)
  {
    setMotorSpeed(rightMotor,-100);
    setMotorSpeed(leftMotor,-100);
    delay(4000);
    if (readUS()> 10)
    {
      amhindernissvorbei=true;
      break;
    }
    else
    {
       setMotorSpeed(rightMotor,100);
       setMotorSpeed(leftMotor,100);
       delay(4000);
       setMotorSpeed(rightMotor,0);
       setMotorSpeed(leftMotor,0);
       setMotorSpeed(rightMotor,-100);
       setMotorSpeed(leftMotor,100);
       delay(5000);
    }
    
  }
  setMotorSpeed(rightMotor,-100);
  setMotorSpeed(leftMotor,100);
  delay(6000*fahrcounter);
  setMotorSpeed(rightMotor,100);
  setMotorSpeed(leftMotor,100);
  search();
  
}


void loop()
{
  mainWorking();
  //motorTest();
  //Serial.println("Test");
  //analogWrite(leftPWM, 200);
  //analogWrite(rightPWM, 200);
  //Serial.println("ture");
  //delay(1000);
  //test1();
  //test2();
  //testTurn();
  
}

void mainWorking() {
  /*Serial.print("US:");
  Serial.print(readUS());
  Serial.print("\n");
  if (i==0)
  {
    setMotorSpeed( rightMotor,100);
    setMotorSpeed( leftMotor,-100);
    int i=i+1;
  }
  */
  if (readUS() > 10)
  {
    readSensors();
    /*if (sensorValues[0] > blackReflectance || sensorValues[1] > blackReflectance || sensorValues[2] > blackReflectance || sensorValues[3] > blackReflectance || sensorValues[4] > blackReflectance || sensorValues[5] > blackReflectance || sensorValues[6] > blackReflectance || sensorValues[7] > blackReflectance)
    {
      rightMotorSpeed += 50;
      leftMotorSpeed += 51;
    }
    if (sensorValues[0] > blackReflectance || sensorValues[1] > blackReflectance) {
      rightMotorSpeed += 75;
    }
    if (sensorValues[6] > blackReflectance || sensorValues[7] > blackReflectance) {
      leftMotorSpeed += 76;
    }
    if (sensorValues[0] > blackReflectance) {
      rightMotorSpeed += 75;
    }
    if (sensorValues[7] > blackReflectance) {
      leftMotorSpeed += 76;
    }
    leftMotorSpeed = leftMotorSpeed * (-1);
    setMotorSpeed(rightMotor, rightMotorSpeed);
    Serial.print("Right:");
    Serial.print(rightMotorSpeed);
    Serial.print("\n");
    setMotorSpeed(leftMotor, leftMotorSpeed);
    Serial.print("Left:");
    Serial.print(leftMotorSpeed);
    Serial.print("\n");
  */


    if(sensorValues[7]>=black)
    {
      setMotorSpeed(leftMotor,100);
      setMotorSpeed(rightMotor,-25);
      lastSensor= 7;
      searchCounter=0;
    }
    else if(sensorValues[0]>=black)
    {
      setMotorSpeed(leftMotor,25);
      setMotorSpeed(rightMotor,-100);
      lastSensor=0;
      searchCounter=0;
    }
    else if(sensorValues[6]>=black)
    {
      setMotorSpeed(leftMotor,100);
      setMotorSpeed(rightMotor,-50);
      lastSensor=6;
      searchCounter=0;
    }
    
    else if(sensorValues[1]>=black)
    {
      setMotorSpeed(leftMotor,50);
      setMotorSpeed(rightMotor,-100);
      lastSensor=1;
      searchCounter=0;
    }
   
    else if(sensorValues[5]>=black)
    {
      setMotorSpeed(leftMotor,100);
      setMotorSpeed(rightMotor,-75);
      lastSensor=5;
      searchCounter=0;
    }
    else if(sensorValues[2]>=black)
    {
      setMotorSpeed(leftMotor,75);
      setMotorSpeed(rightMotor,-100);
      lastSensor=2;
      searchCounter=0;
    }
    else if (sensorValues[4]>=black || sensorValues[3]>=black)
    {
      setMotorSpeed(rightMotor,-100);
      setMotorSpeed(leftMotor,100);
      lastSensor=3;
      searchCounter=0; 
    }
    else
    {
     if( searchCounter<= 30)
     {
      if(lastSensor==7)
      {
        setMotorSpeed(leftMotor,25);
        setMotorSpeed(rightMotor,-150);
        searchCounter +=1;
      }
      else if (lastSensor ==0)
      {
        setMotorSpeed(leftMotor,150);
        setMotorSpeed(rightMotor,-25);
        searchCounter +=1;
      }
    }
    else
    {
    search();
    }
    
  }
  }
  else
  {
    brakeMotor(rightMotor);
    brakeMotor(leftMotor);
    hinderniss();
  }
}


void test1() {
  Serial.print("US: ");
  Serial.println(readUS());
  Serial.print("LineSensor: ");
  testSensors();
  delay(100);
}

void test2() {
  testSensors();
  delay(100);
}

void motorTest()
{
  setMotorSpeed(rightMotor,150);
  setMotorSpeed(leftMotor,-150);
}
void startMotor()
{
  if (starting == true)
  {
  setMotorSpeed(rightMotor,25);
  setMotorSpeed(leftMotor,-25);
  delay(500);
  setMotorSpeed(rightMotor,50);
  setMotorSpeed(leftMotor,-50);
  delay(500);
  starting =false;
  }

}
void testTurn()
{
  setMotorSpeed(rightMotor,100);
  setMotorSpeed(leftMotor,100);
  delay(4000);
  setMotorSpeed(rightMotor,0);
  setMotorSpeed(leftMotor,0);
  delay(2000);
}

  



