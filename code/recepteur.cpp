#include <Arduino.h>
#include <iostream>
#include <chrono>

const String strState[] = {
    "INIT_SEUIL",
    "WAITING",
    "BIT_1",
    "BIT_2",
    "BIT_3",
    "DATA",
    "PARITE"};

String stateToString(int state)
{
  return strState[state];
}

enum State
{
  INIT_SEUIL,
  WAITING,
  BIT_1,
  BIT_2,
  BIT_3,
  DATA,
  PARITE
};

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

class FSM
{
  State mCurrentState;

public:
  FSM();

  void checkState(State source, State target, bool condition = true);

  State getCurrentState();
};

FSM::FSM() : mCurrentState(INIT_SEUIL)
{
}

State FSM::getCurrentState()
{
  return mCurrentState;
}

void FSM::checkState(State source, State target, bool condition)
{
  if (source == mCurrentState && condition)
  {
    mCurrentState = target;
  }
}

const int pininputlaser = 34;
FSM fsm;
Timer timer = Timer();
Timer *timerPtr = &timer;

char dataTrame[5];

char currentBit = 'e';
char dataParite = 'e';

const int LIMIT_VALUE_INPUT_LASER = 200;
const int TIME_LISTENING_MS = 16;
const int TIME_WAITING_MS = 20;

void setup()
{
  Serial.begin(9600);
  pinMode(pininputlaser, INPUT);
}

void RunFsm()
{
  fsm.checkState(PARITE, WAITING, dataTrame[3] == dataTrame[4] && timerPtr->_elapsed() > TIME_LISTENING_MS);
  fsm.checkState(PARITE, INIT_SEUIL, dataTrame[3] != dataTrame[4] && timerPtr->_elapsed() > TIME_LISTENING_MS);

  fsm.checkState(DATA, PARITE, true);

  fsm.checkState(BIT_3, DATA, currentBit == '0');
  fsm.checkState(BIT_3, WAITING, currentBit == '1' && timerPtr->_elapsed() > TIME_LISTENING_MS);

  fsm.checkState(BIT_2, BIT_3, currentBit == '1' && timerPtr->_elapsed() > TIME_LISTENING_MS);
  fsm.checkState(BIT_2, WAITING, currentBit == '0' && timerPtr->_elapsed() > TIME_LISTENING_MS);

  fsm.checkState(BIT_1, BIT_2, currentBit == '1' && timerPtr->_elapsed() > TIME_LISTENING_MS);
  fsm.checkState(BIT_1, WAITING, currentBit == '0' && timerPtr->_elapsed() > TIME_LISTENING_MS);

  fsm.checkState(WAITING, BIT_1, true);

  // TODO : Implementer une methode pour initier le seuil
  fsm.checkState(INIT_SEUIL, WAITING, true);
}

void getCurrentBit()
{
  timerPtr->start();

  while (timerPtr->_elapsed() < TIME_LISTENING_MS)
  {
    int analogValue = analogRead(pininputlaser);
    //// Serial.println(analogValue);
    if (analogValue > LIMIT_VALUE_INPUT_LASER)
    {
      currentBit = '1';
      delay(TIME_WAITING_MS);
    }
    else
    {
      currentBit = '0';
    }
  }
}

void resetDataTrame()
{
  dataTrame[0] = 'e';
  dataTrame[1] = 'e';
  dataTrame[2] = 'e';
  dataTrame[3] = 'e';
  dataTrame[4] = 'e';
}

void loop()
{
  RunFsm();

  switch (fsm.getCurrentState())
  {
  case INIT_SEUIL:
    break;
  case WAITING:
    resetDataTrame();
    break;
  case BIT_1:
    getCurrentBit();
    dataTrame[0] = currentBit;
    break;
  case BIT_2:
    getCurrentBit();
    dataTrame[1] = currentBit;
    break;
  case BIT_3:
    getCurrentBit();
    dataTrame[2] = currentBit;
    break;
  case DATA:
    getCurrentBit();
    dataTrame[3] = currentBit;
    break;
  case PARITE:
    getCurrentBit();
    dataTrame[4] = currentBit;
    if (dataTrame[3] != dataTrame[4])
    {
      Serial.println("########################");
      Serial.println(dataTrame);
      Serial.println("########################");
    }

    break;
  }
}
