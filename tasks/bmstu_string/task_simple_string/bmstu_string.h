#pragma once

#include <cstring>
#include <exception>
#include <initializer_list>
#include <iostream>

namespace bmstu
{
template <typename T>
class simple_basic_string;

typedef simple_basic_string<char> string;
typedef simple_basic_string<wchar_t> wstring;
typedef simple_basic_string<char16_t> u16string;
typedef simple_basic_string<char32_t> u32string;

template <typename T>
class simple_basic_string
{
   public:
	simple_basic_string() : ptr_(new T[1]{0}), size_(0) {}

	simple_basic_string(size_t size) : ptr_(new T[size + 1]), size_(size)
	{
		for (size_t i = 0; i < size; ++i)
		{
			ptr_[i] = T(' ');
		}
		ptr_[size] = 0;
	}

	simple_basic_string(std::initializer_list<T> il)
		: ptr_(new T[il.size() + 1]), size_(il.size())
	{
		size_t i = 0;
		for (const auto& item : il)
		{
			ptr_[i++] = item;
		}
		ptr_[size_] = 0;
	}

	simple_basic_string(const T* c_str)
	{
		size_ = strlen_(c_str);
		ptr_ = new T[size_ + 1];
		for (size_t i = 0; i < size_; ++i)
		{
			ptr_[i] = c_str[i];
		}
		ptr_[size_] = 0;
	}

	simple_basic_string(const simple_basic_string& other)
		: ptr_(new T[other.size_ + 1]), size_(other.size_)
	{
		for (size_t i = 0; i < size_; ++i)
		{
			ptr_[i] = other.ptr_[i];
		}
		ptr_[size_] = 0;
	}

	simple_basic_string(simple_basic_string&& dying)
		: ptr_(dying.ptr_), size_(dying.size_)
	{
		dying.ptr_ = new T[1]{0};
		dying.size_ = 0;
	}

	~simple_basic_string() { clean_(); }

	const T* c_str() const { return ptr_; }

	size_t size() const { return size_; }

	simple_basic_string& operator=(simple_basic_string&& other)
	{
		if (this != &other)
		{
			clean_();
			ptr_ = other.ptr_;
			size_ = other.size_;
			other.ptr_ = new T[1]{0};
			other.size_ = 0;
		}
		return *this;
	}

	simple_basic_string& operator=(const T* c_str)
	{
		size_t new_size = strlen_(c_str);
		T* new_ptr = new T[new_size + 1];

		for (size_t i = 0; i < new_size; ++i)
		{
			new_ptr[i] = c_str[i];
		}
		new_ptr[new_size] = 0;

		delete[] ptr_;

		ptr_ = new_ptr;
		size_ = new_size;

		return *this;
	}

	simple_basic_string& operator=(const simple_basic_string& other)
	{
		if (this != &other)
		{
			T* new_ptr = new T[other.size_ + 1];
			for (size_t i = 0; i < other.size_; ++i)
			{
				new_ptr[i] = other.ptr_[i];
			}
			new_ptr[other.size_] = 0;

			clean_();
			ptr_ = new_ptr;
			size_ = other.size_;
		}
		return *this;
	}

	friend simple_basic_string<T> operator+(const simple_basic_string<T>& left,
											const simple_basic_string<T>& right)
	{
		simple_basic_string<T> result;
		result.size_ = left.size_ + right.size_;
		result.ptr_ = new T[result.size_ + 1];

		for (size_t i = 0; i < left.size_; ++i)
		{
			result.ptr_[i] = left.ptr_[i];
		}
		for (size_t i = 0; i < right.size_; ++i)
		{
			result.ptr_[left.size_ + i] = right.ptr_[i];
		}
		result.ptr_[result.size_] = 0;

		return result;
	}

	template <typename S>
	friend S& operator<<(S& os, const simple_basic_string& obj)
	{
		for (size_t i = 0; i < obj.size_; ++i)
		{
			os << obj.ptr_[i];
		}
		return os;
	}

	template <typename S>
	friend S& operator>>(S& is, simple_basic_string& obj)
	{
		obj.clean_();
		const size_t BUFFER_SIZE = 256;
		T buffer[BUFFER_SIZE];
		size_t total_size = 0;

		while (!is.eof())
		{
			is.read(buffer, BUFFER_SIZE);
			size_t bytes_read = is.gcount();

			T* new_ptr = new T[total_size + bytes_read + 1];
			for (size_t i = 0; i < total_size; ++i)
			{
				new_ptr[i] = obj.ptr_[i];
			}
			for (size_t i = 0; i < bytes_read; ++i)
			{
				new_ptr[total_size + i] = buffer[i];
			}

			delete[] obj.ptr_;
			obj.ptr_ = new_ptr;
			total_size += bytes_read;
		}

		obj.ptr_[total_size] = 0;
		obj.size_ = total_size;

		return is;
	}

	simple_basic_string& operator+=(const simple_basic_string& other)
	{
		size_t new_size = size_ + other.size_;
		T* new_ptr = new T[new_size + 1];

		for (size_t i = 0; i < size_; ++i)
		{
			new_ptr[i] = ptr_[i];
		}

		for (size_t i = 0; i < other.size_; ++i)
		{
			new_ptr[size_ + i] = other.ptr_[i];
		}

		new_ptr[new_size] = 0;

		delete[] ptr_;

		ptr_ = new_ptr;
		size_ = new_size;

		return *this;
	}

	simple_basic_string& operator+=(T symbol)
	{
		size_t new_size = size_ + 1;
		T* new_ptr = new T[new_size + 1];

		for (size_t i = 0; i < size_; ++i)
		{
			new_ptr[i] = ptr_[i];
		}

		new_ptr[size_] = symbol;
		new_ptr[new_size] = 0;

		delete[] ptr_;

		ptr_ = new_ptr;
		size_ = new_size;

		return *this;
	}

	T& operator[](size_t index) noexcept { return ptr_[index]; }

	const T& operator[](size_t index) const noexcept { return ptr_[index]; }

	T& at(size_t index)
	{
		if (index >= size_)
		{
			throw std::out_of_range("Wrong index");
		}
		return ptr_[index];
	}

	const T& at(size_t index) const
	{
		if (index >= size_)
		{
			throw std::out_of_range("Wrong index");
		}
		return ptr_[index];
	}

	T* data() { return ptr_; }

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

	void clean_()
	{
		if (ptr_)
		{
			delete[] ptr_;
			ptr_ = nullptr;
		}
		size_ = 0;
	}

	T* ptr_ = nullptr;
	size_t size_ = 0;
};
}  // namespace bmstu