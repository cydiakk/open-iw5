#pragma once
#include "launcher/launcher.hpp"

class module
{
public:
	virtual ~module() {};
	virtual void post_load() {}
	virtual void pre_destroy() {}
};
