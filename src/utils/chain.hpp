namespace utils
{
	template <typename T>
	class chain
	{
	public:
		class entry
		{
		private:
			std::shared_ptr<T> object_;
			std::shared_ptr<entry> next_;

		public:
			bool has_next()
			{
				return (this->next_.use_count() > 0);
			}

			bool is_valid()
			{
				return (this->object_.use_count() > 0);
			}
			
			void set(T object)
			{
				this->object_ = std::shared_ptr<T>(new T());
				*this->object_.get() = object;
			}

			std::shared_ptr<T> get()
			{
				return this->object_;
			}

			entry get_next()
			{
				if (this->has_next())
				{
					return *(this->next_.get());
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

			T *operator->()
			{
				return (this->object_.get());
			}

			entry& operator++ ()
			{
				*this = this->get_next();
				return *this;
			}

			entry operator++ (int)
			{
				entry result = *this;
				this->operator++();
				return result;
			}
		};

	private:
		std::mutex mutex_;
		entry object_;

	public:
		void add(const T& object)
		{
			std::lock_guard _(this->mutex_);
			
			if (!this->empty())
			{
				// Create new chain entry
				std::shared_ptr<entry> current_object = std::shared_ptr<entry>(new entry);
				*current_object.get() = this->object_;

				// Add it to the chain
				this->object_ = entry();
				this->object_.set_next_entry(current_object);
			}

			this->object_.set(object);
		}

		void remove(std::shared_ptr<T> object)
		{
			std::lock_guard _(this->mutex_);
			
			if (!this->empty())
			{
				if (this->object_.get().get() == object.get())
				{
					this->object_ = this->object_.get_next();
				}
				else if(this->object_.has_next())
				{
					for (auto entry = this->object_; entry.is_valid(); ++entry)
					{
						auto next = entry.get_next();

						if (next.is_valid() && next.get().get() == object.get())
						{
							*entry.get_next_entry().get() = next.get_next();
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
			return !this->object_.is_valid();
		}

		entry begin()
		{
			return this->object_;
		}

		void clear()
		{
			this->object_ = entry();
		}
	};
}
