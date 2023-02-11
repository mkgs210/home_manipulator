/*коды!!!!!!!!!!
  1  866B20DF
  2  866B10EF
  3  866B40BF
  4  866BF00F
  5  866BA05F
  6  866B609F
  7  866B30CF
  8  866BB04F
  9  866B50AF
  0  866B8877
  ch+  866B906F
  ch-  866BE01F
  vol+ 866B708F
  vol- 866BC837
  <-   866B58A7
  ->   866BD827
  ^    866B18E7
  v    866B9867
 power 866BC23D


motor 3= 2579
motor 4= 2104



*/
#define ir1 0x866B20DF
#define ir2 0x866B10EF
#define ir3 0x866B40BF
#define ir4 0x866BF00F
#define ir5 0x866BA05F
#define ir6 0x866B609F
#define ir7 0x866B30CF
#define ir8 0x866BB04F
#define ir9 0x866B50AF
#define ir0 0x866B8877
#define power 0x866BC23D
#include <IRremote.h> // библиотека

#define del 10

//-----пины драйверов
#define in1 7
#define in2 8
#define in3 9
#define in4 10

#define del 3// от 5 до 50 скорость шаговиков
//-----пины переключения питания драйверов
#define pw1 2
#define pw2 3
#define pw3 4
#define pw4 5
#define pw5 6
//-----пины концевиков
#define sw2 13
#define sw3 11
#define sw4 12
//-------------
#define IR_PIN A5 //вход ИК приемника
IRrecv irrecv(IR_PIN);
decode_results results;
#define timer A4
unsigned long eventTime = 0;//eventTime, для паузы, вместо delay,будем использовать функцию millis.
//----------------работа через USB
const char StaPack = '#';                // Признак начала пакета данных
const char EndPack = '|';                // Признак окончания пакета данных
const uint16_t TimeOut = 500;            // Отвалились через это время mls
//----------------сколько максимум шагов
const uint16_t step1 = 500;//+++
const uint16_t step2 = 4000;//4000
const uint16_t step3 = 2579;//++++ 2640 то что считал Макс
const uint16_t step4 = 2104 ;//++++
const uint16_t step5 = 600;//+++
//-----------------значение которое сейчас
uint16_t defstep1 = 0;
uint16_t defstep2 = 0;
uint16_t defstep3 = 0;
uint16_t defstep4 = 0;
uint16_t defstep5 = 0;
//-----------------
#include <Stepper.h>
Stepper stepMot1(48, 7, 8, 9, 10); //биполярные 48 шагов на оборот ok
Stepper stepMot2(48, 7, 8, 9, 10); //биполярные 48 шагов на оборот
Stepper stepMot3(48, 7, 8, 9, 10); //биполярные 48 шагов на оборот
Stepper stepMot4(48, 7, 8, 9, 10); //униполярные 48 шагов на оборот
Stepper stepMot5(48, 7, 8, 9, 10); //униполярные 48 шагов на оборот200

int i;

void setup() {
  irrecv.enableIRIn(); // включить приёмник
  stepMot1.setSpeed(30); //установка скорости вращения ротора ok 150
  stepMot2.setSpeed(300); //установка скорости вращения ротора 600
  stepMot3.setSpeed(200); //установка скорости вращения ротора
  stepMot4.setSpeed(300); //установка скорости вращения ротора
  stepMot5.setSpeed(50); //установка скорости вращения ротора
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(pw1, OUTPUT);
  pinMode(pw2, OUTPUT);
  pinMode(pw3, OUTPUT);
  pinMode(pw4, OUTPUT);
  pinMode(pw5, OUTPUT);



  pinMode(sw2, INPUT_PULLUP);
  pinMode(sw3, INPUT_PULLUP);
  pinMode(sw4, INPUT_PULLUP);

  pinMode(timer, OUTPUT);
  Serial.begin(9600);
  Serial.println("Hi");
  
  //***********************

  //void zetNol();

  //***********************
  
  //----------------------------модуль реле инвертированны входы кроме 5го
  digitalWrite(pw1, HIGH);
  digitalWrite(pw2, HIGH);
  digitalWrite(pw3, HIGH);
  digitalWrite(pw4, HIGH);
  digitalWrite(pw5, HIGH);
}
byte hexToByte (String StrControlHex) {         //0 : 255 (1 байт)

  uint8_t  HEX16 = 0;   // число 16-е из символа
  uint8_t  exp16 = 1;   // степень числа 16
  uint8_t  decBy = 0;   // число 10-е расчитанное без знака

  StrControlHex.remove(0, 2);           //отрезаем управляющие символы (2шт)
  int i = StrControlHex.indexOf('|');   //определяем дилнну строки
  if (i == -1) return 0;                //фигня пришла а не байт
  if (i > 2)   return 0;                //фигня пришла а не байт
  StrControlHex.remove(i, 1);           //отрезаем управляющие символы ('|')

  for (int j = StrControlHex.length() - 1; j >= 0; j--) {

    HEX16 = StrControlHex.charAt(j);

    if (HEX16 >= 48 && HEX16 <= 57) HEX16 = map(HEX16, 48, 57, 0, 9);
    if (HEX16 >= 65 && HEX16 <= 70) HEX16 = map(HEX16, 65, 70, 10, 15);
    if (HEX16 >= 97 && HEX16 <= 102) HEX16 = map(HEX16, 97, 102, 10, 15);

    decBy = decBy + HEX16 * exp16;
    exp16 = exp16 * 16;

  }
  return decBy;                         //возвращаем десятичное число 1 байт без знака
}
int16_t hexToInt16 (String StrControlHex) {      // -32768 : 32767 (2 байта)

  uint8_t  HEX16 = 0;    // число 16-е из символа
  uint16_t exp16 = 1;    // степень числа 16
  int16_t  dec16 = 0;    // число 10-е расчитанное со знаком

  StrControlHex.remove(0, 2);           //отрезаем управляющие символы (2шт)
  int i = StrControlHex.indexOf('|');   //определяем дилнну строки
  if (i == -1) return 0;                //фигня пришла а не 2 байта
  if (i > 4)   return 0;                //фигня пришла а не 2 байта
  StrControlHex.remove(i, 1);           //отрезаем управляющие символы ('|')

  for (int j = StrControlHex.length() - 1; j >= 0; j--) {

    HEX16 = StrControlHex.charAt(j);

    if (HEX16 >= 48 && HEX16 <= 57) HEX16 = map(HEX16, 48, 57, 0, 9);
    if (HEX16 >= 65 && HEX16 <= 70) HEX16 = map(HEX16, 65, 70, 10, 15);
    if (HEX16 >= 97 && HEX16 <= 102) HEX16 = map(HEX16, 97, 102, 10, 15);

    dec16 = dec16 + HEX16 * exp16;
    exp16 = exp16 * 16;
  }
  return dec16;                         //возвращаем десятичное число int16_t со знаком
  }
  int32_t hexToInt32 (String StrControlHex) {      // -2147483648 : 2147483647 (4 байта)

  uint8_t  HEX16 = 0;    // число 16-е из символа
  uint32_t exp16 = 1;    // степень числа 16
  int32_t  dec32 = 0;    // число 10-е расчитанное со знаком

  StrControlHex.remove(0, 2);           //отрезаем управляющие символы (2шт)
  int i = StrControlHex.indexOf('|');   //определяем дилнну строки
  if (i == -1) return 0;               //фигня пришла а не 4 байта
  if (i > 8)   return 0;               //фигня пришла а не 4 байта
  StrControlHex.remove(i, 1);          //отрезаем управляющие символы ('|')

  for (int j = StrControlHex.length() - 1; j >= 0; j--) {

    HEX16 = StrControlHex.charAt(j);

    if (HEX16 >= 48 && HEX16 <= 57) HEX16 = map(HEX16, 48, 57, 0, 9);
    if (HEX16 >= 65 && HEX16 <= 70) HEX16 = map(HEX16, 65, 70, 10, 15);
    if (HEX16 >= 97 && HEX16 <= 102) HEX16 = map(HEX16, 97, 102, 10, 15);

    dec32 = dec32 + HEX16 * exp16;
    exp16 = exp16 * 16;
  }
  return dec32;                         //возвращаем десятичное число int32_t со знаком
  }
void left() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(del);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(del);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(del);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(del);
}
void right() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(del);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(del);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(del);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(del);
}
void zetNol() {
  //--------------гоним в 0
  Serial.println("Zet 0 ready");
  uint16_t j = 0;
  /*digitalWrite(pw2, LOW);
  delay(10);
  delay(del);
  while (digitalRead(sw2) == LOW) {
    stepMot2.step(-1);
    j++;
    
  }
  digitalWrite(pw2, HIGH);
  Serial.print("motor 2= ");
  Serial.println(j);
  //----------------------
  delay(500);
*/


  //--------------гоним в 0
  delay(del);
  digitalWrite(pw3, HIGH);
    delay(del);
  while (digitalRead(sw3) == HIGH) {
    stepMot3.step(1);
    j++;
  }
  digitalWrite(pw3, LOW);
  Serial.print("motor 3= ");
  Serial.println(j);
  //----------------------
  delay(500);


  j = 0;
  //--------------гоним в 0
  digitalWrite(pw4, HIGH);
  delay(del);
  while (digitalRead(sw4) == HIGH) {
    stepMot1.step(-1);
    j++;
  }
  digitalWrite(pw4, LOW);
  Serial.print("motor 4= ");
  Serial.println(j);
  //----------------------
  delay(500);

}

void IK() {
  if (digitalRead(IR_PIN) == LOW) { // Если обнаружен отрицательный сигнал запускаем таймер,и включаем таймерПИН //(timer, HIGH);
    eventTime = millis(), digitalWrite(timer, HIGH);
  }

  // Продолжительность работы таймера >100 миллисекунд. И ТаймерПИН выключаем //(timer, LOW)
  if (millis() - eventTime > 100) {
    digitalWrite(timer, LOW);

    
  }

  if (irrecv.decode(&results)) { //Если обнаружен сигнал с датчика
    if (digitalRead(timer) == HIGH) {
        if (results.value == ir7) {//влево
        //if(defstep1<=170)moveMotor(1,defstep1 +=100);всос////////////
        digitalWrite(pw1, LOW);
        stepMot1.step(-48);
        delay(50);
        digitalWrite(pw1, HIGH);
        Serial.println("motor1 right");
      }
        if (results.value == ir9) {//вправо
        //if(defstep1<=170)moveMotor(1,defstep1 +=100);всос////////////
        digitalWrite(pw1, LOW);
        stepMot1.step(48);
        delay(50);
        digitalWrite(pw1, HIGH);
        Serial.println("motor1 right");
      }
      /*
      if (results.value == ir0) {
        //if(defstep1<=170)moveMotor(1,defstep1 +=100);всос////////////
        digitalWrite(pw2, LOW);
        stepMot2.step(1000);
        delay(50);
        digitalWrite(pw2, HIGH);
        Serial.println("motor1 right");
      }
      if (results.value == ir8) {
        digitalWrite(pw2, LOW);
        stepMot2.step(-1000);
        delay(50);
        digitalWrite(pw2, HIGH);
      }
     */
      
      if (results.value == ir3) {//вверх
        digitalWrite(pw3, LOW);
        stepMot3.step(-240);
        delay(50);
        digitalWrite(pw3, HIGH);
        Serial.println("motor1 right");
      }
      
        if (results.value == ir6) {//вниз
        digitalWrite(pw3, LOW);
        stepMot3.step(240);
        delay(50);
        digitalWrite(pw3, HIGH);
        Serial.println("motor1 right");
      }

      if (results.value == ir5) {//вверх
        digitalWrite(pw4, LOW);
        stepMot4.step(-120);
        delay(50);
        digitalWrite(pw4, HIGH);
        Serial.println("motor1 right");
      }
      
        if (results.value == ir2) {//вниз
        digitalWrite(pw4, LOW);
        stepMot4.step(120);
        delay(50);
        digitalWrite(pw4, HIGH);
        Serial.println("motor1 right");
      }

      if (results.value == ir1) {//зажим
        digitalWrite(pw5, LOW);
        stepMot5.step(-108);
        delay(50);
        digitalWrite(pw5, HIGH);
        Serial.println("motor1 right");
      }
      
        if (results.value == ir4) {//отжим
        digitalWrite(pw5, LOW);
        stepMot5.step(108);
        delay(50);
        digitalWrite(pw5, HIGH);
        Serial.println("motor1 right");
      }

      if (results.value ==  power) {// 
        zetNol();
      }
      
    }
    Serial.println(results.value, HEX);
    irrecv.resume(); //Принимаем новый сигнал
  }
}
void ContrUSB() {
clearPack:                                // переходим сюда если приняли мусор /отвалились по таймауту/

  char   IncomChar;
  String StrControl = "";

  while (Serial.available() > 0) {        // выгребаем все байты которые пришли в буфер Serial
    IncomChar = Serial.read();
    if (IncomChar == StaPack) {             // пришел символ начала посылки данных
      StrControl += IncomChar;              // плюсуем пришедший символ к строке
      unsigned long currentTime = millis(); // записываем текущее время
ReceptionPacket:                      // начало приема пакета
      if (Serial.available() > 0) {
        IncomChar = Serial.read();
        StrControl += IncomChar;            // плюсуем пришедший символ к строке
        if (IncomChar == EndPack) break;    // вываливаемся из цикла выгребания байт если приняли весь пакет
      }
      if (millis() - currentTime >= TimeOut) goto clearPack;  // отвалились по TimeOut
      goto ReceptionPacket;             // переход в начало приема пакета
    }
  }                                     // конец цикла по выгребанию байт

  if (StrControl != "") {
    Serial.println(StrControl);         // печатаем принятый пакет отладка
    switch (StrControl.charAt(1)) {       // применяем полученую строку
      /*case 'w':
        digitalWrite(13, 0);
        break;
      case 'x':
        digitalWrite(13, 1);
        break;*/
      /*case 'A':
        moveMotor(1, (map(hexToByte(StrControl), 0, 250, 0, 500)));
        
        break;
      case 'B':
        moveMotor(2, (map(hexToByte(StrControl), 0, 250, 0, 2000)));

        break;
      case 'C':
        moveMotor(3, map(hexToInt32(StrControl), 0, 250, 0, 500));

        break;
      case 'D':
        //moveMotor(4, (map(hexToByte(StrControl), 0, 250, 0, 500)));
        digitalWrite(pw4, LOW);
      delay(del);
        stepMot4.step(map(hexToInt32(StrControl), 0, 250, 0, 500));
        digitalWrite(pw1, HIGH);//выкл*/

        break;
      case 'Z'://влево основание
         digitalWrite(pw1, LOW);
        stepMot1.step(-48);
        delay(50);
        digitalWrite(pw1, HIGH);
        break;
        
        case 'X'://вправо основание
         digitalWrite(pw1, LOW);
        stepMot1.step(48);
        delay(50);
        digitalWrite(pw1, HIGH);
        break;



        case 'V'://вверх 2 колено 
        digitalWrite(pw3, LOW);
        stepMot3.step(-240);
        delay(50);
        digitalWrite(pw3, HIGH);
        break;

        case 'B'://вниз 2 колено 
        digitalWrite(pw3, LOW);
        stepMot3.step(240);
        delay(50);
        digitalWrite(pw3, HIGH);
        break;

        case 'M'://вверх 3 колено 
        digitalWrite(pw4, LOW);
        stepMot4.step(-120);
        delay(50);
        digitalWrite(pw4, HIGH);
        break;

        case 'N'://вниз 3 колено 
        digitalWrite(pw4, LOW);
        stepMot4.step(120);
        delay(50);
        digitalWrite(pw4, HIGH);
        break;

        case 'K'://вверх захват зажим
        digitalWrite(pw5, LOW);
        stepMot5.step(-105);
        delay(50);
        digitalWrite(pw5, HIGH);
        break;

        case 'L'://вниз 4 колено отжим
        digitalWrite(pw5, LOW);
        stepMot5.step(107);
        delay(50);
        digitalWrite(pw5, HIGH);
        break;

        
        
    }
  }
}
void test() {
  digitalWrite(pw1, LOW);
  stepMot1.step(200);
  delay(1000);
  stepMot1.step(-200);
  delay(1000);
  digitalWrite(pw1, HIGH);//выкл

  digitalWrite(pw2, LOW);
  stepMot2.step(200);
  delay(1000);
  stepMot2.step(-200);
  delay(1000);
  digitalWrite(pw2, HIGH);//выкл

  digitalWrite(pw3, LOW);
  stepMot3.step(200);
  delay(1000);
  stepMot3.step(-200);
  delay(1000);
  digitalWrite(pw3, HIGH);//выкл

  digitalWrite(pw4, LOW);
  stepMot4.step(200);
  delay(1000);
  stepMot4.step(-200);
  delay(1000);
  digitalWrite(pw4, HIGH);//выкл

  digitalWrite(pw5, LOW);
  stepMot5.step(200);
  delay(1000);
  stepMot5.step(-200);
  delay(1000);
  digitalWrite(pw5, HIGH);
}
void moveMotor(byte NumbMot, uint16_t Poloj) {

  //какой мотор     положение куда хотим увести его
  switch (NumbMot) {       // применяем полученую строку
    case 1:
      digitalWrite(pw1, LOW);
      delay(del);
      while (defstep1 != Poloj  and defstep1 <= step1) {
        if (Poloj < defstep1) stepMot1.step(-1);
        else stepMot1.step(1);
        defstep1 = Poloj;
      }
      digitalWrite(pw1, HIGH);
      break;

    case 2://шприц
      digitalWrite(pw2, LOW);
      delay(del);
      while (defstep2 != Poloj and /*digitalRead(sw2) == LOW and*/ defstep2 <= step2) {
        if (Poloj < defstep2) stepMot2.step(-1);
        else stepMot2.step(1);
        defstep2 = Poloj;
      }
      digitalWrite(pw2, HIGH);
      break;

    case 3:
      digitalWrite(pw3, LOW);
      delay(del);
      while (defstep3 != Poloj /*and digitalRead(sw3) == HIGH*/ and defstep3 <= step3) {
        if (Poloj < defstep3) stepMot3.step(-1);
        else stepMot3.step(1);
        defstep3 = Poloj;
      }
      digitalWrite(pw3, HIGH);
      break;

    case 4:
      digitalWrite(pw4, LOW);
      delay(del);
      while (defstep4 != Poloj /*and digitalRead(sw4) == HIGH*/ and defstep4 <= step4) {
        if (Poloj < defstep4) stepMot4.step(-1);
        else stepMot4.step(1);
        defstep4 = Poloj;
      }
      digitalWrite(pw4, HIGH);
      break;

    case 5:
      digitalWrite(pw5, LOW);
      delay(del);
      while (defstep5 != Poloj  and defstep5 <= step5) {
        if (Poloj < defstep5) stepMot1.step(-1);
        else stepMot5.step(1);
        defstep5 = Poloj;
      }
      digitalWrite(pw5, HIGH);
      break;
  }
}
void loop() {
  IK();
  ContrUSB();
  //test();
  if (digitalRead(sw2) != LOW)Serial.println("шприц"); //ограничивает высос
  if (digitalRead(sw3) != HIGH)Serial.println("sw3");
  if (digitalRead(sw4) != HIGH)Serial.println("sw4");


}
