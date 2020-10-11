#include <Arduino.h>

void AE_HX711_Init(void);
void AE_HX711_Reset(void);
void AE_HX711_Read(int);
void AE_HX711_Averaging(long adc,char num);
void AE_HX711_getGram(char num);

//---------------------------------------------------//
// ロードセル　シングルポイント（ ビーム型）　２０ｋＧ
// 赤E+
// 黒E-
// 白A-
// 緑A+
//---------------------------------------------------/
#define OUT_VOL   0.001f      //定格出力 [V]
#define LOAD      20000.0f    //定格容量 [g]

//---------------------------------------------------//
// ロードセル数
//---------------------------------------------------/
#define CELLS 4

//---------------------------------------------------//
// ピンの設定
//---------------------------------------------------/
int pin_dout[] =  {2,4,6,8};
int pin_slk[] =   {3,5,7,9};

//---------------------------------------------------//
// グローバル
//---------------------------------------------------/
float fgr_data[4];
float offset[4];
float result[4]; 

void setup() {
  char S1[20];
  char s[20];
  Serial.begin(9600);
  Serial.println("Mekanical keyboard 2020");
  //Serial.println("setup");
  AE_HX711_Init();
  AE_HX711_Reset();
  AE_HX711_getGram(30);
  for(int pin = 0 ; pin < CELLS ; pin++)
    offset[pin] = fgr_data[pin];
  /* オフセット表示
  Serial.println("offset_value");
  for(int pin = 0 ; pin < CELLS ; pin++){
    sprintf(S1,"No.%d %s g",pin,dtostrf((offset[pin]), 5, 3, s));
    Serial.println(S1);
  }
  */
}

void loop() 
{ 
  char S1[20];
  char s[20];
  //Serial.println("loop");
  AE_HX711_getGram(5);
  Serial.println("result");
  for(int pin = 0 ; pin < CELLS ; pin++){
    sprintf(S1,"No.%d %s g",pin,dtostrf((fgr_data[pin]-offset[pin]), 5, 3, s));
    Serial.println(S1);
  }
}

void AE_HX711_Init(void)
{
  //Serial.println("init");
  for(int pin = 0 ; pin < CELLS ; pin++)
    pinMode(pin_slk[pin], OUTPUT);
  for(int pin = 0 ; pin < CELLS ; pin++)
    pinMode(pin_dout[pin], INPUT);
}

void AE_HX711_Reset(void)
{
  //Serial.println("reset");
  for(int pin = 0 ; pin < CELLS ; pin++)
    digitalWrite(pin_slk[pin],1);  
  delayMicroseconds(100);
  for(int pin = 0 ; pin < CELLS ; pin++)
    digitalWrite(pin_slk[pin],0);
  
  delayMicroseconds(100); 
}

void AE_HX711_Read()
{
  long data[CELLS];
  for (int pin = 0; pin < CELLS; pin++) data[pin] = 0;
  //Serial.println("read");
  while(digitalRead(pin_dout[0])!=0)
    delayMicroseconds(10);
  for(int i=0;i<24;i++) {
    for (int pin = 0; pin< CELLS; pin++)
      digitalWrite(pin_slk[pin],1);
    delayMicroseconds(5);
    for (int pin = 0; pin < CELLS; pin ++)
      digitalWrite(pin_slk[pin ],0);
    delayMicroseconds(5);
    for (int pin = 0; pin < CELLS; pin++)
      data[pin] = (data[pin]<<1)|(digitalRead(pin_dout[pin]));
  }
  //Serial.println(data,HEX);
  for (int pin = 0; pin < CELLS; pin++)
    digitalWrite(pin_slk[pin],1);
  delayMicroseconds(10);
  for (int pin = 0; pin < CELLS; pin++)
    digitalWrite(pin_slk[pin],0);
  delayMicroseconds(10); 
  for (int pin = 0; pin < CELLS; pin++)
    result[pin] = data[pin]^0x800000; 
}

void AE_HX711_Averaging(char num)
{
  //Serial.println("averaging");
  long sum[CELLS];
  long result[CELLS];
  for (int pin = 0; pin < CELLS; pin++)
    sun[pin] = 0;
  for (int i = 0; i < num; i++) {
     AE_HX711_Read();
     for (int pin = 0; pin < CELLS; pin++) {
       sum[pin] += result[pin];
     }
  }
  for (int pin = 0; pin < CELLS; pin++)
    fgr_data[pin] = sun[pin] / num ;
}

void AE_HX711_getGram(char num)
{
  //Serial.println("getGram");
// Power Supply Options registor
  #define HX711_R1  20000.0f
  // Value Correction #define HX711_R2  8200.0f
  #define HX711_R2  3200.0f
// Reference bypass
  #define HX711_VBG 1.25f
  // Value Correction #define HX711_AVDD      4.2987f//(HX711_VBG*((HX711_R1+HX711_R2)/HX711_R2))
  #define HX711_AVDD      9.0625kf//(HX711_VBG*((HX711_R1+HX711_R2)/HX711_R2))
  #define HX711_ADC1bit   HX711_AVDD/16777216 //16777216=(2^24)
  #define HX711_PGA 128
  #define HX711_SCALE     (OUT_VOL * HX711_AVDD / LOAD *HX711_PGA)
  AE_HX711_Averaging(num);
  for(int pin = 0 ; pin < CELLS ; pin++){
    //Serial.println("getGram2");
    fgr_data[pin] = fgr_data[pin] * HX711_ADC1bit; 
    //Serial.println( HX711_AVDD);   
    //Serial.println( HX711_ADC1bit);   
    //Serial.println( HX711_SCALE);   
    //Serial.println( fgr_data);   
    fgr_data[pin] =  fgr_data[pin] / HX711_SCALE;
  }
}
