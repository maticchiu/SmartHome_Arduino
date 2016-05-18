void setup() {
	// put your setup code here, to run once:
	Serial.begin(57600);
	while(!Serial);
}

#define UARTDATA_MAX 100

int iDataLen = 0;
byte abyUartData[UARTDATA_MAX];
unsigned int uiUartDataIndex_Start = 0;
unsigned int uiUartDataIndex_End = 0;

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
	
}

