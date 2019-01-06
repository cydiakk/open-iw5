#pragma once

#ifdef interface
#undef interface
#endif

namespace steam
{
	class interface final
	{
	public:
		class method final
		{
		public:
			void* pointer = nullptr;
			size_t param_size = 0;
		};

		class method_result final
		{
		public:
			bool valid = false;
			std::string name;
			size_t param_size = 0;
		};

		interface(void* interface_ptr);

		template<typename T, typename... Args>
		T invoke(const std::string& method_name, Args... args)
		{
			if(!this->interface_ptr_)
			{
				throw std::runtime_error("Invalid interface pointer");
			}

			const auto method_result = this->find_method(method_name);
			if(!method_result.pointer)
			{
				throw std::runtime_error("Unable to find desired method");
			}

			return reinterpret_cast<T(__thiscall*)(void*, Args ...)>(method_result.pointer)(this->interface_ptr_, args...);
		}

		template<typename T, typename... Args>
		T invoke(size_t table_entry, Args... args)
		{
			if (!this->interface_ptr_)
			{
				throw std::runtime_error("Invalid interface pointer");
			}

			return reinterpret_cast<T(__thiscall*)(void*, Args ...)>((*this->interface_ptr_)[table_entry])(this->interface_ptr_, args...);
		}

	private:
		void*** interface_ptr_;
		std::unordered_map<std::string, method> methods_;

		method find_method(const std::string& name);
		method search_method(const std::string& name);

		method_result analyze_method(const void* method_ptr);
	};
}
