#pragma once

#include <exception>
#include <iostream>
#include <initializer_list>
#include <cstring>

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
	/// Конструктор по умолчанию
	simple_basic_string() : ptr_(new T[1]{0}), size_(0) {}

	simple_basic_string(size_t size) : ptr_(new T[size + 1]), size_(size) {}

	simple_basic_string(std::initializer_list<T> il)
		: ptr_(new T[il.size() + 1]), size_(il.size())
	{
	}

	/// Конструктор с параметром си-с
	simple_basic_string(const T* c_str) {}

	/// Конструктор копирования
	simple_basic_string(const simple_basic_string& other) {}

	/// Перемещающий конструктор
	simple_basic_string(simple_basic_string&& dying) {}

	/// Деструктор
	~simple_basic_string() {}

    /// Геттер на си-строку
    const T* c_str() const { return ptr_; }

    size_t size() const { return size_; }

	/// Оператор копирующего присваивания
	simple_basic_string& operator=(simple_basic_string&& other)
	{
		return *this;
	}

	/// Оператор копирующего присваивания си строки
	simple_basic_string& operator=(const T* c_str) { return *this; }

	/// Оператор копирующего присваивания
	simple_basic_string& operator=(const simple_basic_string& other)
	{
		return *this;
	}

	friend simple_basic_string<T> operator+(const simple_basic_string<T>& left,
											const simple_basic_string<T>& right)
	{
		return {};
	}

	template <typename S>
	friend S& operator<<(S& os, const simple_basic_string& obj)
	{
		return os;
	}

	template <typename S>
	friend S& operator>>(S& is, simple_basic_string& obj)
	{
		return is;
	}

	simple_basic_string& operator+=(const simple_basic_string& other)
	{
		return *this;
	}

	simple_basic_string& operator+=(T symbol) { return *this; }

    T& operator[](size_t index) noexcept 
    { 
        return ptr_[index]; 
    }

    const T& operator[](size_t index) const noexcept 
    { 
        return ptr_[index]; 
    }

    T& at(size_t index) 
    { 
        if (index >= size_) {
            throw std::out_of_range("Wrong index");
        }
        return ptr_[index]; 
    }

    const T& at(size_t index) const
    { 
        if (index >= size_) {
            throw std::out_of_range("Wrong index");
        }
        return ptr_[index]; 
    }

    T* data() { return ptr_; }

   private:
    static size_t strlen_(const T* str) 
    {
        if (!str) return 0;
        size_t len = 0;
        while (str[len] != 0) {
            ++len;
        }
        return len;
    }

    void clean_() 
    {
        if (ptr_) {
            delete[] ptr_;
            ptr_ = nullptr;
        }
        size_ = 0;
    }

    T* ptr_ = nullptr;
    size_t size_ = 0;
};
}  // namespace bmstu