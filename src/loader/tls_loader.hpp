#pragma once
#include "utils/memory.hpp"

class tls_loader final
{
public:
	class tls_executer final
	{
	public:
		tls_executer();
		~tls_executer();
	};

	static void handle(IMAGE_TLS_DIRECTORY* tls_directory);

private:
	class tls_entry final
	{
	public:
		tls_entry(IMAGE_TLS_DIRECTORY* tls_directory);
		~tls_entry();

		void execute(bool is_attaching);

	private:
		utils::memory::allocator allocator_;
		IMAGE_TLS_DIRECTORY* tls_directory_;
		DWORD tls_index_;

		void attach();
		void detach();
	};

	static std::mutex mutex_;
	static std::vector<std::unique_ptr<tls_entry>> tls_entries_;

	static void execute(bool is_attaching);
};
