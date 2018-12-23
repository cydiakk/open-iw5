#include <std_include.hpp>
#include "launcher.hpp"

launcher::launcher() : window_("Open-IW5", 300, 400)
{

}

void launcher::run() const
{
	this->window_.run();
}
