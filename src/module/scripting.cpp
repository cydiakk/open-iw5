#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "game/game.hpp"

class scripting final : public module
{
public:
	void post_load() override
	{
		this->chai_.add(chaiscript::fun(&function), "function");

		double d = this->chai_.eval<double>("function(3, 4.75);");
		printf("Result: %f", d);
	}

private:
	chaiscript::ChaiScript chai_;

	static double function(int i, double j)
	{
		return i * j;
	}

};

REGISTER_MODULE(scripting)
