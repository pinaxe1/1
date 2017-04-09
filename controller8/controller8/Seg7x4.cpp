#include "Arduino.h"
#include "Seg7x4.h"

//----------------------------------------------------------------
//  Инициализация Индикатора и клавиатуры

void Seg7x4init()
{ 
  
   pinMode(dataPin    , OUTPUT); 
  pinMode(clockPin   , OUTPUT);
  pinMode(latchPin   , OUTPUT);
  pinMode(kbLatchPin , OUTPUT);
  pinMode(kbdPin     , INPUT );
}




//----------------------------------------------------------------
// Метод выводит 10_ичное число на индикатор 4х7

void Digits4Write( int data){ 
  int da;
  digitalWrite(latchPin, HIGH); //Отключаем вывод на регистре
  da=data/10000;shiftOut(dataPin, clockPin, MSBFIRST, znak[da]); data=data-10000*da;// Сперва старший разряд
  da=data/1000; shiftOut(dataPin, clockPin, MSBFIRST, znak[da]); data=data-1000*da; // затем cледуюший
  da=data/100;  shiftOut(dataPin, clockPin, MSBFIRST, znak[da]); data=data-100*da;  // и так далее
  da=data/10;   shiftOut(dataPin, clockPin, MSBFIRST, znak[da]); data=data-10*da;   // до самого
  da=data/1;    shiftOut(dataPin, clockPin, MSBFIRST, znak[da]);                    // младшего разряда
  digitalWrite(latchPin, LOW); // "защелкиваем" регистр, чтобы байт появился на его выходах
  digitalWrite(latchPin, HIGH); //Отключаем вывод на регистре
  regWrite(255,kbLatchPin); //  Восстановить регистр клавы. Процедура портит содержимое
}

//----------------------------------------------------------------
// выводит произвольный набор бит на 4х7 сегментный индикатор 	 

void Raw4Write( long int data)
{ int i;
  digitalWrite(latchPin, HIGH); //Отключаем вывод на регистре
  for (i=0; i<5 ; i++){ 
    shiftOut(dataPin, clockPin, MSBFIRST, lowByte(data)); 
    data=data>>8; 
  }
  digitalWrite(latchPin, LOW); // "защелкиваем" регистр, чтобы байт появился на его выходах
  digitalWrite(latchPin, HIGH); //Отключаем вывод на регистре
}


//----------------------------------------------------------------
//  записывает один бит в регистр индикаторов

void aBitWrite( byte data) 
{
  digitalWrite(kbLatchPin,  LOW ); // Отключаем вывод на регистре
  digitalWrite(dataPin , data ); // бит в регистр ;
  digitalWrite(clockPin,  LOW ); // проталкиваем 
  digitalWrite(clockPin, HIGH ); // проталкиваем 
  digitalWrite(kbLatchPin, HIGH ); // "защелкиваем" регистр, чтобы байт появился на его выходах
}

//----------------------------------------------------------------
// читаем номер нажатой кнопки
byte aKeyPressed()
{ byte i,j;
  for ( i=0; ((i<8)&&(digitalRead(kbdPin)));i++)//Прогоняем 0 и проверяем возврат
     aBitWrite(0);  // по всем разрядам пока единичка не пропадет
  if (i>0) regWrite(255,kbLatchPin); // Взвести регистр клавы если был опрос  
  return(i);
}

byte aKey()
{ byte i,j;
  j=aKeyPressed();
  for ( i=0; (i<3&&j==aKeyPressed());i++);
  if (i<3) j=0; // Если хоть раз из трех прочиталось по разному сбросим значение в ноль
  return(j);
}

//----------------------------------------------------------------
//  записывает одно знакоместо в регистр

void regWrite( byte data, const byte latch) 
{
  digitalWrite(latch, LOW); //Отключаем вывод на регистре
  shiftOut(dataPin, clockPin, MSBFIRST, data); // проталкиваем 1 байт в регистр индикатора
  digitalWrite(latch, HIGH); // "защелкиваем" регистр, чтобы байт появился на его выходах
}

//----------------------------------------------------------------
//  текст в регистр бегущей (ползущей) строкой
void writeStr( const byte text[])
{ byte count=text[0];
  Raw4Write(0);
  for(int i=1; i<=count; i++){
    regWrite(text[i],latchPin);
    if (aKeyPressed()) break;
    delay(400);
  } 
  for(int i=1; i<3; i++){
    regWrite(0,latchPin);
    if (aKeyPressed()) break;
    delay(400);
  } 
  regWrite(255,kbLatchPin); //  Восстановить регистр клавы. Процедура портит содержимое
}
//----------------------------------------------------------------
// Произвольный набор бит в регистр из буфера
void write4S(const byte buf[],byte j)
{ int i;
  digitalWrite(latchPin, HIGH); //Отключаем вывод на регистре
  for (i=j; i<j+5 ; i++) shiftOut(dataPin, clockPin, MSBFIRST, buf[i]); 
  digitalWrite(latchPin, LOW); // "защелкиваем" регистр, чтобы байт появился на его выходах
  digitalWrite(latchPin, HIGH); //Отключаем вывод на регистре
  regWrite(255,kbLatchPin); //  Восстановить регистр клавы. Процедура портит содержимое
}



