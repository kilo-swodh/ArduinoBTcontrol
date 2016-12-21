#include<SoftwareSerial.h>
#define LED1 3
#define LED2 5
#define LED3 6
#define LED4 10
#define LED5 11

#define RATE_A 60
#define RATE_B 40
#define RATE_C 20
#define RATE_MAX 120

//杂项
int delayValue, startValue, endValue;
unsigned char rateLevel = 0;

//装开始值和结束值的String
String strS = "";
String strE = "";

//判断性参数
boolean isSec = false;
boolean isWhoS = false;
boolean isWhoF = false;

//是否进入闪烁
boolean isFlash1 = false;
boolean isFlash2 = false;
boolean isFlash3 = false;
boolean isFlash4 = false;
boolean isFlash5 = false;

//各灯的闪烁频率
unsigned char rate1;
unsigned char rate2;
unsigned char rate3;
unsigned char rate4;
unsigned char rate5;

//用于延迟闪烁
int tempValue1 = 0;
int tempValue2 = 0;
int tempValue3 = 0;
int tempValue4 = 0;
int tempValue5 = 0;

//暂存光照强度
int tempLED1 = 200;
int tempLED2 = 200;
int tempLED3 = 200;
int tempLED4 = 200;
int tempLED5 = 200;

//分时复用参数
const int everyPar = 15;
const int everyFlash = 70;
unsigned long lastPar = 0;
unsigned long lastFlash = 0;

boolean isFlashOn = true;
boolean isReadyRead = true;

SoftwareSerial bt(8, 9);  //RX,TX

/*****************  Author:Kilo  *********************/
void setup() {
  // put your setup code here, to run once:
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  delayValue = 30;
  Serial.begin(9600);
  bt.begin(38400);
  analogWrite(LED1, 200);
  analogWrite(LED2, 200);
  analogWrite(LED3, 200);
  analogWrite(LED4, 200);
  analogWrite(LED5, 200);
}

void loop() {
  // put your main code here, to run repeatedly:
  parseCmd();
  doFlash();
}

void parseCmd() {
  if (millis( ) - lastPar < everyPar)
    return;                 //时候未到
  lastPar = millis( );
  if (bt.available() > 0) {
    char ch = char(bt.read());
    switch (ch) {
      case '$':
        checkShade();
        break;
      case '%':
        checkFlash();
        break;
      case '~':
        checkReset();
        break;
      default:
        ch = char(bt.read());
        switch (ch) {
          case '$':
            checkShade();
            break;
          case '%':
            checkFlash();
            break;
          case '~':
            checkReset();
            break;
        }
        break;
    }
  }
}

void checkShade() {
  int Num;
  while (bt.available() > 0 && isReadyRead) {
    delay(20);
    while (isReadyRead) {
      char ch = char(bt.read());
      //Serial.print("ch is ");
      //Serial.println(ch);
      if (ch == '*') {
        isReadyRead = false;
        delay(20);
        break;
      }

      if (ch == ' ') {
        Serial.println("Packet loss in checkShade");
        continue;
      }
      if (ch == '$')
        continue;
      if (!isWhoS) {
        Num = int(ch);
        isWhoS = true;
        continue;
      }
      if (ch != '#' && ch != '&' && !isSec && isWhoS) {
        strS += ch;
        continue;
      } else if (ch == '&' && isWhoS) {
        isSec = true;
        continue;
      } else {
        if (ch != '*' && ch != '#')
          strE += ch;
        continue;
      }
    }

  }
  Serial.print("Shade: ");
  Serial.println(Num);
  if (strS != "" && strE != "") {
    startValue = strS.toInt() + 30;
    endValue = strE.toInt() + 30;
    strS = "";
    strE = "";
    isSec = false;
  }
  int LED = 0;
  switch (Num) {
    case 49:
      LED = LED1;
      break;
    case 50:
      LED = LED2;
      break;
    case 51:
      LED = LED3;
      break;
    case 52:
      LED = LED4;
      break;
    case 53:
      LED = LED5;
      break;
    default:
      break;
  }
  if (LED > 0 ) {
    switch (Num) {
      case 49:
        if (startValue != tempLED1)
          startValue = tempLED1;
        if (isFlash1)
          isFlash1 = false;
        break;
      case 50:
        if (startValue != tempLED2)
          startValue = tempLED2;
        if (isFlash2)
          isFlash2 = false;
        break;
      case 51:
        if (startValue != tempLED3)
          startValue = tempLED3;
        if (isFlash3)
          isFlash3 = false;
        break;
      case 52:
        if (startValue != tempLED4)
          startValue = tempLED4;
        if (isFlash4)
          isFlash4 = false;
        break;
      case 53:
        if (startValue != tempLED5)
          startValue = tempLED5;
        if (isFlash5)
          isFlash5 = false;
        break;
    }
    //Serial.print("startValue: ");
    //Serial.println(startValue);
    //Serial.print("endValue: ");
    //Serial.println(endValue);
    if (startValue > endValue) {        //是否是变暗
      if (startValue <= 100)
        doShade(LED, startValue * 2, endValue, true);
      else
        doShade(LED, startValue , endValue, true);
    }
    else {
      if (endValue <= 100)
        doShade(LED, startValue, endValue * 2, false);
      else
        doShade(LED, startValue, endValue, false);
    }
    isWhoS = false;
  }
}

void doShade(int pin, int s, int e, int vary) {
  int progress = s;
  boolean v;
  v = vary;
  while (true) {
    //Serial.print("progress: ");
    //Serial.println(progress);
    analogWrite(pin, progress);
    if (v) {
      progress -= 2;
      if (progress <= e) {
        break;
      }
    } else if (!v) {
      progress += 2;
      if (progress >= e) {
        break;
      }
    }
  }
  switch (pin) {
    case 3:
      tempLED1 = e;
      break;
    case 5:
      tempLED2 = e;
      break;
    case 6:
      tempLED3 = e;
      break;
    case 10:
      tempLED4 = e;
      break;
    case 11:
      tempLED5 = e;
      break;
  }
  isReadyRead = true;
  //Serial.print("finish pin: ");
  //Serial.println(pin);
}

void checkFlash() {
  int Num;
  while (bt.available() > 0) {
    char ch = char(bt.read());
    if (ch == "") {
      Serial.println("Packet loss in checkFlash");
      continue;
    }
    if (ch == '%')
      continue;
    if (!isWhoF) {
      Num = int(ch);
      isWhoF = true;
      continue;
    }
    if (ch == '*')
      break;
    boolean isTurnOn;
    if (ch == 'o')
      isTurnOn = false;
    else if (isWhoF) {
      isTurnOn = true;
      switch (ch) {
        case 'a':
          rateLevel = 1;
          break;
        case 'b':
          rateLevel = 2;
          break;
        case 'c':
          rateLevel = 3;
          break;
      }
    }
    if (isTurnOn && isWhoF && rateLevel != 0) {
      switch (Num) {
        case 49:
          isFlash1 = true;
          rate1 = rateLevel;
          break;
        case 50:
          isFlash2 = true;
          rate2 = rateLevel;
          break;
        case 51:
          isFlash3 = true;
          rate3 = rateLevel;
          break;
        case 52:
          isFlash4 = true;
          rate4 = rateLevel;
          break;
        case 53:
          isFlash5 = true;
          rate5 = rateLevel;
          break;
        default:
          break;
      }
      rateLevel = 0;
    } else {
      switch (Num) {
        case 49:
          isFlash1 = false;
          analogWrite(LED1, tempLED1);
          break;
        case 50:
          isFlash2 = false;
          analogWrite(LED2, tempLED2);
          break;
        case 51:
          isFlash3 = false;
          analogWrite(LED3, tempLED3);
          break;
        case 52:
          isFlash4 = false;
          analogWrite(LED4, tempLED4);
          break;
        case 53:
          isFlash5 = false;
          analogWrite(LED5, tempLED5);
          break;
        default:
          break;
      }
    }
  }
  isWhoF = false;
  Serial.print("Flash: ");
  Serial.println(Num);
}

void doFlash() {
  if (millis( ) - lastFlash < everyFlash)
    return;
  lastFlash = millis( );
  if (isFlashOn) {
    isFlashOn = false;
    if (isFlash1) {
      if (tempValue1 < RATE_MAX) {
        switch (rate1) {
          case 1:
            tempValue1 += RATE_A;
          case 2:
            tempValue1 += RATE_B;
          case 3:
            tempValue1 += RATE_C;
        }
      } else {
        digitalWrite(LED1, HIGH);
        tempValue1 = 0;
      }
    }
    if (isFlash2) {
      if (tempValue2 < RATE_MAX) {
        switch (rate2) {
          case 1:
            tempValue2 += RATE_A;
          case 2:
            tempValue2 += RATE_B;
          case 3:
            tempValue2 += RATE_C;
        }
      } else {
        digitalWrite(LED2, HIGH);
        tempValue2 = 0;
      }
    }
    if (isFlash3) {
      if (tempValue3 < RATE_MAX) {
        switch (rate3) {
          case 1:
            tempValue3 += RATE_A;
          case 2:
            tempValue3 += RATE_B;
          case 3:
            tempValue3 += RATE_C;
        }
      } else {
        digitalWrite(LED3, HIGH);
        tempValue3 = 0;
      }
    }
    if (isFlash4) {
      if (tempValue4 < RATE_MAX) {
        switch (rate4) {
          case 1:
            tempValue4 += RATE_A;
          case 2:
            tempValue4 += RATE_B;
          case 3:
            tempValue4 += RATE_C;
        }
      } else {
        digitalWrite(LED4, HIGH);
        tempValue4 = 0;
      }
    }
    if (isFlash5) {
      if (tempValue5 < RATE_MAX) {
        switch (rate5) {
          case 1:
            tempValue5 += RATE_A;
          case 2:
            tempValue5 += RATE_B;
          case 3:
            tempValue5 += RATE_C;
        }
      } else {
        digitalWrite(LED5, HIGH);
        tempValue5 = 0;
      }
    }
  } else {
    isFlashOn = true;
    if (isFlash1)
      digitalWrite(LED1, LOW);
    if (isFlash2)
      digitalWrite(LED2, LOW);
    if (isFlash3)
      digitalWrite(LED3, LOW);
    if (isFlash4)
      digitalWrite(LED4, LOW);
    if (isFlash5)
      digitalWrite(LED5, LOW);
  }
}

void checkReset(){
  if (bt.available() > 0){
    char ch = char(bt.read());
    if( ch == '~')
      doReset();
  }
}

void doReset(){
  Serial.println("doReset!");
  isFlash1 = false;
  isFlash2 = false;
  isFlash3 = false;
  isFlash4 = false;
  isFlash5 = false;
  analogWrite(LED1, 200);
  analogWrite(LED2, 200);
  analogWrite(LED3, 200);
  analogWrite(LED4, 200);
  analogWrite(LED5, 200);
  isFlashOn = true;
  isReadyRead = true;
  tempLED1 = 200;
  tempLED2 = 200;
  tempLED3 = 200;
  tempLED4 = 200;
  tempLED5 = 200;
  tempValue1 = 0;
  tempValue2 = 0;
  tempValue3 = 0;
  tempValue4 = 0;
  tempValue5 = 0;
  rate1 = rateLevel;
  rate2 = rateLevel;
  rate3 = rateLevel;
  rate4 = rateLevel;
  rate5 = rateLevel;
}
