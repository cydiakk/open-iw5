#include <std_include.hpp>
#include "interface.hpp"
#include "utils/memory.hpp"

namespace steam
{
	interface::interface(void* interface_ptr) : interface_ptr_(static_cast<void***>(interface_ptr))
	{

	}

	interface::method interface::find_method(const std::string& name)
	{
		const auto method_entry = this->methods_.find(name);
		if(method_entry != this->methods_.end())
		{
			return method_entry->second;
		}

		return this->search_method(name);
	}

	interface::method interface::search_method(const std::string& name)
	{
		if (!utils::memory::is_bad_read_ptr(this->interface_ptr_))
		{
			auto vftbl = *this->interface_ptr_;

			while (!utils::memory::is_bad_read_ptr(vftbl) && !utils::memory::is_bad_code_ptr(*vftbl))
			{
				const interface::method_result result = this->analyze_method(*vftbl);
				if (!result.valid) break;

				const interface::method method_result{ *vftbl, result.param_size };
				this->methods_[result.name] = method_result;

				if(result.name == name)
				{
					return method_result;
				}

				++vftbl;
			}
		}

		return {};
	}

	interface::method_result interface::analyze_method(const void* method_ptr)
	{
		interface::method_result result;
		if (utils::memory::is_bad_code_ptr(method_ptr)) return result;

		ud_t ud;
		ud_init(&ud);
		ud_set_mode(&ud, 32);
		ud_set_pc(&ud, uint64_t(method_ptr));
		ud_set_input_buffer(&ud, static_cast<const uint8_t*>(method_ptr), INT32_MAX);

		while (true)
		{
			ud_disassemble(&ud);

			if (ud_insn_mnemonic(&ud) == UD_Iret)
			{
				const ud_operand* operand = ud_insn_opr(&ud, 0);
				if (!operand) break;

				if (operand->type == UD_OP_IMM && operand->size == 16)
				{
					result.param_size = operand->lval.uword;
					result.valid = !result.name.empty();
					return result;
				}

				break;
			}

			if (ud_insn_mnemonic(&ud) == UD_Ipush && result.name.empty())
			{
				const auto operand = ud_insn_opr(&ud, 0);
				if (operand->type == UD_OP_IMM && operand->size == 32)
				{
					char* operand_ptr = reinterpret_cast<char*>(operand->lval.udword);
					if (!utils::memory::is_bad_read_ptr(operand_ptr))
					{
						result.name = operand_ptr;
					}
				}
			}
		}

		return result;
	}
}
