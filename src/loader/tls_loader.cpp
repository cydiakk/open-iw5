#include <std_include.hpp>
#include "tls_loader.hpp"
#include "utils/nt.hpp"

std::mutex tls_loader::mutex_;
std::vector<std::unique_ptr<tls_loader::tls_entry>> tls_loader::tls_entries_;

//static thread_local tls_loader::tls_executer $;

void tls_loader::handle(IMAGE_TLS_DIRECTORY* tls_directory)
{
	std::lock_guard _(mutex_);
	tls_entries_.push_back(std::make_unique<tls_entry>(tls_directory));
}

void tls_loader::execute(const bool is_attaching)
{
	std::lock_guard _(mutex_);

	for (auto& entry : tls_entries_)
	{
		entry->execute(is_attaching);
	}
}

tls_loader::tls_entry::tls_entry(IMAGE_TLS_DIRECTORY* tls_directory) : tls_directory_(tls_directory)
{
	this->tls_index_ = 18;//TlsAlloc();
	*reinterpret_cast<DWORD*>(this->tls_directory_->AddressOfIndex) = this->tls_index_;

	this->execute(true);
}

tls_loader::tls_entry::~tls_entry()
{
	//TlsFree(this->tls_index_);
	this->allocator_.clear();
}

void tls_loader::tls_entry::execute(const bool is_attaching)
{
	if (is_attaching) this->attach();
	else this->detach();
}

void tls_loader::tls_entry::attach()
{
	const size_t data_size = this->tls_directory_->EndAddressOfRawData - this->tls_directory_->StartAddressOfRawData;
	const size_t size = data_size + this->tls_directory_->SizeOfZeroFill;
	const auto data = this->allocator_.allocate(size);

	std::memcpy(data, PVOID(this->tls_directory_->StartAddressOfRawData), data_size);

	const auto tls_indices = reinterpret_cast<LPVOID*>(__readfsdword(0x2C));
	tls_indices[this->tls_index_] = data;

	auto callbacks = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(this->tls_directory_->AddressOfCallBacks);
	if (callbacks)
	{
		utils::nt::module self;

		while (*callbacks)
		{
			(*callbacks)(self.get_ptr(), DLL_THREAD_ATTACH, nullptr);
			++callbacks;
		}
	}
}

void tls_loader::tls_entry::detach()
{
	auto callbacks = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(this->tls_directory_->AddressOfCallBacks);
	if (callbacks)
	{
		utils::nt::module self;

		while (*callbacks)
		{
			(*callbacks)(self.get_ptr(), DLL_THREAD_DETACH, nullptr);
			++callbacks;
		}
	}

	const auto tls_indices = reinterpret_cast<LPVOID*>(__readfsdword(0x2C));
	const auto data = tls_indices[this->tls_index_];

	//const auto data = TlsGetValue(this->tls_index_);
	//this->allocator_.free(data);
}

tls_loader::tls_executer::tls_executer()
{
	execute(true);
}

tls_loader::tls_executer::~tls_executer()
{
	execute(false);
}
