/* 
  Ticker.h - esp32 library that calls functions periodically

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

#include "Ticker.h"

std::vector<Ticker*> Ticker::_timers;

void Ticker::tickerCallback(TimerHandle_t timerHandle) {
  for (uint8_t i = 0; i < _timers.size(); ++i) {
    if (_timers.at(i)->_timerHandle == timerHandle) {
      _timers.at(i)->_callback((void*)_timers.at(i)->_arg);
    }
  }
}

Ticker::Ticker():
  _timerHandle(nullptr) {
}

Ticker::~Ticker() {
  if (_timerHandle) {
    xTimerDelete(_timerHandle, 0);
    for (uint8_t i = 0; i < _timers.size(); ++i) {
      if (_timers.at(i)->_timerHandle == _timerHandle) {
        _timers.erase(_timers.begin()+i);
      }
    }
  }
}

void Ticker::_attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t callback, uint32_t arg) {
  _callback = callback;
  _arg = arg;
  if (_timerHandle) {
    xTimerChangePeriod(_timerHandle, milliseconds, 0);
  }
  else {
    _timerHandle = xTimerCreate("Ticker", pdMS_TO_TICKS(milliseconds), (repeat)?pdTRUE:pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(&Ticker::tickerCallback));
    if (_timerHandle == NULL) {
    }
    else {
      xTimerStart(_timerHandle, 0);
      _timers.push_back(this);
    }
  }
}

void Ticker::detach() {
  xTimerStop(_timerHandle, 0);
}
