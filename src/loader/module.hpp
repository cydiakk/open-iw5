#pragma once

class module
{
public:
	virtual ~module() {};
	virtual void pre_load() {}
	virtual void post_load() {}
	virtual void pre_destroy() {}
};
