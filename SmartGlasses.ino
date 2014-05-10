// Biblioteca para uso do Receptor RF
#include <VirtualWire.h>
#define NBEACONS 59

const int clockPin = 6;  // the pin number of the clock pin
const int dataPin = 9;   // the pin number of the dataPin pin
const int resetPin = 3;  // the pin number of the reset pin

const unsigned int VOLUME_0 = 0xFFF0;
const unsigned int VOLUME_1 = 0xFFF1;
const unsigned int VOLUME_2 = 0xFFF2;
const unsigned int VOLUME_3 = 0xFFF3;
const unsigned int VOLUME_4 = 0xFFF4;
const unsigned int VOLUME_5 = 0xFFF5;
const unsigned int VOLUME_6 = 0xFFF6;
const unsigned int VOLUME_7 = 0xFFF7;

int beacons[] = {
  0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,  //15
  0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 
  0x0035, 0x0035, 0x0035, 0x0035, 0x0035, // ARQUIVOS NO INTERVALO
  0x001B, 0x001C, 0x001D, 0x001E, 0x001F, 0x0020,  
  0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,  
  0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0X0035}; //59

int delays[]  = {
  20000, 18000, 18000, 18000, 16000, 22000, 10000, 18000, 21000, 10000, 13000, 18000, 18000, 17000, 15000, 21000,
  21000, 13000, 13000, 20000, 17000, 14000, 16000, 22000, 16000, 11000, 9000, 9000, 9000, 9000, 9000,
  13000, 18000, 17000, 19000, 16000, 17000, 17000,
  21000, 10000, 17000, 21000,  9000, 17000, 14000, 17000, 18000, 16000, 23000, 13000, 13000, 20000, 16000, 13000,
  14000, 22000, 17000, 12000, 12000, 9000}; //59 

char * landmarks[] = {
  "Ida Sala 117c", //1
  "Sala 117b", //2
  "Sala 117a", //3
  "Sala 115",  //4
  "Sala 113",  //5
  "Sala 111",  //6
  "Sala 109",  //7
  "Sala 107",  //8
  "Sala 105",  //9
  "Sala 103",  //10
  "Lojinha",   //11
  "Troféus",   //12
  "Entrada / NUTAP",  //13     
  "Sala 128",  //14
  "Sala 130",  //15
  "Sala 132",  //16
  "Sala 134",  //17
  "Sala 136-DMR", //18 
  "Baixa Visão-138", //19
  "Sala 140\nTéc. Cirúrgicas", //20
  "", //21
  "Sala 142",       //22
  "Sala 144-S.Social", //23
  "",  //24
  "Sala 146",   //25
  "Sala 146",   //26
  "Salas 148 e 150", //27
  "","","","",""
    "Sala 133",  //1
  "",          //1
  "Sala 135",  //3
  "Sala 137",  //4
  "Sala 139",  //5
  "",          //6
  "Sala 141",  //7
  "Sala 143",  //8
  "Sala 145",  //9
  "",          //10
  "Sala 149",  //11
  "Escada",    //12
  "",          //13
  "DOA",       //14
  "Psicologia",//15
  "Sala 106",  //16
  "Secretaria",//17
  "Sala 110",  //18  
  "Baixa Visão-112", //19
  "Baixa Visão-114", //20 
  "Sala 116",        //21
  "Sala 120",   //22
  "Sala 122",    //23
  "Sala 124",    //24  
  "Sala 126-DPMO", //25
  "", //26
  "" //27
}; 
int beaconOcurrences[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  //30
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //60

int debugPin  = 5; // When pin 5 LOW the Arduino module is in "DEBUG" mode and print "verbose" messages to the serial port
int debugMode = 1; // Default MODE starts OFF

const unsigned int PLAY_PAUSE = 0xFFFE;
const unsigned int STOP = 0xFFFF;
long lastBeaconChecking;
boolean tocou = false;

int lastBeaconDetected = -1;

void setup() {
  Serial.begin(9600);

  vw_setup(2000);   // Defines the BaudRate
  vw_rx_start();    // Starts to "listening" the air

  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  delay(100);

  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, LOW);

  delay(100);

  // reset the module
  digitalWrite(resetPin, HIGH);
  delay(100);
  digitalWrite(resetPin, LOW);
  delay(10);
  digitalWrite(resetPin, HIGH);
  delay(600);

  sendCommand(VOLUME_7);
  lastBeaconChecking = millis();
}

void loop() {
  byte buf[VW_MAX_MESSAGE_LEN];
  byte buflen = VW_MAX_MESSAGE_LEN;  
  if( vw_get_message(buf, &buflen) )
  {
    if (debugMode == 1) {
      Serial.print("Mensagem recebida via radio (entre colchetes): [");
      for (int j=0; j < buflen; j++) {
        Serial.print(buf[j]);
      }
      Serial.println("] ");
    }
    int beacon = buf[0]-65;
    if (beacon < 0 || beacon >59) {
      if (debugMode == 1) {
        Serial.print("invalid beacon detected:  numero "); 
        Serial.println(beacon);
      }  

    } 
    else {
      if (debugMode == 1) {
        Serial.print("beacon detected: "); 
        Serial.println(landmarks[beacon]);
        beaconOcurrences[beacon]++;
      }
      // each 500 milliseconds it checks what's the most frequent beacon
      if ((millis()-lastBeaconChecking) > 500) {
        int maxVal = 0;
        int maxIndex = 0;
        for(int k = 0; k < NBEACONS; k++) {
          if(beaconOcurrences[k] > maxVal) {
            maxVal = beaconOcurrences[k];
            maxIndex = k;
          }
          beaconOcurrences[k] = 0;
        }
        if (lastBeaconDetected != maxIndex) {
          sendCommand(beacons[maxIndex]);
          delay(delays[maxIndex]);
          sendCommand(STOP);
          delay(250);
          lastBeaconChecking = millis();
          lastBeaconDetected = maxIndex;
        }
      }  
    }
  }


  /*
  Serial.println("Arquivo 1");
   sendCommand(0x0001);
   delay(20000);
   sendCommand(STOP);
   
   */
  delay(5);
}

void sendCommand(int addr) {
  digitalWrite(clockPin, LOW);
  delay(2);
  for (int i=15; i>=0; i--)
  { 
    delayMicroseconds(50);
    if((addr>>i)&0x0001 >0)
    {
      digitalWrite(dataPin, HIGH);
      //Serial.print(1);
    }
    else
    {
      digitalWrite(dataPin, LOW);
      // Serial.print(0);
    }
    delayMicroseconds(50);
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(50);

    if(i>0)
      digitalWrite(dataPin, LOW);
    else
      digitalWrite(dataPin, HIGH);
    delayMicroseconds(50);

    if(i>0)
      digitalWrite(clockPin, LOW);
    else
      digitalWrite(clockPin, HIGH);
  }  
  delay(20); 
}

