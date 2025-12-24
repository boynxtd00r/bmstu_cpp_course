#pragma once
#include <cstdint>
#include <exception>
#include <utility>

namespace bmstu
{
struct nullopt_t
{
	constexpr explicit nullopt_t(int) {}
};
constexpr nullopt_t nullopt{0};

class bad_optional_access : public std::exception
{
   public:
	using exception::exception;

	const char* what() const noexcept override { return "Bad optional access"; }
};

template <typename T>
class optional
{
   public:
	optional() = default;

	optional(const T& value)
        {
            is_initialized_ = true;
            T * val = new (&data_[0])T{value};
        }

	optional( T&& value)
        {
            is_initialized_ = true;
            T* val = new (&data_[0])T{std::move(value)};
        }

	optional(const optional& other)
        {
            if (other.is_initialized_) {
                T* val = new (&data_[0])T{other.value()};
                is_initialized_ = true;
            }
        }

	optional(optional&& other) noexcept
        {
            if (other.is_initialized_) {
                T* val = new (&data_[0])T{std::move(other.value())};
                is_initialized_ = true;
            }
        }

    optional(nullopt_t) noexcept {}

	optional& operator=(const T& value) {
        if (has_value()) {
            this->value() = value;
        } else {
            T* val = new (&data_[0])T{value};
            is_initialized_ = true;
        }
        return *this;
    }

	optional& operator=(T&& value) {
        if (has_value()) {
            this->value() = std::move(value);
        } else {
            T* val = new (&data_[0])T{std::move(value)};
            is_initialized_ = true;
        }
        return *this;
    }

	optional& operator=(const optional& other) {
        if (this != &other) {
            if (other.has_value()) {
                if (has_value()) {
                    value() = other.value();
                } else {
                    T* val = new (&data_[0])T{other.value()};
                    is_initialized_ = true;
                }
            } else {
                reset();
            }
        }
        return *this;
    }

	optional& operator=(optional&& other) {
        if (this != &other) {
            if (other.has_value()) {
                if (has_value()) {
                    value() = std::move(other.value());
                } else {
                    T* val = new (&data_[0])T{std::move(other.value())};
                    is_initialized_ = true;
                }
            } else {
                reset();
            }
        }
        return *this;
    }

	T& operator*() &
	{
		return *reinterpret_cast<T*>(&data_[0]);
	}

	const T& operator*() const&
	{
		return *reinterpret_cast<const T*>(&data_[0]);
	}

	T* operator->() { 
        return reinterpret_cast<T*>(&data_[0]);
    }

	const T* operator->() const { 
        return reinterpret_cast<const T*>(&data_[0]);
    }

	T&& operator*() &&
	{
		return std::move(*reinterpret_cast<T*>(&data_[0]));
	}

	T& value() &
	{
		if (!has_value()) {
            throw bad_optional_access();
        }
        return **this;
	}

	const T& value() const&
	{
		if (!has_value()) {
            throw bad_optional_access();
        }
        return **this;
	}

	T&& value() &&
	{
		if (!has_value()) {
            throw bad_optional_access();
        }
        return std::move(**this);
	}

	template <typename... Args>
	void emplace(Args&&... args)
	{
        reset();
        T* val = new (&data_[0]) T(std::forward<Args>(args)...);
        is_initialized_ = true;
	}

	void reset()
        {
            if (is_initialized_)
            {
                reinterpret_cast<T*>(&data_[0])->~T();
                is_initialized_ = false;
            }
        }

	~optional() { reset(); }

	bool has_value() const { return is_initialized_; };

    bool operator==(const optional& other) const {
        if (has_value() != other.has_value()) return false;
        if (!has_value()) return true;
        return value() == other.value();
    }

    bool operator!=(const optional& other) const {
        return !(*this == other);
    }

    template <typename U>
    std::enable_if_t<std::is_convertible_v<U, T>, bool>
    operator==(const U& other) const {
        return has_value() && value() == other;
    }

    template <typename U>
    std::enable_if_t<std::is_convertible_v<U, T>, bool>
    operator!=(const U& other) const {
        return !(*this == other);
    }

    bool operator==(nullopt_t) const {
        return !has_value();
    }

    bool operator!=(nullopt_t) const {
        return has_value();
    }

   private:
	alignas(T) uint8_t data_[sizeof(T)];
	bool is_initialized_ = false;
};

template<typename T, typename U>
std::enable_if_t<std::is_convertible_v<U, T>, bool>
operator==(const U& lhs, const optional<T>& rhs) {
    return rhs == lhs;
}

template<typename T, typename U>
std::enable_if_t<std::is_convertible_v<U, T>, bool>
operator!=(const U& lhs, const optional<T>& rhs) {
    return rhs != lhs;
}

template<typename T>
bool operator==(const char* lhs, const optional<T>& rhs) {
    return rhs.has_value() && rhs.value() == lhs;
}

template<typename T>
bool operator!=(const char* lhs, const optional<T>& rhs) {
    return !(lhs == rhs);
}

template<typename T>
bool operator==(nullopt_t, const optional<T>& rhs) {
    return !rhs.has_value();
}

template<typename T>
bool operator!=(nullopt_t, const optional<T>& rhs) {
    return rhs.has_value();
}
}  // namespace bmstu