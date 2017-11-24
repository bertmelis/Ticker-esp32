/* 
  ESP32Ticker.cpp - esp32 library that calls functions periodically

	Copyright (c) 2017 Bert Melis. All rights reserved.
	
	Based on the original work of:
  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
	The original version is part of the esp8266 core for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "ESP32Ticker.h"

Ticker* Ticker::_timers[10] = {nullptr};

void Ticker::tickerCallback(TimerHandle_t timerHandle) {
  Ticker* ticker = _timers[(uint32_t)pvTimerGetTimerID(timerHandle)];
  ticker->_callback((void*)ticker->_arg);
}

Ticker::Ticker():
  _timerHandle(nullptr) {
}

Ticker::~Ticker() {
  if (_timerHandle) {
    _timers[(uint32_t)pvTimerGetTimerID(_timerHandle)] = nullptr;
    xTimerDelete(_timerHandle, 0);
  }
}

void Ticker::_attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t callback, uint32_t arg) {
  _callback = callback;
  _arg = arg;
  if (_timerHandle) {
    xTimerChangePeriod(_timerHandle, milliseconds, 0);
  }
  else {
    uint32_t vacant_position = 0;
    while (_timers[vacant_position]) {
      ++vacant_position;
      if (vacant_position > 9) return;  // array is full
    }
    _timerHandle = xTimerCreate("Ticker", pdMS_TO_TICKS(milliseconds), (repeat)?pdTRUE:pdFALSE, (void*)vacant_position, reinterpret_cast<TimerCallbackFunction_t>(&Ticker::tickerCallback));
    if (_timerHandle == NULL) {
      //Timer was not created
    }
    else {
      xTimerStart(_timerHandle, 0);
      _timers[vacant_position] = this;
    }
  }
}

void Ticker::detach() {
  if (_timerHandle) {
    if (xTimerIsTimerActive(_timerHandle) == pdTRUE) {
      xTimerStop(_timerHandle, 0);
    }
  }
}
