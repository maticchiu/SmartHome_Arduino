#include <DHT.h>

#include <Event.h>
#include <Timer.h>

#include <IRremote.h>
#include <IRremoteInt.h>
#include <SoftwareSerial.h>

enum {
  SEVENSEG_NONE,
  SEVENSEG_0001,
  SEVENSEG_0010,
  SEVENSEG_0100,
  SEVENSEG_1000
};


#define IR_RCV_PIN		41
#define BUTTON_PIN  	39
#define UARTDATA_MAX  100
#define DHT_PIN		  	2
#define DHT_TYPE	  	DHT11

#define PIN_SEVENSEG_1000	22
#define PIN_SEVENSEG_0100	23
#define PIN_SEVENSEG_0010	24
#define PIN_SEVENSEG_0001	25
#define PIN_SEVENSEG_A		38
#define PIN_SEVENSEG_B		40
#define PIN_SEVENSEG_C		42
#define PIN_SEVENSEG_D		44
#define PIN_SEVENSEG_E		46
#define PIN_SEVENSEG_F		48
#define PIN_SEVENSEG_G		50
#define PIN_SEVENSEG_H		52


SoftwareSerial Serial_1(10, 11);


IRrecv irRcv(IR_RCV_PIN);
decode_results irRcvCode;
IRsend irSend;
DHT dht(DHT_PIN, DHT_TYPE);
Timer t;
int timer_DHT;
int timer_SevenSeg;

int iDataLen = 0;
byte abyUartData[UARTDATA_MAX];
unsigned int uiUartDataIndex_Start = 0;
unsigned int uiUartDataIndex_End = 0;
int iButtonStatus = 0;
unsigned int uiHumidity = 0;
unsigned int uiTemperature = 0;

unsigned int auiIrRaw[] = {3550, 3200, 1000, 2350, 1000, 700, 1000, 650, 1050, 2300, 1000, 700, 1000, 700, 1000, 700, 1000, 2350, 1000, 650, 1050, 2300, 1000, 2350, 1000, 700, 1000, 2350, 1000, 2300, 1000, 2350, 1000, 700, 1000, 700, 1000, 650, 1000, 2350, 1000, 700, 1000, 700, 1000, 650, 1050, 2300, 1000, 2350, 1000, 2350, 1000, 2350, 1000, 650, 1000, 2350, 1000, 2350, 1050, 2300, 1000, 700, 1000, 650, 3550, 3250, 1000};
byte seven_seg_digits[10][7] = { { 1, 1, 1, 1, 1, 1, 0 }, // = 0
  { 0, 1, 1, 0, 0, 0, 0 }, // = 1
  { 1, 1, 0, 1, 1, 0, 1 }, // = 2
  { 1, 1, 1, 1, 0, 0, 1 }, // = 3
  { 0, 1, 1, 0, 0, 1, 1 }, // = 4
  { 1, 0, 1, 1, 0, 1, 1 }, // = 5
  { 1, 0, 1, 1, 1, 1, 1 }, // = 6
  { 1, 1, 1, 0, 0, 0, 0 }, // = 7
  { 1, 1, 1, 1, 1, 1, 1 }, // = 8
  { 1, 1, 1, 0, 0, 1, 1 } // = 9
};


unsigned int UartRcvData()
{
  iDataLen = Serial.available();

  if (iDataLen > 0)
  {
    for (int i = 0; i < iDataLen; i++)
    {
      uiUartDataIndex_End = (uiUartDataIndex_End + 1) % UARTDATA_MAX;
      if (uiUartDataIndex_Start == uiUartDataIndex_End)
        uiUartDataIndex_Start++;
      abyUartData[uiUartDataIndex_End] = Serial.read();

    }
  }
}

void DHT_Read()
{
  uiHumidity = dht.readHumidity();
  uiTemperature = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(uiHumidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(uiTemperature);
  Serial.print(" *C \n");
}

void IR_Rcv()
{
  if (irRcv.decode(&irRcvCode))
  {
    Serial.print("results value is ");
    Serial.print(irRcvCode.value, HEX);
    Serial.print(", bits is ");
    Serial.print(irRcvCode.bits);
    Serial.print(", decode_type is ");
    Serial.println(irRcvCode.decode_type);

    for (int i = 1; i < irRcvCode.rawlen; i++) {
      Serial.print(irRcvCode.rawbuf[i]*USECPERTICK, DEC);
      Serial.print(" ");
    }
    Serial.println(" ");
    irRcv.resume();
  }
}

void IR_Send()
{
  //  iButtonStatus = digitalRead(BUTTON_PIN);
  //  if (iButtonStatus)
  {
    Serial.println("Button down");
    // irSend.sendRaw(auiIrRaw, sizeof(auiIrRaw) / sizeof(auiIrRaw[0]), 38);
    irSend.sendNEC(0x906FA25D, 32);
    delay(5000);
  }
}

void SevenSeg_ValueSet(unsigned int uiValue)
{
  unsigned int auiSeg[] = {PIN_SEVENSEG_A, PIN_SEVENSEG_B, PIN_SEVENSEG_C, PIN_SEVENSEG_D,
                           PIN_SEVENSEG_E, PIN_SEVENSEG_F, PIN_SEVENSEG_G
                          };

  for (unsigned int uiIndex = 0; uiIndex < 7; uiIndex++)
  {
    digitalWrite(auiSeg[uiIndex], !seven_seg_digits[uiValue][uiIndex]);
  }
}

void SevenSeg_DigitSet(unsigned int uiDigit)
{
  unsigned int uiSeg[] = {0, PIN_SEVENSEG_0001, PIN_SEVENSEG_0010, PIN_SEVENSEG_0100, PIN_SEVENSEG_1000};
  for (unsigned int uiIndex = SEVENSEG_0001; uiIndex <= SEVENSEG_1000; uiIndex++)
  {
    if (uiDigit == uiIndex)
      digitalWrite(uiSeg[uiIndex], LOW);
    else
      digitalWrite(uiSeg[uiIndex], HIGH);
  }
}

void SevenSeg_4Set(unsigned int uiDigit, unsigned int uiValue)
{
  SevenSeg_DigitSet(SEVENSEG_NONE);
  SevenSeg_ValueSet(uiValue);
  SevenSeg_DigitSet(uiDigit);
}

void Loop_DHT()
{
  DHT_Read();
}

void Loop_SevenSeg()
{
  //  SevenSeg_4Set(SEVENSEG_1000, (unsigned int)uiHumidity/10);
  //  SevenSeg_4Set(SEVENSEG_0100, (unsigned int)uiHumidity%10);
  //  SevenSeg_4Set(SEVENSEG_0010, (unsigned int)uiTemperature/10);
  //  SevenSeg_4Set(SEVENSEG_0001, (unsigned int)uiTemperature%10);
}

void setup()
{

  Serial.begin(57600);
  while (!Serial);
  Serial_1.begin(57600);
  while (!Serial_1);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(PIN_SEVENSEG_0001, OUTPUT);
  digitalWrite(PIN_SEVENSEG_0001, HIGH);
  pinMode(PIN_SEVENSEG_0010, OUTPUT);
  digitalWrite(PIN_SEVENSEG_0010, HIGH);
  pinMode(PIN_SEVENSEG_0100, OUTPUT);
  digitalWrite(PIN_SEVENSEG_0100, HIGH);
  pinMode(PIN_SEVENSEG_1000, OUTPUT);
  digitalWrite(PIN_SEVENSEG_1000, HIGH);

  pinMode(PIN_SEVENSEG_A, OUTPUT);
  digitalWrite(PIN_SEVENSEG_A, HIGH);
  pinMode(PIN_SEVENSEG_B, OUTPUT);
  digitalWrite(PIN_SEVENSEG_B, HIGH);
  pinMode(PIN_SEVENSEG_C, OUTPUT);
  digitalWrite(PIN_SEVENSEG_C, HIGH);
  pinMode(PIN_SEVENSEG_D, OUTPUT);
  digitalWrite(PIN_SEVENSEG_D, HIGH);
  pinMode(PIN_SEVENSEG_E, OUTPUT);
  digitalWrite(PIN_SEVENSEG_E, HIGH);
  pinMode(PIN_SEVENSEG_F, OUTPUT);
  digitalWrite(PIN_SEVENSEG_F, HIGH);
  pinMode(PIN_SEVENSEG_G, OUTPUT);
  digitalWrite(PIN_SEVENSEG_G, HIGH);
  pinMode(PIN_SEVENSEG_H, OUTPUT);
  digitalWrite(PIN_SEVENSEG_H, HIGH);

  dht.begin();

  irRcv.enableIRIn();

  timer_DHT = t.every(2000, Loop_DHT);
  timer_SevenSeg = t.every(10, Loop_SevenSeg);
}

void loop()
{
#if 0
  UartRcvData();
  iButtonStatus = digitalRead(pinButton);
  if (iButtonStatus)
  {
    Serial.println("Detect");
  }

#endif
  //  SevenSeg_4Set(SEVENSEG_1000, 1);
  //  SevenSeg_4Set(SEVENSEG_0100, 2);
  //  SevenSeg_4Set(SEVENSEG_0010, 6);
  //  SevenSeg_4Set(SEVENSEG_0001, 4);
  //  IR_Rcv();

  //    IR_Send();
  SevenSeg_DigitSet(SEVENSEG_NONE);
  t.update();
  SevenSeg_4Set(SEVENSEG_1000, (unsigned int)uiHumidity / 10);
  SevenSeg_4Set(SEVENSEG_0100, (unsigned int)uiHumidity % 10);
  SevenSeg_4Set(SEVENSEG_0010, (unsigned int)uiTemperature / 10);
  SevenSeg_4Set(SEVENSEG_0001, (unsigned int)uiTemperature % 10);
}

