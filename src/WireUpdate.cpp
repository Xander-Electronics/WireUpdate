/*
  Copyright (c) 2017 Arturo Guadalupi.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include "WireUpdate.h"

#define DEFAULT_UPDATE_ADDRESS 0x55
#define START_UPDATE "UPDATE"
#define START_UPDATE_SIZE 6
#define BUFFER_SIZE 8

wireUpdateStates _updateStatus = UPDATE_IDLE;
uint32_t _currentSize = 0, _updateSize = 0;
char _rxBuffer[BUFFER_SIZE];
uint8_t _rxBufferIndex = 0;

void receiveEvent(int howMany) {
  switch (_updateStatus) {
    default: break;

    case UPDATE_IDLE:
      if (howMany == START_UPDATE_SIZE) {
        while (Wire.available() > 0) {
          _rxBuffer[_rxBufferIndex++] = Wire.read();
        }
        _rxBuffer[_rxBufferIndex] = '\0'; //terminate the string
        if (!strcmp(_rxBuffer, START_UPDATE)) {
          _updateStatus = UPDATE_SIZE;
        }
        else {
          _updateStatus = UPDATE_IDLE;
        }
      }
      _rxBufferIndex = 0;
      break;

    case UPDATE_SIZE:
      if (howMany == 4) {
        while (Wire.available() > 0) {
          _rxBuffer[_rxBufferIndex++] = Wire.read();
        }
        _updateSize = ((uint32_t)_rxBuffer[0]) + ((uint32_t)_rxBuffer[1] << 8) + ((uint32_t)_rxBuffer[2] << 16) + ((uint32_t)_rxBuffer[3] << 24);
        _updateStatus = UPDATE_RECEIVING;
      }
      else {
        _updateStatus = UPDATE_IDLE;
      }
      _rxBufferIndex = 0;
      break;

    case UPDATE_RECEIVING:
      while (Wire.available() > 0) {
        char c = Wire.read();
        InternalStorage.write(c);
      }
      _currentSize += howMany;
      if (_currentSize == _updateSize) {
        InternalStorage.close();
        InternalStorage.apply();
        _updateStatus = UPDATE_IDLE;
      }
      else {
        if (_currentSize > _updateSize) {
          _updateStatus = UPDATE_IDLE; //something went wrong. Too many data received
        }
        else {
          _updateStatus = UPDATE_RECEIVING;
        }
      }
      _rxBufferIndex = 0;
      break;
  }
}

/*Public functions*/
int WireUpdateClass::beginSlave(void) {
  Wire.begin(DEFAULT_UPDATE_ADDRESS);
  return initialize();
}

int WireUpdateClass::beginSlave(uint8_t address) {
  Wire.begin(address);
  return initialize();
}

int WireUpdateClass::beginSlave(int address) {
  Wire.begin((uint8_t) address);
  return initialize();
}

int WireUpdateClass::beginMaster(int csPin) {
  Wire.begin();

  if (!SD.begin(csPin)) {
    return SD_ERROR;
  }
  return OK;
}

void WireUpdateClass::startUpdate(uint8_t address) {
  Wire.beginTransmission(address);
  Wire.write(START_UPDATE, strlen(START_UPDATE));
  Wire.endTransmission();
}

int WireUpdateClass::sendUpdate(uint8_t address, const char fileName[]) {
  myFile = SD.open(fileName);
  if (myFile) {
    startUpdate(address);
    delay(100);
    uint32_t fileSize = myFile.size();
    send(address, fileSize);
    delay(100);

    uint32_t currentByte = 0, totalSize = 0;
    char page[BUFFER_SIZE];

    // read from the file until there's nothing else in it
    while (myFile.available()) {
      char c = myFile.read();
      page[currentByte] = c;
      currentByte++;
      totalSize++;
      if ((currentByte >= BUFFER_SIZE) || (totalSize == fileSize))  {
        Wire.beginTransmission(address);
        Wire.write(page, currentByte);
        Wire.endTransmission();
        delay(5);
        currentByte = 0;
      }
    }
    myFile.close();

    return OK;
  }
  else {
    return FILE_ERROR;
  }
}

/*Private functions*/
int WireUpdateClass::initialize(void) {
  Wire.onReceive(receiveEvent);
  if (!InternalStorage.open()) {
    return STORAGE_ERROR;
  }
  else {
    return OK;
  }
}

void WireUpdateClass::send(uint8_t address, uint32_t data) {
  Wire.beginTransmission(address);
  Wire.write((uint8_t)(data));
  Wire.write((uint8_t)(data >> 8));
  Wire.write((uint8_t)(data >> 16));
  Wire.write((uint8_t)(data >> 24));
  Wire.endTransmission();
}

WireUpdateClass WireUpdate;
