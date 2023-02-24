#include <Arduino.h>
#include <iostream>
#include <chrono>


class Timer
{
public:
  Timer();
  unsigned long elapsed = 0;
  unsigned long startTime = 0;

  void start();
  unsigned long _elapsed();
};

Timer::Timer()
{
}

void Timer::start()
{
  startTime = millis();
}

unsigned long Timer::_elapsed()
{
  return millis() - startTime;
}

const int pininputlaser = 34;

Timer timer = Timer();
Timer* timerPtr = &timer;

void setup()
{
  Serial.begin(9600);
  pinMode(pininputlaser, INPUT);
}


const int STATE_WAITING = 0;
const int STATE_BIT_1 = 1;
const int STATE_BIT_2 = 2;
const int STATE_BIT_3 = 3;
const int STATE_DATA_PARITE = 4;

int currentState = STATE_WAITING;

char dataTrame[5];
int dataTrameIndex = 0;

char currentBit;
char dataParite= 'e';

const int LIMIT_VALUE_INPUT_LASER = 0;
const int TIME_LISTENING_MS = 16;
const int TIME_WAITING_MS = 20;

void loop()
{

  int analogValue= analogRead(pininputlaser);  
  
  // Choix de l'état
  switch (currentState)
  {
    case STATE_WAITING:
      //Récupération du bit actuel pendant 16ms
      timerPtr->start();
      while (timerPtr -> _elapsed() < TIME_LISTENING_MS) {
        analogValue = analogRead(pininputlaser);
        //Serial.println(analogValue);
        if (analogValue > LIMIT_VALUE_INPUT_LASER) {
          currentBit = '1';
          delay(TIME_WAITING_MS);
        } else {
          currentBit = '0';
        }
      }

      //Définition de l'état suivant en fonction de l'index de la trame
      switch (dataTrameIndex)
      {
        case 0:
          currentState = STATE_BIT_1;
          //Serial.println("STATE_BIT_1");
          break;
        case 1:
          currentState = STATE_BIT_2;
          //Serial.println("STATE_BIT_2");
          break;
        case 2:
          currentState = STATE_BIT_3;
          //Serial.println("STATE_BIT_3");
          break;
        case 3:
        case 4:
          currentState = STATE_DATA_PARITE;
          Serial.println("STATE_DATA_PARITE");
          break;  
        default:
          dataTrameIndex = 0;
          break;
      }
      break;
    case STATE_BIT_1:
      //On vérifie le premier bit de la trame depart
      if(currentBit == '1') {
        dataTrame[0] = currentBit;
        dataTrameIndex++;
      } else {
        dataTrameIndex = 0;
      }
      // Serial.println("############STATE_BIT_1############");
      // Serial.println(currentBit);
      // Serial.println("########################");
      currentState = STATE_WAITING;
      break;
    case STATE_BIT_2:
      //On vérifie le deuxième bit de la trame depart
      if(currentBit == '1') {
        dataTrame[1] = currentBit;
        dataTrameIndex++;
      } else {
        dataTrameIndex =0;
      }
      // Serial.println("###########STATE_BIT_2#############");
      // Serial.println(currentBit);
      // Serial.println("########################");
      currentState = STATE_WAITING;
      break;
    case STATE_BIT_3:
      //On vérifie le troisième bit de la trame depart
      if(currentBit == '0') {
        dataTrame[2] = currentBit;
        dataTrameIndex++;
      } else {
        dataTrameIndex =0;
      }
      // Serial.println("#############STATE_BIT_3###########");
      // Serial.println(currentBit);
      // Serial.println("########################");
      currentState = STATE_WAITING;
      break;
    case STATE_DATA_PARITE:
      if (dataTrameIndex == 3) {
        //Si on est à l'index 3, on récupère la data
        dataParite = currentBit;
        dataTrame[3] = currentBit;
        dataTrameIndex++;
      } else if (dataTrameIndex == 4 && currentBit != dataParite) {
        //Si on est à l'index 4, on récupère la parité et on vérifie qu'elle est différente de la data, on affiche la trame
        dataTrame[4] = currentBit;
        dataTrameIndex = 0;
        Serial.println("########################");
        Serial.println(dataTrame);
        Serial.println("########################");
      } else {
        //Si la parité est égale à la data, on reset la state machine
        dataTrameIndex = 0;
      }
      currentState = STATE_WAITING;
      break;
    default:
      break;
  }

}