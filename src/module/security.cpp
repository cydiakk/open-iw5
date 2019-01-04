#include <std_include.hpp>
#include "loader/module_loader.hpp"

class security final : public module
{
public:
	void post_load() override
	{
		// TODO: Patch open vulnerabilities
	}
};

REGISTER_MODULE(security)
