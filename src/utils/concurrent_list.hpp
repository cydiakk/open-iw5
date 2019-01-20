#pragma once

namespace utils
{
	template <typename T>
	class concurrent_list final
	{
	public:
		class entry final
		{
		public:

			bool has_next()
			{
				return this->next_.operator bool();
			}

			bool is_valid()
			{
				return this->object_.operator bool();
			}

			void set(const std::shared_ptr<T>& object)
			{
				this->object_ = object;
			}

			std::shared_ptr<T> get()
			{
				return this->object_;
			}

			entry get_next()
			{
				if (this->has_next())
				{
					return *this->next_;
				}
				else
				{
					return entry();
				}
			}

			std::shared_ptr<entry> get_next_entry()
			{
				return this->next_;
			}

			void set_next_entry(std::shared_ptr<entry> entry)
			{
				this->next_ = entry;
			}

			T* operator->()
			{
				return this->object_.get();
			}

			std::shared_ptr<T> operator*()
			{
				return this->object_;
			}

			explicit operator bool()
			{
				return this->is_valid();
			}

			bool operator==(const entry& other)
			{
				return this->object_.get() == other.object_.get();
			}

			bool operator!=(const entry& other)
			{
				return !(*this == other);
			}

			entry& operator++()
			{
				*this = this->get_next();
				return *this;
			}

			entry operator++(int)
			{
				entry result = *this;
				this->operator++();
				return result;
			}

		private:
			std::shared_ptr<T> object_;
			std::shared_ptr<entry> next_;
		};

		void add(const T& object)
		{
			std::lock_guard _(this->mutex_);

			if (!this->empty())
			{
				std::shared_ptr<entry> current_object = std::make_shared<entry>();
				current_object->set(this->object_.get());

				this->object_ = entry();
				this->object_.set_next_entry(current_object);
			}

			const auto obj_ptr = std::make_shared<T>(object);
			this->object_.set(obj_ptr);
		}

		void remove(const std::shared_ptr<T>& object)
		{
			std::lock_guard _(this->mutex_);

			if (!this->empty())
			{
				if (this->object_.get() == object)
				{
					this->object_ = this->object_.get_next();
				}
				else if (this->object_.has_next())
				{
					for (auto entry = this->object_; entry.is_valid(); ++entry)
					{
						auto next = entry.get_next();

						if (next.is_valid() && next.get() == object)
						{
							entry.set_next_entry(next.get_next_entry());
						}
					}
				}
			}
		}

		void remove(entry entry)
		{
			if (entry.is_valid())
			{
				this->remove(entry.get());
			}
		}

		bool empty()
		{
			return !this->object_;
		}

		entry begin()
		{
			return this->object_;
		}

		entry end()
		{
			return entry();
		}

		void clear()
		{
			this->object_ = entry();
		}

	private:
		std::mutex mutex_;
		entry object_;
	};
}
