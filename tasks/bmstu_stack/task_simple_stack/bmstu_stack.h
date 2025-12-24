#pragma once

#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

namespace bmstu
{
template <typename T>
class stack
{
   public:
	stack() : data_(nullptr), capacity_(0u), size_(0u) {}

	stack(const stack& other) : capacity_(0u), size_(0u)
	{
		if (other.size_ > 0)
		{
			reserve(other.size_);
			for (size_t i = 0; i < other.size_; ++i)
			{
				new (data_ + i) T(other.data_[i]);
			}
			size_ = other.size_;
		}
	}

	stack& operator=(const stack& other)
	{
		if (this != &other)
		{
			clear();
			if (other.size_ > 0)
			{
				reserve(other.size_);
				for (size_t i = 0; i < other.size_; ++i)
				{
					new (data_ + i) T(other.data_[i]);
				}
			}
			size_ = other.size_;
		}
		return *this;
	}

	stack(stack&& other) noexcept
		: data_(other.data_), capacity_(other.capacity_), size_(other.size_)
	{
		other.data_ = nullptr;
		other.capacity_ = 0;
		other.size_ = 0;
	}

	stack& operator=(stack&& other) noexcept
	{
		if (this != &other)
		{
			clear();
			::operator delete(data_);
			data_ = other.data_;
			capacity_ = other.capacity_;
			size_ = other.size_;
			other.data_ = nullptr;
			other.capacity_ = 0;
			other.size_ = 0;
		}
		return *this;
	}

	bool empty() const noexcept { return size_ == 0; }

	size_t size() const noexcept { return size_; }

	~stack()
	{
		clear();
		::operator delete(data_);
	}

	template <typename... Args>
	void emplace(Args&&... args)
	{
		reserve(size_ + 1);
		new (data_ + size_) T(std::forward<Args>(args)...);
		++size_;
	}

	void push(T&& value)
	{
		reserve(size_ + 1);
		new (data_ + size_) T(std::move(value));
		++size_;
	}

	void push(const T& value)
	{
		reserve(size_ + 1);
		new (data_ + size_) T(value);
		++size_;
	}

	void pop()
	{
		if (empty())
		{
			throw std::underflow_error("Stack is empty");
		}
		--size_;
		data_[size_].~T();
	}

	T& top()
	{
		if (empty())
		{
			throw std::underflow_error("Stack is empty");
		}
		return data_[size_ - 1];
	}

	const T& top() const
	{
		if (empty())
		{
			throw std::underflow_error("Stack is empty");
		}
		return data_[size_ - 1];
	}

	void clear() noexcept
	{
		for (size_t i = 0; i < size_; ++i)
		{
			data_[i].~T();
		}
		size_ = 0;
	}

   private:
	void reserve(size_t new_capacity)
	{
		if (new_capacity <= capacity_)
			return;

		T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
		for (size_t i = 0; i < size_; ++i)
		{
			new (new_data + i) T(std::move(data_[i]));
			data_[i].~T();
		}
		::operator delete(data_);
		data_ = new_data;
		capacity_ = new_capacity;
	}

	T* data_;
	size_t capacity_;
	size_t size_;
};
}  // namespace bmstu