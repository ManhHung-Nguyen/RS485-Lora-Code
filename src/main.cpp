#include <Arduino.h>
#include <SoftwareSerial.h>
#include "CPU.h"
#define BAUDRATE 115200
/*************** 2 MAX485 *************************
SoftwareSerial Reader(A3, A4);
SoftwareSerial Writer(10, 11);
/*************** 1 MAX485 1 LORA ******************/
SoftwareSerial Reader(10, 11);
SoftwareSerial Writer(A4, A3);
/*************************************************/
int bits[] = { 9, 8, 7, 6, 5, 4, 3, 2 };

Buffer obuff(10);
Buffer ibuff(10);

int clock_delay() {
  delayMicroseconds(100);
  return 10;
}

Clock SysClock;

void Send(Buffer& buf) {
    byte *it = buf.Handle();
    while (*it != 0) {
        delay(10);
        log << (char)*it;
        Writer.write(*it++);
    } 
}

void on_second() {
  log << SysClock.Time()<< endl;
}

void Alarm() {
    obuff[0] = '*';
    Send(obuff);
    obuff[0] = '#';
}

void write_rs485(int v) {
    Writer.write((byte)v);
    log << (char)v;
}

void read_input() {
    if (digitalRead(12) == 0) {
        Alarm();
    } else if (Reader.available()) {
        int s = Reader.read();
        write_rs485(s);
        while (Reader.available()) {
            write_rs485(Reader.read());
        }
    }
}
void on_milisecond() {
  int m = SysClock.GetMilisecond();
  if ((m % 500) == 0) {
    read_input();
  }
}

void on_minute() {
  log << endl << "CHECK CONNECTED" << endl;
  Send(obuff);
}

void GetAddress() {
  int a = 0;
  for( int i = 0; i < 8; i++ ) {
    if (digitalRead(bits[i]) == 0) {
      a |= 1 << i;
    }
  }
  obuff << "$" << a << "\n";
  log << obuff << endl;
}

void Begin() {
  Serial.begin(9600);
  Writer.begin(9600);
  Reader.begin(BAUDRATE);

  pinMode(12, INPUT_PULLUP);
  for( int i = 0; i < 8; i++ ) {
    pinMode(bits[i], INPUT_PULLUP);
  }

  log << "BEGIN RS485" << endl;
  GetAddress();
}
void setup() {
  Begin();
  SysClock.SetEvent(Clock::Milisecond, on_milisecond);
  SysClock.SetEvent(Clock::Second, on_second);
  SysClock.SetEvent(Clock::Minute, on_minute);
  SysClock.Begin();
}

void loop() {
 
}