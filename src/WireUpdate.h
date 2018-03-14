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

#ifndef _WIRE_UPDATE_H_INCLUDED
#define _WIRE_UPDATE_H_INCLUDED

#include <SDU.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <InternalStorage.h>

#define OK  1
#define FILE_ERROR  -1
#define SD_ERROR -2
#define STORAGE_ERROR -3

typedef enum {
  UPDATE_IDLE,
  UPDATE_SIZE,
  UPDATE_RECEIVING,
} wireUpdateStates;

class WireUpdateClass {
  public:
    void setWire(TwoWire *wire = &Wire);
    int beginSlave(TwoWire *wire = &Wire);
    int beginSlave(uint8_t address, TwoWire *wire = &Wire);
    int beginSlave(int address, TwoWire *wire = &Wire);

    int beginMaster(int csPin, TwoWire *wire = &Wire);
    int sendUpdate(uint8_t address, const char fileName[]);

  private:
    File myFile;
    int initialize(void);
    void startUpdate(uint8_t address);
    void send(uint8_t address, uint32_t data);
};

extern WireUpdateClass WireUpdate;
#endif
