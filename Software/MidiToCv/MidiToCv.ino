#include <MIDI.h>
#include <SPI.h>

// MCP4822 DAC out
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK  13
#define SPI_CS_1 9
#define SPI_CS_2 10
#define GATE_1_PIN 5
#define GATE_2_PIN 6
#define GATE_3_PIN 7
#define GATE_4_PIN 8
#define NOTE_SF 47.069f 

MIDI_CREATE_DEFAULT_INSTANCE();

//SPISettings settingsA(16000000, MSBFIRST, SPI_MODE0);  // At 16 = SPI Clock = 8MHz.

void setup() {
  pinMode(SPI_CS_1, OUTPUT); 
  pinMode(SPI_CS_2, OUTPUT); 
  pinMode(GATE_1_PIN, OUTPUT); 
  pinMode(GATE_2_PIN, OUTPUT); 
  pinMode(GATE_3_PIN, OUTPUT); 
  pinMode(GATE_4_PIN, OUTPUT); 

  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz
  //noInterrupts();

  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {  

  if (MIDI.read()) { 

    int channel = MIDI.getChannel();

    byte gatePin, cv, cs, dacChannel, note;
    unsigned int voltage;
    
    switch(channel) {
      case 1:
        gatePin = GATE_1_PIN;
        cs = SPI_CS_1;
        dacChannel = 0;
        break;
      case 2:
        gatePin = GATE_2_PIN;
        cs = SPI_CS_1;
        dacChannel = 1;
        break;
      case 3:
        gatePin = GATE_3_PIN;
        cs = SPI_CS_2;
        dacChannel = 0;
        break;
      case 4:
        gatePin = GATE_4_PIN;
        cs = SPI_CS_2;
        dacChannel = 1;
        break;
    }
                   
    byte type = MIDI.getType();
     
    switch (type) {
      case midi::NoteOn: 
        note = MIDI.getData1();
        voltage = (unsigned int) ((float) note * NOTE_SF + 0.5); 
        setVoltage(cs, dacChannel, 1, voltage); 
        digitalWrite(gatePin, LOW);
        break;
      case midi::NoteOff:
        digitalWrite(gatePin, HIGH);
        break;
      default:
        break;
    }
  }
}

void setVoltage(byte cs, bool dacChannel, bool gain, unsigned int mV)
{
  unsigned int command = dacChannel ? 0x9000 : 0x1000;

  command |= gain ? 0x0000 : 0x2000;
  command |= (mV & 0x0FFF);
  
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  SPI.transfer(command>>8);
  SPI.transfer(command&0xFF);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
}
