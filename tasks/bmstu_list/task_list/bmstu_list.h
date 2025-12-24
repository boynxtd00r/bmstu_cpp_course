#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include "abstract_iterator.h"

namespace bmstu
{

template <typename T>
class list
{
	struct node
	{
		node() = default;

		node(node* prev, const T& value, node* next)
			: value_(value), next_node_(next), prev_node_(prev) {}

		T value_;
		node* next_node_ = nullptr;
		node* prev_node_ = nullptr;
	};

   public:
	struct iterator
		: public abstract_iterator<iterator, T, std::bidirectional_iterator_tag>
	{
		using Base = abstract_iterator<iterator, T, std::bidirectional_iterator_tag>;
		using difference_type = typename Base::difference_type;
		using reference = typename Base::reference;
		using pointer = typename Base::pointer;
		
		node* current;

		iterator() : current(nullptr) {}
		iterator(node* node) : current(node) {}

		iterator& operator++() override {
			if (current) {
				current = current->next_node_;
			}
			return *this;
		}

		iterator& operator--() override {
			if (current) {
				current = current->prev_node_;
			}
			return *this;
		}

		iterator operator++(int) override {
			iterator tmp = *this;
			++(*this);
			return tmp;
		}

		iterator operator--(int) override {
			iterator tmp = *this;
			--(*this);
			return tmp;
		}

		iterator& operator+=(const difference_type& n) override {
			if (n > 0) {
				for (difference_type i = 0; i < n && current; ++i) {
					++(*this);
				}
			} else if (n < 0) {
				for (difference_type i = 0; i < -n && current; ++i) {
					--(*this);
				}
			}
			return *this;
		}

		iterator& operator-=(const difference_type& n) override {
			return *this += (-n);
		}

		iterator operator+(const difference_type& n) const override {
			iterator tmp = *this;
			return tmp += n;
		}

		iterator operator-(const difference_type& n) const override {
			iterator tmp = *this;
			return tmp -= n;
		}

		reference operator*() const override {
			return current->value_;
		}

		pointer operator->() const override {
			return &(current->value_);
		}

		bool operator==(const iterator& other) const override {
			return current == other.current;
		}

		bool operator!=(const iterator& other) const override {
			return !(*this == other);
		}

		explicit operator bool() const override { 
			return current != nullptr; 
		}

		difference_type operator-(const iterator& other) const override {
			if (current == other.current) {
				return 0;
			}
			
			difference_type dist = 0;
			iterator temp = other;
			while (temp.current != nullptr && temp.current != current) {
				++temp;
				++dist;
			}
			
			if (temp.current == current) {
				return dist;
			}
			
			dist = 0;
			temp = *this;
			while (temp.current != nullptr && temp.current != other.current) {
				++temp;
				--dist;
			}
			
			return dist;
		}
	};

	struct const_iterator
		: public abstract_iterator<const_iterator, const T, std::bidirectional_iterator_tag>
	{
		using Base = abstract_iterator<const_iterator, const T, std::bidirectional_iterator_tag>;
		using difference_type = typename Base::difference_type;
		using reference = typename Base::reference;
		using pointer = typename Base::pointer;
		
		const node* current;

		const_iterator() : current(nullptr) {}
		const_iterator(const node* node) : current(node) {}
		const_iterator(const iterator& it) : current(it.current) {}  // Conversion from iterator

		const_iterator& operator++() override {
			if (current) {
				current = current->next_node_;
			}
			return *this;
		}

		const_iterator& operator--() override {
			if (current) {
				current = current->prev_node_;
			}
			return *this;
		}

		const_iterator operator++(int) override {
			const_iterator tmp = *this;
			++(*this);
			return tmp;
		}

		const_iterator operator--(int) override {
			const_iterator tmp = *this;
			--(*this);
			return tmp;
		}

		const_iterator& operator+=(const difference_type& n) override {
			if (n > 0) {
				for (difference_type i = 0; i < n && current; ++i) {
					++(*this);
				}
			} else if (n < 0) {
				for (difference_type i = 0; i < -n && current; ++i) {
					--(*this);
				}
			}
			return *this;
		}

		const_iterator& operator-=(const difference_type& n) override {
			return *this += (-n);
		}

		const_iterator operator+(const difference_type& n) const override {
			const_iterator tmp = *this;
			return tmp += n;
		}

		const_iterator operator-(const difference_type& n) const override {
			const_iterator tmp = *this;
			return tmp -= n;
		}

		reference operator*() const override {
			return current->value_;
		}

		pointer operator->() const override {
			return &(current->value_);
		}

		bool operator==(const const_iterator& other) const override {
			return current == other.current;
		}

		bool operator!=(const const_iterator& other) const override {
			return !(*this == other);
		}

		explicit operator bool() const override { 
			return current != nullptr; 
		}

		difference_type operator-(const const_iterator& other) const override {
			if (current == other.current) {
				return 0;
			}
			
			difference_type dist = 0;
			const_iterator temp = other;
			while (temp.current != nullptr && temp.current != current) {
				++temp;
				++dist;
			}
			
			if (temp.current == current) {
				return dist;
			}
			
			dist = 0;
			temp = *this;
			while (temp.current != nullptr && temp.current != other.current) {
				++temp;
				--dist;
			}
			
			return dist;
		}
	};

	list() {
		head_.next_node_ = &tail_;
		tail_.prev_node_ = &head_;
	}

	template <typename InputIt>
	list(InputIt first, InputIt last) : list() {
		for (; first != last; ++first) {
			push_back(*first);
		}
	}

	list(std::initializer_list<T> init) : list(init.begin(), init.end()) {}

	list(const list& other) : list() {
		for (const auto& item : other) {
			push_back(item);
		}
	}

	list(list&& other) noexcept : list() {
		swap(other);
	}

	list& operator+=(list& other) noexcept {
		if (other.empty()) {
			return *this;
		}
		node* last_current = tail_.prev_node_;
		node* first_other = other.head_.next_node_;
		node* last_other = other.tail_.prev_node_;

		last_current->next_node_ = first_other;
		first_other->prev_node_ = last_current;

		last_other->next_node_ = &tail_;
		tail_.prev_node_ = last_other;

		size_ += other.size_;

		other.head_.next_node_ = &other.tail_;
		other.tail_.prev_node_ = &other.head_;
		other.size_ = 0;

		return *this;
	}

	list& operator+=(list&& other) noexcept {
		return *this += other;
	}

	~list() { clear(); }

	list& operator=(const list& other) {
		if (this != &other) {
			list tmp(other);
			swap(tmp);
		}
		return *this;
	}

	list& operator=(list&& other) noexcept {
		if (this != &other) {
			clear();
			swap(other);
		}
		return *this;
	}

	void push_back(const T& value) { insert(end(), value); }

	void push_front(const T& value) { insert(begin(), value); }

	list& reverse() noexcept {
		if (size_ <= 1) {
			return *this;
		}
		node* current = head_.next_node_;
		while (current != &tail_) {
			std::swap(current->prev_node_, current->next_node_);
			current = current->prev_node_;
		}
		std::swap(head_.next_node_, tail_.prev_node_);
		head_.next_node_->prev_node_ = &head_;
		tail_.prev_node_->next_node_ = &tail_;
		return *this;
	}

	void pop_back() {
		if (!empty()) {
			erase(--end());
		}
	}

	void pop_front() {
		if (!empty()) {
			erase(begin());
		}
	}

	iterator insert(iterator pos, const T& value) {
		node* new_node = new node(pos.current->prev_node_, value, pos.current);
		pos.current->prev_node_->next_node_ = new_node;
		pos.current->prev_node_ = new_node;
		++size_;
		return iterator(new_node);
	}

	iterator erase(iterator pos) {
		if (pos == end()) {
			return pos;
		}
		node* to_erase = pos.current;
		iterator ret(to_erase->next_node_);

		to_erase->prev_node_->next_node_ = to_erase->next_node_;
		to_erase->next_node_->prev_node_ = to_erase->prev_node_;

		delete to_erase;
		--size_;
		return ret;
	}

	void clear() {
		while (!empty()) {
			pop_front();
		}
	}

	void swap(list& other) noexcept {
		std::swap(head_.next_node_, other.head_.next_node_);
		std::swap(tail_.prev_node_, other.tail_.prev_node_);
		std::swap(size_, other.size_);

		if (head_.next_node_) {
			head_.next_node_->prev_node_ = &head_;
		}
		if (tail_.prev_node_) {
			tail_.prev_node_->next_node_ = &tail_;
		}
		if (other.head_.next_node_) {
			other.head_.next_node_->prev_node_ = &other.head_;
		}
		if (other.tail_.prev_node_) {
			other.tail_.prev_node_->next_node_ = &other.tail_;
		}
	}

	size_t size() const { return size_; }
	bool empty() const { return size_ == 0; }

	iterator begin() { return iterator(head_.next_node_); }
	iterator end() noexcept { return iterator(&tail_); }
	const_iterator begin() const { return const_iterator(head_.next_node_); }
	const_iterator end() const noexcept { return const_iterator(&tail_); }
	const_iterator cbegin() const { return const_iterator(head_.next_node_); }
	const_iterator cend() const noexcept { return const_iterator(&tail_); }

	const T& operator[](size_t pos) const {
		const_iterator it = cbegin();
		while (pos--) {
			++it;
		}
		return *it;
	}

	T& operator[](size_t pos) {
		iterator it = begin();
		while (pos--) {
			++it;
		}
		return *it;
	}

	friend bool operator==(const list& l, const list& r) {
		if (l.size() != r.size()) {
			return false;
		}
		auto l_it = l.begin();
		auto r_it = r.begin();
		while (l_it != l.end()) {
			if (*l_it != *r_it) {
				return false;
			}
			++l_it;
			++r_it;
		}
		return true;
	}

	friend bool operator!=(const list& l, const list& r) {
		return !(l == r);
	}

	friend std::ostream& operator<<(std::ostream& os, const list& other) {
		os << "{";
		bool first = true;
		for (const auto& item : other) {
			if (!first) {
				os << ", ";
			}
			os << item;
			first = false;
		}
		os << "}";
		return os;
	}

   private:
	size_t size_ = 0;
	node head_;
	node tail_;
};

template <typename T>
void swap(list<T>& lhs, list<T>& rhs) noexcept {
	lhs.swap(rhs);
}

}  // namespace bmstu