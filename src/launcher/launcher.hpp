#pragma once
#include "window.hpp"

class launcher
{
public:
	launcher();

	void run() const;

private:
	window window_;
};
