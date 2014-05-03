#include "app.hpp"

namespace wombat {

using core::TaskState;

void App::init() {
	core::subscribe(core::KeyDownEvent);
	core::subscribe(core::KeyUpEvent);
	core::subscribe(core::QuitEvent);
}

TaskState App::run(core::Event e) {
	TaskState retval = TaskState::Continue;

	switch (e.type()) {
	case core::ResolutionChange:
		core::draw();
		break;
	case core::Timeout:
		core::draw();
		retval = 16;
		break;
	case core::QuitEvent:
		quit();
		break;
	case core::KeyDownEvent:
		switch (e.key()) {
		case core::Key_Escape:
		case core::Key_Q:
			quit();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return retval;
}

void App::quit() {
	core::quit();
}

}
