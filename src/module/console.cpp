#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "game/game.hpp"

class console final : public module
{
public:
	console()
	{
		ShowWindow(GetConsoleWindow(), SW_HIDE);

		_pipe(this->handles_, 1024, _O_TEXT);
		_dup2(this->handles_[1], 1);
		_dup2(this->handles_[1], 2);

		setvbuf(stdout, nullptr, _IONBF, 0);
		setvbuf(stderr, nullptr, _IONBF, 0);
	}

	void post_start() override
	{
		this->console_runner_ = std::thread(std::bind(&console::runner, this));
	}

	void pre_destroy() override
	{
		this->terminate_runner_ = true;

		printf("\r\n");
		_flushall();

		_close(this->handles_[0]);
		_close(this->handles_[1]);

		if (this->console_runner_.joinable())
		{
			this->console_runner_.join();
		}
	}

	void post_load() override
	{
		if (game::is_dedi()) return;

		game::native::Sys_ShowConsole();

		std::lock_guard _(this->mutex_);
		this->console_initialized_ = true;
		this->replay_messages();
	}

private:
	bool console_initialized_ = false;
	bool terminate_runner_ = false;

	std::mutex mutex_;
	std::thread console_runner_;
	std::queue<std::string> message_queue_;

	int handles_[2]{};

	void replay_messages()
	{
		if (!this->console_initialized_) return;

		while (!this->message_queue_.empty())
		{
			this->push_message(this->message_queue_.front());
			this->message_queue_.pop();
		}
	}

	void push_message(const std::string& message)
	{
		if (!this->console_initialized_)
		{
			std::lock_guard _(this->mutex_);
			this->message_queue_.push(message);
			return;
		}

		game::native::Conbuf_AppendText(message.data());
	}

	void runner()
	{
		char buffer[1024];

		while (!this->terminate_runner_ && this->handles_[0])
		{
			const auto len = _read(this->handles_[0], buffer, sizeof(buffer));
			if (len > 0)
			{
				this->push_message(std::string(buffer, len));
			}
			else
			{
				std::this_thread::sleep_for(10ms);
			}
		}

		std::this_thread::yield();
	}
};

REGISTER_MODULE(console)
