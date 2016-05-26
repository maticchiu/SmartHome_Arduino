#include <DHT.h>
#include <SoftwareSerial.h>

#define DHT_PIN		2
#define DHT_TYPE	DHT11


SoftwareSerial Serial_1(10, 11);
const int pinButton = 52;
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
	// put your setup code here, to run once:
	Serial.begin(57600);
	Serial_1.begin(57600);
	
	pinMode(pinButton, INPUT);
	
	dht.begin();
	
	while(!Serial);
	while(!Serial_1);
}

#define UARTDATA_MAX 100

int iDataLen = 0;
byte abyUartData[UARTDATA_MAX];
unsigned int uiUartDataIndex_Start = 0;
unsigned int uiUartDataIndex_End = 0;

int iButtonStatus = 0;

unsigned int UartRcvData()
{
	iDataLen = Serial.available();
	
	if(iDataLen > 0)
	{
		for(int i = 0; i< iDataLen; i++)
		{
			uiUartDataIndex_End = (uiUartDataIndex_End + 1) % UARTDATA_MAX;
			if(uiUartDataIndex_Start == uiUartDataIndex_End)
				uiUartDataIndex_Start++;
			abyUartData[uiUartDataIndex_End] = Serial.read();
			
		}
	}
}

void loop() 
{
	UartRcvData();
	iButtonStatus = digitalRead(pinButton);
	if(iButtonStatus)
	{
		Serial.println("Detect");
		
	}

	float h = dht.readHumidity();
	float t = dht.readTemperature();

	Serial.print("Humidity: ");
	Serial.print(h);
	Serial.print(" %\t");
	Serial.print("Temperature: ");
	Serial.print(t);
	Serial.print(" *C \n");	
}

