#ifndef Seg7x4_h
#define Seg7x4_h

#include "Arduino.h"

//                  0   // pin 0 1  лучше не использовать они 
//                  1   //   зарезервированы под прямой сериальный ввод вывод процессора
#define stopPin     2   // ENA к драйверу шаговика СТОП
#define dirPin      3   // DIR к драйверу шаговика Направление
#define Pin4        4   // СВОБОДЕН <<<<<<<<<<<<<<<<<<<<<<<<<<
#define Pin5        5   // СВОБОДЕН <<<<<<<<<<<<<<<<<<<<<<<<<<
#define Pin6        6   // СВОБОДЕН <<<<<<<<<<<<<<<<<<<<<<<<<<
 
#define latchPin    12  // ST_CP Защелка индикатора 4х7сег
#define kbdPin      11   // линия данных с клавиатуры
#define kbLatchPin  10  // ST_CP Защелка регистра клавы
#define dataPin     9   // DS данные на индикатор и клаву
#define clockPin    8   // SH_CP тактовые импульсы индикатора и клавы
#define motorPin    7 // STEP к драйверу шаговика Серия
// Аналоговые пины можно использовать как цифровые
#define clk1Pin     A0   // енкодер импульс 1
#define clk2Pin     A1   // енкодер импульс 2   
#define wireRPin    A2   // положения провода 1
#define wireLPin    A3   // положения провода 2
#define termLPin    A4   // Концевик положения каретки 1
#define termRPin    A5   // Концевик положения каретки 2


//---- прочие Константы -------------
#define goRight  0    // Направление шагового двигателя право
#define goLeft   1    // Направление шагового двигателя лево
#define encRate  36   // Импульсов енкодера за оборот
#define SlowDown 900  // 900 Гц пониженная скорость укладчика
#define HurryUp  1100 // 1100 Гц повышенная скорость укладчика
#define JustFine 1000 // 1000 шагов в секунду нормальная скорость укладчика
//------------------  Названия режимов работы (состояний)
#define aColdStart   0
#define aDiamBanner  1
#define aDiamEntry   2
#define aVitkiBanner 3
#define aVitkiEntry  4
#define aReadyBanner 5
#define aRun         6 
#define aPause       7
#define aFinish      8
//------------------ Названия кнопок клавиатуры
#define aEnter      7
#define aEsc        2
#define aKbd1       3
#define aKbd2       4
#define aKbd3       5
#define aKbd4       6
//--------------------
#define keyPress    (kbd!=kbdo && kbd!=0)


void write4S(const byte buf[],byte j);// Произвольный набор бит в регистр из буфера
void regWrite( byte data, const byte latch);    // записывает одно знакоместо в регистр <<<<<<<
void writeStr(const byte text[]); //  текст в регистр бегущей (ползущей) строкой
void Raw4Write( long int data);  // выводит произвольный набор бит на 4х7 сегментный индикатор 	 
void Digits4Write( int data);   // выводит 10_ичное число на индикатор 4х7
void aBitWrite( byte data);    // записывает один бит в регистр индикаторов
byte aKey();                  // тройное сканирование клавиатуры
byte aKeyPressed();          // читаем номер нажатой кнопки
void Seg7x4init();          // инициализация клавиатуры и дисплея


//Знакогенератор PGFEDCBA     
const byte znak[]={
0x3F,   //    0  00111111   
0x06,   //    1  00000110   
0x5B,   //    2  01011011   
0x4F,   //    3  01001111   
0x66,   //    4  01100110   
0x6D,   //    5  01101101   
0x7D,   //    6  01111101   
0x07,   //    7  00000111   
0x7F,   //    8  01111111   
0x6F,   //    9  01101111   
0x77,   //    A  01110111   
0x7C,   //    b  01111100  
0x39,   //    C  00111001  
0x5E,   //    d  01011111   
0x79,   //    E  01111001  
0x71,   //    F  01110001  
0x7b,   //    e  01111011  
0x5F,   //    д  01011111   д
0x00,   //    _  00000000  
0x3E,   //    U  00111110  
0x1C,   //    u  00011100  
0x37,   //    П  00110111  
0x54,   //    n  01010100  
0x5C,   //    o  01011100    o low
0x63,   //    o  01100011    o high
0x73,   //    o  01110011    P
0x40,   //    -  01000000    just  -
0x08,   //    _  00001000    lower _
0x01,   //    -  00000001    upper -
0x41,   //    =  01000001  
0x76,   //    H  01110110  
0x74,   //    h  01110100  
0x30,   //    I  00110000  
0x38,   //    L  00111000  
0x18,   //    l  00011000    L low  
0x36,   //   ||  00110110  
0x31,   //    Г  00110001  
0x50,   //    r  01010000  
0x6E,   //    Y  01101110   
0x3D,   //    G  00111101  
0x67,   //    q  01100111  
0x0E,   //    J  00001110  
0x0C,   //    j  00001100    J low
0x44,}; //    7  01000100    7 low

                  //   9,       П    Р    И    В    Е    Т   
const byte tprivet[]={ 9,0,0,0,0x37,0x73,0x3e,0x7f,0x79,0x07,0,0,0};
                  //   7,       П   Y    C    K
const byte tpusk[]  ={ 7,0,0,0,0x37,0x6e,0x39,0xf2,0,0,0};
                  //   8        П    А    Y    3    A
const byte tpause[] ={ 8,0,0,0,0x37,0x77,0x6e,0x4f,0x77,0,0,0};
                  //  10        П    О    Е    х    А    Л    И
const byte tstart[] ={10,0,0,0,0x37,0x3f,0x79,0xd2,0x77,0x56,0x3e,0,0,0};
                  //  24        В    В    o    д    и        д    и    А    М    M    Е    Т   Р        П    Р    О    В    о    д    А
const byte tdiam[]  ={3,0,0,0,0x3f,0,0,0};
                  //  20        В    В    o    д    и        ч    и    с    л    о        b    и    T    k    о    b     
const byte tvitki[] ={6,0,0,0,0,0x06,0,0,0,0,0 };
                  //   7        О    Б    Е    д
const byte tobed[]  ={ 7,0,0,0,0x3f,0x7d,0x79,0x5f,0,0,0};

#endif


