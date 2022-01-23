#ifndef RFID_READER_HPP_INCLUDED
#define RFID_READER_HPP_INCLUDED

#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <MFRC522.h>

namespace dps { namespace rfid { 
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~


#define CLK_PIN       2 // 14
#define MOSI_PIN      3 // 13
#define MISO_PIN      4 // 12
#define RST_PIN       0 // 26           // Configurable, see typical pin layout above
#define SS_PIN        1 // 25           // Configurable, see typical pin layout above
#define IRQ_PIN       5 // 27           // Configurable, depends on hardware



//==============================================================================================================================================================
class Reader
{
//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  Reader() : Mfrc522(SS_PIN, RST_PIN)
  {
    SPI.setSCK(CLK_PIN);
    SPI.setRX (MISO_PIN);
    SPI.setTX (MOSI_PIN);
    SPI.begin();
    //SPI.begin(CLK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    Mfrc522.PCD_Init(); // Init MFRC522 card

    /* read and printout the MFRC522 version (valid values 0x91 & 0x92)*/
    auto version = Mfrc522.PCD_ReadRegister(Mfrc522.VersionReg);
    Serial.print(F("Version "));
    Serial.println(version);

    /* setup the IRQ pin*/
    pinMode(IRQ_PIN, INPUT_PULLUP);

    /*
    * Allow the ... irq to be propagated to the IRQ pin
    * For test purposes propagate the IdleIrq and loAlert
    */
    regVal = 0xA0; //rx irq
    Mfrc522.PCD_WriteRegister(Mfrc522.ComIEnReg, regVal);

    /*Activate the interrupt*/
    attachInterrupt(digitalPinToInterrupt(IRQ_PIN), readCard, FALLING);

    while (TagDetected) {}
    TagDetected = false;

    delay(1000);

    Serial.println(F("End setup"));
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  MFRC522::Uid* operator()()
  {
    MFRC522::Uid* uid = nullptr;

    TagDetected = !digitalRead(IRQ_PIN);


    if (TagDetected)
    {
      auto succ = Mfrc522.PICC_ReadCardSerial(); //read the tag data

      clearInt();
      Mfrc522.PICC_HaltA();
      if (succ)
      {


        CurrentUid = Mfrc522.uid;
        uid = &CurrentUid;
      }

      TagDetected = false;
    }

    activateRec();

    return uid;
  }


//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  static volatile bool TagDetected;
  
  uint8_t regVal = 0x7F;
  MFRC522 Mfrc522;
  MFRC522::Uid CurrentUid = {};


  void activateRec()
  {
    Mfrc522.PCD_WriteRegister(Mfrc522.FIFODataReg,   Mfrc522.PICC_CMD_REQA);
    Mfrc522.PCD_WriteRegister(Mfrc522.CommandReg,    Mfrc522.PCD_Transceive);
    Mfrc522.PCD_WriteRegister(Mfrc522.BitFramingReg, 0x87);
  }

  void clearInt()
  {
    Mfrc522.PCD_WriteRegister(Mfrc522.ComIrqReg, 0x7F);    
  }

  static void readCard()
  {
    TagDetected = true;
  }
};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
}} // namespace dps::rfid

#endif // RFID_READER_HPP_INCLUDED
