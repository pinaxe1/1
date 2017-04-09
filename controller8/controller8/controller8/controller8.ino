/*
Управление намоточного станка
Project started 2013 Sept 11
Version 0.5     2013 Sept 30
Version 0.6     2013 Oct 6
PinAxe
*/
#include "Seg7x4.h"   // Функции дисплея клавы и Pin declarations 

byte opMode=aColdStart; // Состояния станка. Режимы работы

//------------------ Переменныые
long counter,        // счетчик импульсов енкодера
     dire;           // направление счета 
unsigned int stepFreq, // Частота шагов для шаговика
              D,     // диаметр провода
           vitkov;   // число витков намотать
byte mdir,           // направление подачи каретки 
     rdy;          // служебка счетчика импульсов енкодера
 
byte kbd,            //служебка клавиатуры     
     kbdo,           //служебка клавиатуры     
     wireR,wireL,    // положение провода
     termR,termL,    // положение каретки
     clk1 ,clk2 ,    // новое положение енкодера 
     clk1o,clk2o;    // старое положение енкоде  ра 

unsigned long mics,  // текущее значение таймера clk
              omic,  // прошлое значение таймера clk
              tClk;  // время между импульсами енкодера tClk=mics-omic
//------------------------------------------------------------------------------------
// Инициализация входов выходов и прочее
void setup() {
  //устанавливаем режим 
  Seg7x4init();
  pinMode(motorPin , OUTPUT);
  pinMode(stopPin  , OUTPUT);
  pinMode(dirPin   , OUTPUT);
  pinMode(clk1Pin  , INPUT );
  pinMode(clk2Pin  , INPUT );
  pinMode(termRPin , INPUT );
  pinMode(termLPin , INPUT );  
  
  digitalWrite(stopPin,0); // остановить укладчик
  //tone(motorPin, 0);// запуск step с частотой 0гц 
  // Serial.begin(9600);     //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< УДАЛИТЬ ОТЛАДКА
 //Serial.print(opMode);      //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< УДАЛИТЬ ОТЛАДКА
 //Serial.print(" ");      //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< УДАЛИТЬ ОТЛАДКА
 //Serial.println(opMode); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< УДАЛИТЬ ОТЛАДКА
  kbd=0;
  kbdo=kbd;
  vitkov=2000;
  D=200;
}

//------------------------------------------------------------------------------------
// Главный цикл программы
void loop()
{    
  ReadSensors(); // Читаем сенсоры и клавиатуру
  switch (opMode) {
    case aColdStart  : banner(tprivet,aDiamBanner,aColdStart,aDiamBanner);       break;
    case aDiamBanner : banner(tdiam  ,aDiamEntry ,aColdStart,aDiamEntry );       break;
    case aDiamEntry  : D=Enter4digit(D,500,aVitkiBanner,aColdStart);             break;
    case aVitkiBanner: banner(tvitki ,aVitkiEntry,aColdStart,aVitkiEntry);       break;
    case aVitkiEntry : vitkov=Enter4digit(vitkov,5000,aReadyBanner,aDiamBanner); break;
    case aReadyBanner: banner(tstart ,aRun ,aDiamBanner,aRun);                   break;
    case aRun        : DoControl();                                              break;
    case aPause      : banner(tpause,aRun  ,aColdStart ,aDiamBanner);            break; 
    case aFinish     : banner(tobed ,aColdStart,aColdStart ,aColdStart);         break;
  }
  kbdo=kbd;
}
//------------------------------------------------------------------------------------
// Вводим 4-х значное число
int Enter4digit(int Data, int maxi, byte EnterMode, byte EscMode)
{ int tmpD=Data;
  if (keyPress) // если нажали кнопку
    switch (kbd){ 
      case aKbd1 : tmpD++;            break;  
      case aKbd2 : tmpD+=10;          break;  
      case aKbd3 : tmpD+=100;         break;  
      case aKbd4 : tmpD+=1000;        break;
      //case aKbd4 : tmpD+=10000;        break;   
      case aEsc  : opMode=EscMode;    break;  
      case aEnter: opMode=EnterMode;  break;  
    }
  if (tmpD>maxi) tmpD=0;
  Digits4Write(tmpD);  
  return(tmpD);
}

//------------------------------------------------------------------------------------
// Читать состояние датчиков и кнопок.
void    ReadSensors()
{ regWrite(0xFF,kbLatchPin); 
  clk1o = clk1;                   // сохраняем предидущее
  clk2o = clk2;                   // состояние енкодера 
  clk1  = digitalRead( clk1Pin  );// читаем новое 
  clk2  = digitalRead( clk2Pin  );// положение енкодера 
  wireR = digitalRead( wireRPin );// читаем положение провода
  wireL = digitalRead( wireLPin );  
  termR = digitalRead( termRPin );// читаем положение каретки
  termL = digitalRead( termLPin );   
  kbd=aKey();              // читаем номер кнопки
}

//------------------------------------------------------------------------------------
// Логика и упарвляющие действия
void DoControl()
{ 

  if (termR==1 && mdir==goRight || termL==1 && mdir==goLeft); // Бежали влево сработал левый  концевик (право правый). Пауза 
  if (counter>=vitkov*encRate) opMode=aFinish; // Всё намотали пора остановиться
  if (termR){ mdir=goLeft;  digitalWrite(dirPin,mdir);}// Сработал ПРАВЫЙ концевик. Бежать влево  (установить напрвление подачи 1)
  if (termL){ mdir=goRight; digitalWrite(dirPin,mdir);}// Сработал ЛЕВЫЙ  концевик. Бежать вправо (установить напрвление подачи 0)
  
  if (wireR && mdir==goRight || wireL && mdir==goLeft ) stepFreq=SlowDown; // Укладчик убежал притормозим
  if (wireL && mdir==goRight || wireR && mdir==goLeft ) stepFreq=HurryUp;  // Укладчик отстал подгоним
  if (!wireL && !wireR )                                stepFreq=JustFine; // Укладчик идет как надо
  if (opMode!=aFinish && opMode!=aPause) // Если не закончили и не пауза то
              digitalWrite(stopPin,1); // Продолжать крутить
         else digitalWrite(stopPin,0); // иначе остановить укладчик 
  
  doCounterAndSteps(); // Здесь считаем импульсы с енкодера и запускаем серию STEP на шаговик укладчика
  Digits4Write(counter*10/encRate);  // Выводим на индикатор число витков

  if (millis() % 3000 ==0) 
  Digits4Write(counter);  // Выводим на индикатор число витков
  omic=mics;
}
//------------------------------------------------------------------------------------
// Считаем импульсы с енкодера и запускаем STEP на шаговик укладчика
void   doCounterAndSteps()
{
  if (!clk1&&!clk2) rdy=1;             // сброс направления СLK
  if (rdy &&  clk1 && !clk2) dire=-1;  // барабан идет вперед
  if (rdy && !clk1 &&  clk2) dire= 1;  // барабан идет назад
  if (clk1 && clk2 && rdy) {           // пришел сигнал на подвижку укладчика
    rdy=0;                             // сбрасываем готовность до следующего сигнала  
    omic=mics;                         // запоминаем старое время
    mics=micros();                     // читаем новое время
  //  if (mics<omic)                   // если счетчик micros перешел через 0
   // tClk=4294967295-omic+mics;
    //else
    tClk=mics-omic;
    stepFreq=156000*D/tClk;
    tone(motorPin, stepFreq);            // Запускаем укладчик на 2 миллисекунды <<<<<<<< зависит от провода
    counter=counter+dire;              // счетчик витков увеличить + -
  }
}


//----------------------------------------------------------------
// считаем скорость длительность и частоту для управлениея шаговиком
// <<<<<<<<<<<<<<<<<< Удалить эту функцию <<< в программе не используется
//byte poschitaem()
//{ int l1  ,  // Шаг намотки провода за один тик енкодера
//
     // Lu=5,  // Шаг нарезки винта укладчика 
    //  Ne=36, // Число (зубцов) енкодера
     // Fu,    // Частота шагов укладчика     Fu = Sue/Te
     // Tu,    // Время серии шагов укладчика Tu = Te
     // Te,    // Время между тиками енкодера
    //  Dp,    // Диаметр провода
      //Sue;   // Шагов (Оборотов *400) укладчика за тик енкодера
//Sue=Dp*Su/Ne/Lu; // Sue = Dp*2.2222  
//Fu = Sue/Te;
//Tu = Te;
                 // Шагов за тик енкодера = Шаг за Оборот * Диам провод / Шаг нарезки / Число енкодера




//----------------------------------------------------------------
// текст в регистр бегущей (ползущей) строкой и управляет режимом opMode.
void banner(const byte text[], byte EnterMode, byte EscMode, byte OtherMode )
{ int i;
  byte count=text[0]; 
  kbdo=kbd;
  for (i=1; i<=count && !(keyPress) ; i++){ 
    write4S(text,i); delay(500);
    kbdo=kbd;
    kbd=aKey();
  }
  if (keyPress) 
    switch (kbd){
      case aEnter: opMode=EnterMode; break; // Если нажали Enter
      case aEsc  : opMode=EscMode  ; break; // Если нажали Esc
      default    : opMode=OtherMode; break; // Если нажали другое
    } 
}

