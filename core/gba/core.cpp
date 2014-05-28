/*
 * Copyright 2013-2014 gtalent2@gmail.com
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <vector>

#include <gba.h>

#include "../core.hpp"

namespace wombat {
namespace core {

extern SubscriptionManager _submgr;
extern std::vector<std::pair<Drawer*, Graphics*>> _drawers;
extern bool _running;
const uint64 _timer3Interval = 61040;
uint64 _time = 0;   // time in milliseconds
uint64 _timeMs = 0; // time in microseconds

EventQueue _mainEventQueue;

TaskProcessor _taskProcessor(&_mainEventQueue);

void addTask(std::function<TaskState(Event)> task, TaskState state) {
	_taskProcessor.addTask(task, state);
}

void addTask(Task *task, TaskState state) {
	_taskProcessor.addTask(task, state);
}

void draw() {
}

void _draw() {
	for (auto d : _drawers) {
		d.first->draw(*d.second);
		d.second->resetViewport();
	}
}

void _timer3Int() {
	_timeMs += _timer3Interval;
	_time = _timeMs / 1000;
	REG_IF = IRQ_TIMER3;
}

void main() {
	while (_running) {
		IntrWait(0, 0);
	}
}

int init(models::Settings settings) {
	irqInit();
	irqSet(IRQ_TIMER3, _timer3Int);
	irqEnable(IRQ_TIMER3);
	return 0;
}

}
}
