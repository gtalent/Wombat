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
#ifndef WOMBAT_WORLD_CAMERA_HPP
#define WOMBAT_WORLD_CAMERA_HPP

#include <common/common.hpp>
#include <core/core.hpp>
#include "person.hpp"
#include "world.hpp"

namespace wombat {
namespace world {

class Camera: public core::Drawer, public core::Task {
	private:
		common::Bounds m_bounds;
		std::vector<Zone*> m_zones;
		World *m_world = nullptr;
		Person *m_person = nullptr;

	public:
		/**
		 * Constructor
		 */
		Camera(World *world);

		void init() override;

		core::TaskState run(core::Event) override;

		/**
		 * Draws what the Camera is currently hovering over.
		 * @param g the Graphics instance to draw the Camera's sight with
		 */
		void draw(core::Graphics &g) override;

		/**
		 * Gets the bounds of this Camera.
		 * @return the bounds of this Camera
		 */
		common::Bounds bounds();

		/**
		 * Takes an InitFile object to determine where to place the Camera on the map.
		 * @param init the InitFile used to load the application
		 */
		void init(models::InitFile init);

	private:
		void findZones();
		void updateSize();

		void keyDown(core::Event e);

		void keyUp(core::Event e);
};

}
}

#endif
