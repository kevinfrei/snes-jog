#pragma once

/* This is all the code for communicating with the SNES controller */

uint8_t writeData(uint8_t *data, uint8_t size, const char *err = NULL) {
  Wire.beginTransmission(I2C_ADDR);
  uint8_t num = Wire.write(data, size);
  Wire.endTransmission();
  if (num != size && err != NULL) {
    DBG("Tried to write");
    DBGN(size);
    DBG(" but only wrote ");
    DBGN(num);
    DBG(" bytes instead: ");
    DBGN(err);
  }
  return num;
}

uint8_t readData(uint8_t count, uint8_t id) {
  writeData(&id, 1, "ID Value write");
  delayMicroseconds(200);
  Wire.requestFrom(I2C_ADDR, count);
  uint8_t numTries = 0;
  uint8_t ofs = 0;
  do {
    for (; count && Wire.available(); ofs++) {
      buffer[ofs] = Wire.read();
      count--;
    }
    if (count > 0) {
      numTries++;
      delayMicroseconds(10);
    }
  } while (count && numTries < 5);
  if (count > 0) {
    DBG("Missing remaining bytes: ");
    DBGN(count);
    DBG(": ");
    for (uint8_t i = 0; i < ofs; i++) {
      DBGN(i);
      DBG(">");
      DBGN(buffer[i]);
      DBG(" ");
    }
    return 0;
  } else {
    return ofs;
  }
}

void initController() {
  cur_state = CS_ERR;
  // 0x3 is supposed to be the data format, but it seems irrelevant...
  uint8_t data[] = {0xf0, 0x55, 0xfb, 0x00, 0xfe, 0x3};
  if (writeData(data, 6, "Initializing Device") == 6) {
    cur_state = CS_INIT;
  }
}

void setupController() {
  initController();
  if (cur_state != CS_INIT) {
    return;
  }
  delay(100);
  uint8_t bytesRead = readData(6, 0xfa);
  if (bytesRead != 6) {

    cur_state = CS_ERR;
    DBG("Error in setupController");
    return;
  }
  // For now, just hard-check all 6 values
  if (buffer[0] != 1 || buffer[1] != 0 || buffer[2] != 0xa4 ||
      buffer[3] != 0x20 || buffer[4] > 1 || buffer[5] != 1) {
    cur_state = CS_ERR;
    DBG("Wrong Values in setupController:  ");
    for (int q = 0; q < 6; q++) {
      DBGN(q);
      DBG(":");
      DBGN((uint8_t)buffer[q]);
      DBG(" ");
    }

  } else {
    cur_state = CS_OK;
  }
}

uint8_t isPressed(Button_t button) {
  return (curPressed & button) ? 1 : 0;
}

uint8_t isReleased(Button_t button) {
  return 1 - isPressed(button);
}
