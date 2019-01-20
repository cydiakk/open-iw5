#pragma once

namespace game
{
	namespace scripting
	{
		class context;

		class scheduler final
		{
		public:
			explicit scheduler(context* context);

			void run_frame();

		private:
			context* context_;
		};
	}
}
