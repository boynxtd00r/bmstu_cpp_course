#pragma once

#include <algorithm>
#include <cstring>
#include <exception>
#include <iostream>

namespace bmstu
{
template <typename T>
class basic_string;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

template <typename T>
class basic_string
{
   private:
	static constexpr size_t SSO_CAPACITY =
		(sizeof(T*) + sizeof(size_t) + sizeof(size_t)) / sizeof(T) - 1;

	struct LongString
	{
		T* ptr;
		size_t size;
		size_t capacity;
	};

	struct ShortString
	{
		T buffer[SSO_CAPACITY + 1];
		unsigned char size;
	};

	union Data
	{
		LongString long_str;
		ShortString short_str;
	};

	Data data_;
	bool is_long_;

	bool is_long() const { return is_long_; }

	T* get_ptr()
	{
		return is_long_ ? data_.long_str.ptr : data_.short_str.buffer;
	}

	const T* get_ptr() const
	{
		return is_long_ ? data_.long_str.ptr : data_.short_str.buffer;
	}

	size_t get_size() const
	{
		return is_long_ ? data_.long_str.size : data_.short_str.size;
	}

	size_t get_capacity() const
	{
		return is_long_ ? data_.long_str.capacity : SSO_CAPACITY;
	}

	void set_long_ptr(T* ptr, size_t size, size_t capacity)
	{
		data_.long_str.ptr = ptr;
		data_.long_str.size = size;
		data_.long_str.capacity = capacity;
		is_long_ = true;
	}

	void set_short_size(size_t size)
	{
		data_.short_str.size = static_cast<unsigned char>(size);
		data_.short_str.buffer[size] = 0;
		is_long_ = false;
	}

	void copy_from(const T* str, size_t size)
	{
		if (size <= SSO_CAPACITY)
		{
			for (size_t i = 0; i < size; ++i)
			{
				data_.short_str.buffer[i] = str[i];
			}
			set_short_size(size);
		}
		else
		{
			T* new_ptr = new T[size + 1];
			for (size_t i = 0; i < size; ++i)
			{
				new_ptr[i] = str[i];
			}
			new_ptr[size] = 0;
			set_long_ptr(new_ptr, size, size);
		}
	}

   public:
	basic_string() : is_long_(false)
	{
		data_.short_str.size = 0;
		data_.short_str.buffer[0] = 0;
	}

	basic_string(size_t size) : is_long_(size > SSO_CAPACITY)
	{
		if (is_long_)
		{
			T* ptr = new T[size + 1];
			for (size_t i = 0; i < size; ++i)
			{
				ptr[i] = T(' ');
			}
			ptr[size] = 0;
			set_long_ptr(ptr, size, size);
		}
		else
		{
			for (size_t i = 0; i < size; ++i)
			{
				data_.short_str.buffer[i] = T(' ');
			}
			set_short_size(size);
		}
	}

	basic_string(std::initializer_list<T> il)
	{
		size_t size = il.size();
		if (size <= SSO_CAPACITY)
		{
			size_t i = 0;
			for (const auto& item : il)
			{
				data_.short_str.buffer[i++] = item;
			}
			set_short_size(size);
		}
		else
		{
			T* ptr = new T[size + 1];
			size_t i = 0;
			for (const auto& item : il)
			{
				ptr[i++] = item;
			}
			ptr[size] = 0;
			set_long_ptr(ptr, size, size);
		}
	}

	basic_string(const T* c_str)
	{
		size_t size = strlen_(c_str);
		copy_from(c_str, size);
	}

	basic_string(const basic_string& other) : is_long_(other.is_long_)
	{
		if (is_long_)
		{
			T* ptr = new T[other.data_.long_str.capacity + 1];
			for (size_t i = 0; i < other.data_.long_str.size; ++i)
			{
				ptr[i] = other.data_.long_str.ptr[i];
			}
			ptr[other.data_.long_str.size] = 0;
			set_long_ptr(ptr, other.data_.long_str.size,
						 other.data_.long_str.capacity);
		}
		else
		{
			for (size_t i = 0; i <= other.data_.short_str.size; ++i)
			{
				data_.short_str.buffer[i] = other.data_.short_str.buffer[i];
			}
			data_.short_str.size = other.data_.short_str.size;
		}
	}

	basic_string(basic_string&& dying) noexcept : is_long_(dying.is_long_)
	{
		if (is_long_)
		{
			data_.long_str = dying.data_.long_str;
			dying.data_.long_str.ptr = nullptr;
			dying.data_.long_str.size = 0;
			dying.data_.long_str.capacity = 0;
			dying.is_long_ = false;
			dying.data_.short_str.size = 0;
			dying.data_.short_str.buffer[0] = 0;
		}
		else
		{
			for (size_t i = 0; i <= dying.data_.short_str.size; ++i)
			{
				data_.short_str.buffer[i] = dying.data_.short_str.buffer[i];
			}
			data_.short_str.size = dying.data_.short_str.size;
			dying.data_.short_str.size = 0;
			dying.data_.short_str.buffer[0] = 0;
		}
	}

	~basic_string()
	{
		if (is_long_ && data_.long_str.ptr)
		{
			delete[] data_.long_str.ptr;
		}
	}

	const T* c_str() const { return get_ptr(); }

	size_t size() const { return get_size(); }

	bool is_using_sso() const { return !is_long_; }

	size_t capacity() const { return get_capacity(); }

	basic_string& operator=(basic_string&& other) noexcept
	{
		if (this != &other)
		{
			if (is_long_ && data_.long_str.ptr)
			{
				delete[] data_.long_str.ptr;
			}

			is_long_ = other.is_long_;
			if (is_long_)
			{
				data_.long_str = other.data_.long_str;
				other.data_.long_str.ptr = nullptr;
				other.data_.long_str.size = 0;
				other.data_.long_str.capacity = 0;
				other.is_long_ = false;
				other.data_.short_str.size = 0;
				other.data_.short_str.buffer[0] = 0;
			}
			else
			{
				for (size_t i = 0; i <= other.data_.short_str.size; ++i)
				{
					data_.short_str.buffer[i] = other.data_.short_str.buffer[i];
				}
				data_.short_str.size = other.data_.short_str.size;
				other.data_.short_str.size = 0;
				other.data_.short_str.buffer[0] = 0;
			}
		}
		return *this;
	}

	basic_string& operator=(const T* c_str)
	{
		size_t size = strlen_(c_str);
		if (size <= SSO_CAPACITY)
		{
			if (is_long_ && data_.long_str.ptr)
			{
				delete[] data_.long_str.ptr;
			}
			copy_from(c_str, size);
		}
		else
		{
			if (is_long_ && data_.long_str.capacity >= size)
			{
				for (size_t i = 0; i < size; ++i)
				{
					data_.long_str.ptr[i] = c_str[i];
				}
				data_.long_str.ptr[size] = 0;
				data_.long_str.size = size;
			}
			else
			{
				if (is_long_ && data_.long_str.ptr)
				{
					delete[] data_.long_str.ptr;
				}
				copy_from(c_str, size);
			}
		}
		return *this;
	}

	basic_string& operator=(const basic_string& other)
	{
		if (this != &other)
		{
			if (other.is_long_)
			{
				if (is_long_ &&
					data_.long_str.capacity >= other.data_.long_str.size)
				{
					for (size_t i = 0; i < other.data_.long_str.size; ++i)
					{
						data_.long_str.ptr[i] = other.data_.long_str.ptr[i];
					}
					data_.long_str.ptr[other.data_.long_str.size] = 0;
					data_.long_str.size = other.data_.long_str.size;
				}
				else
				{
					if (is_long_ && data_.long_str.ptr)
					{
						delete[] data_.long_str.ptr;
					}
					T* ptr = new T[other.data_.long_str.capacity + 1];
					for (size_t i = 0; i < other.data_.long_str.size; ++i)
					{
						ptr[i] = other.data_.long_str.ptr[i];
					}
					ptr[other.data_.long_str.size] = 0;
					set_long_ptr(ptr, other.data_.long_str.size,
								 other.data_.long_str.capacity);
				}
			}
			else
			{
				if (is_long_ && data_.long_str.ptr)
				{
					delete[] data_.long_str.ptr;
				}
				for (size_t i = 0; i <= other.data_.short_str.size; ++i)
				{
					data_.short_str.buffer[i] = other.data_.short_str.buffer[i];
				}
				data_.short_str.size = other.data_.short_str.size;
				is_long_ = false;
			}
		}
		return *this;
	}

	friend basic_string<T> operator+(const basic_string<T>& left,
									 const basic_string<T>& right)
	{
		basic_string<T> result;
		size_t left_size = left.size();
		size_t right_size = right.size();
		size_t total_size = left_size + right_size;

		if (total_size <= SSO_CAPACITY)
		{
			for (size_t i = 0; i < left_size; ++i)
			{
				result.data_.short_str.buffer[i] = left.get_ptr()[i];
			}
			for (size_t i = 0; i < right_size; ++i)
			{
				result.data_.short_str.buffer[left_size + i] =
					right.get_ptr()[i];
			}
			result.set_short_size(total_size);
		}
		else
		{
			T* ptr = new T[total_size + 1];
			for (size_t i = 0; i < left_size; ++i)
			{
				ptr[i] = left.get_ptr()[i];
			}
			for (size_t i = 0; i < right_size; ++i)
			{
				ptr[left_size + i] = right.get_ptr()[i];
			}
			ptr[total_size] = 0;
			result.set_long_ptr(ptr, total_size, total_size);
		}

		return result;
	}

	template <typename S>
	friend S& operator<<(S& os, const basic_string& obj)
	{
		const T* ptr = obj.get_ptr();
		for (size_t i = 0; i < obj.size(); ++i)
		{
			os << ptr[i];
		}
		return os;
	}

	template <typename S>
	friend S& operator>>(S& is, basic_string& obj)
	{
		const size_t BUFFER_SIZE = 256;
		T buffer[BUFFER_SIZE];
		size_t total_size = 0;
		basic_string temp;

		while (!is.eof())
		{
			is.read(buffer, BUFFER_SIZE);
			size_t bytes_read = is.gcount();

			size_t new_total_size = total_size + bytes_read;
			if (new_total_size <= SSO_CAPACITY)
			{
				for (size_t i = 0; i < bytes_read; ++i)
				{
					temp.data_.short_str.buffer[total_size + i] = buffer[i];
				}
				temp.set_short_size(new_total_size);
			}
			else
			{
				T* new_ptr = new T[new_total_size + 1];
				const T* old_ptr = temp.is_long_ ? temp.data_.long_str.ptr
												 : temp.data_.short_str.buffer;
				for (size_t i = 0; i < total_size; ++i)
				{
					new_ptr[i] = old_ptr[i];
				}
				for (size_t i = 0; i < bytes_read; ++i)
				{
					new_ptr[total_size + i] = buffer[i];
				}
				new_ptr[new_total_size] = 0;

				if (temp.is_long_ && temp.data_.long_str.ptr)
				{
					delete[] temp.data_.long_str.ptr;
				}
				temp.set_long_ptr(new_ptr, new_total_size, new_total_size);
			}

			total_size = new_total_size;
		}

		obj = std::move(temp);
		return is;
	}

	basic_string& operator+=(const basic_string& other)
	{
		size_t old_size = size();
		size_t other_size = other.size();
		size_t new_size = old_size + other_size;

		if (new_size <= SSO_CAPACITY)
		{
			if (is_long_)
			{
				T buffer[SSO_CAPACITY + 1];
				for (size_t i = 0; i < old_size; ++i)
				{
					buffer[i] = data_.long_str.ptr[i];
				}
				for (size_t i = 0; i < other_size; ++i)
				{
					buffer[old_size + i] = other.get_ptr()[i];
				}
				buffer[new_size] = 0;

				delete[] data_.long_str.ptr;
				for (size_t i = 0; i <= new_size; ++i)
				{
					data_.short_str.buffer[i] = buffer[i];
				}
				set_short_size(new_size);
			}
			else
			{
				for (size_t i = 0; i < other_size; ++i)
				{
					data_.short_str.buffer[old_size + i] = other.get_ptr()[i];
				}
				set_short_size(new_size);
			}
		}
		else
		{
			if (!is_long_ || data_.long_str.capacity < new_size)
			{
				size_t new_capacity = std::max(new_size, old_size * 2);
				T* new_ptr = new T[new_capacity + 1];

				const T* old_ptr = get_ptr();
				for (size_t i = 0; i < old_size; ++i)
				{
					new_ptr[i] = old_ptr[i];
				}
				for (size_t i = 0; i < other_size; ++i)
				{
					new_ptr[old_size + i] = other.get_ptr()[i];
				}
				new_ptr[new_size] = 0;

				if (is_long_ && data_.long_str.ptr)
				{
					delete[] data_.long_str.ptr;
				}
				set_long_ptr(new_ptr, new_size, new_capacity);
			}
			else
			{
				for (size_t i = 0; i < other_size; ++i)
				{
					data_.long_str.ptr[old_size + i] = other.get_ptr()[i];
				}
				data_.long_str.ptr[new_size] = 0;
				data_.long_str.size = new_size;
			}
		}

		return *this;
	}

	basic_string& operator+=(T symbol)
	{
		size_t old_size = size();
		size_t new_size = old_size + 1;

		if (new_size <= SSO_CAPACITY)
		{
			if (is_long_)
			{
				T buffer[SSO_CAPACITY + 1];
				for (size_t i = 0; i < old_size; ++i)
				{
					buffer[i] = data_.long_str.ptr[i];
				}
				buffer[old_size] = symbol;
				buffer[new_size] = 0;

				delete[] data_.long_str.ptr;
				for (size_t i = 0; i <= new_size; ++i)
				{
					data_.short_str.buffer[i] = buffer[i];
				}
				set_short_size(new_size);
			}
			else
			{
				data_.short_str.buffer[old_size] = symbol;
				set_short_size(new_size);
			}
		}
		else
		{
			if (!is_long_ || data_.long_str.capacity < new_size)
			{
				size_t new_capacity = std::max(new_size, old_size * 2);
				T* new_ptr = new T[new_capacity + 1];

				const T* old_ptr = get_ptr();
				for (size_t i = 0; i < old_size; ++i)
				{
					new_ptr[i] = old_ptr[i];
				}
				new_ptr[old_size] = symbol;
				new_ptr[new_size] = 0;

				if (is_long_ && data_.long_str.ptr)
				{
					delete[] data_.long_str.ptr;
				}
				set_long_ptr(new_ptr, new_size, new_capacity);
			}
			else
			{
				data_.long_str.ptr[old_size] = symbol;
				data_.long_str.ptr[new_size] = 0;
				data_.long_str.size = new_size;
			}
		}

		return *this;
	}

	T& operator[](size_t index) noexcept { return get_ptr()[index]; }

	const T& operator[](size_t index) const noexcept
	{
		return get_ptr()[index];
	}

	T& at(size_t index)
	{
		if (index >= size())
		{
			throw std::out_of_range("Wrong index");
		}
		return get_ptr()[index];
	}

	const T& at(size_t index) const
	{
		if (index >= size())
		{
			throw std::out_of_range("Wrong index");
		}
		return get_ptr()[index];
	}

	T* data() { return get_ptr(); }

   private:
	static size_t strlen_(const T* str)
	{
		if (!str)
			return 0;
		size_t len = 0;
		while (str[len] != 0)
		{
			++len;
		}
		return len;
	}
};
}  // namespace bmstu