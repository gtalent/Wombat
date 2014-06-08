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
#include "sprite.hpp"

namespace wombat {
namespace world {

using core::TaskState;

Sprite::Sprite() {
	m_spriteClass = 0;
}

TaskState Sprite::run(core::Event) {
	return TaskState::Continue;
}

void Sprite::load(models::Sprite model) {
	m_spriteClass = SpriteClass::checkout(model.SpriteClass);
}

void Sprite::draw(core::Graphics &gfx, common::Point pt) {
}

}
}
