#include <std_include.hpp>
#include "binary_loader.hpp"
#include "utils/nt.hpp"
#include "utils/io.hpp"
#include "utils/cryptography.hpp"
#include "utils/string.hpp"
#include "utils/compression.hpp"

#define DEDI_HASH "F271C305117B79242E254E9F64BD5AA2993CAC8E57975243EBD44CD576418D20"

namespace binary_loader
{
	std::string load_resource(const int id)
	{
		const auto res = FindResource(::utils::nt::module(), MAKEINTRESOURCE(id), RT_RCDATA);
		if (!res) return {};

		const auto handle = LoadResource(nullptr, res);
		if (!handle) return {};

		return std::string(LPSTR(LockResource(handle)), SizeofResource(nullptr, res));
	}

	std::string load_delta(const launcher::mode mode)
	{
		if (mode == launcher::mode::singleplayer)
		{
			return load_resource(BINARY_SP);
		}

		if (mode == launcher::mode::multiplayer)
		{
			return load_resource(BINARY_MP);
		}

		return {};
	}

	std::string load_base(const bool verify = true)
	{
		std::string data;
		if (!utils::io::read_file("iw5mp_server.exe", &data))
		{
			throw std::runtime_error("Unable to load iw5mp_server.exe");
		}

		if (verify && utils::cryptography::sha256::compute(data, true) != DEDI_HASH)
		{
			throw std::runtime_error("Your iw5mp_server.exe is incompatible with this client.");
		}

		return data;
	}

	void create_for_file(const std::string& file, const std::string& base)
	{
		std::string data;
		if (!utils::io::read_file(file, &data))
		{
			throw std::runtime_error(utils::string::va("Unable to load file %s!", file.data()));
		}

		const auto new_data = reinterpret_cast<const unsigned char*>(data.data());
		const auto old_data = reinterpret_cast<const unsigned char*>(base.data());

		std::vector<unsigned char> diff;
		create_diff(new_data, new_data + data.size(), old_data, old_data + base.size(), diff);

		const unsigned long long size = data.size();

		std::string result(reinterpret_cast<char*>(diff.data()), diff.size());
		result.append(reinterpret_cast<const char*>(&size), sizeof(size));
		result = utils::compression::zlib::compress(result);

		utils::io::write_file(file + ".diff", result);
	}

	void create()
	{
		const auto base = load_base(false);

		utils::io::write_file("hash.txt", utils::cryptography::sha256::compute(base, true));

		create_for_file("iw5sp.exe", base);
		create_for_file("iw5mp.exe", base);
	}

	std::string build_binary(const std::string& base, const std::string& diff)
	{
		const auto* size = reinterpret_cast<const unsigned long long*>(diff.data() + diff.size() - sizeof(unsigned long
			long));

		std::string binary;
		binary.resize(size_t(*size));

		const auto new_data = reinterpret_cast<unsigned char*>(binary.data());
		const auto old_data = reinterpret_cast<const unsigned char*>(base.data());
		const auto diff_data = reinterpret_cast<const unsigned char*>(diff.data());

		if (patch(new_data, new_data + binary.size(), old_data, old_data + base.size(), diff_data,
		          diff_data + diff.size() - sizeof(*size)) == hpatch_FALSE || binary.empty())
		{
			throw std::runtime_error("Unable to create binary from patch!");
		}

		return binary;
	}

	std::string load(const launcher::mode mode)
	{
		auto base = load_base();
		if (mode == launcher::mode::server)
		{
			return base;
		}

		auto delta = load_delta(mode);
		delta = utils::compression::zlib::decompress(delta);
		return build_binary(base, delta);
	}
}
