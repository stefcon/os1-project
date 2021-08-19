#ifndef LIST_H_
#define LIST_H_

#include "utils.h"

template <class T> class List {

	struct Node {
		T info;
		Node* next, * prev;
		Node(const T& i, Node* n = nullptr, Node* p = nullptr) : info(i), next(n), prev(p) {}
	};

	Node* head_, * tail_;
	unsigned size_;
	void clear() volatile;		// Helper for deleting the list

	List(const List&);					// List cannot be copied in any way
	List& operator=(const List&);

public:
	List() : head_(nullptr), tail_(nullptr), size_(0) {}
	~List() { clear(); }

	// Methods for inserting, removing and accessing elements of the list
	void push_front(const T& t) volatile{
		if (head_ == nullptr) {
			head_ = tail_ = new Node(t);
		} else {
			head_ = new Node(t, head_);
			head_->next->prev = head_;
		}

		++size_;

	}

	void push_back(const T& t) volatile {
		if (head_ == nullptr) {
			head_ = tail_ = new Node(t);
		} else {
			tail_ = new Node(t, nullptr, tail_);
			tail_->prev->next = tail_;

		}
		++size_;
	}

	void remove_node(Node* node) volatile {
		if (node) {
			if (node->next)
				node->next->prev = node->prev;
			if (node->prev)
				node->prev->next = node->next;
			if (node == head_)
				head_ = node->next;
			if (node == tail_)
				tail_ = node->prev;

			delete node;
			--size_;
		}
	}

	void pop_front() volatile {
		remove_node(head_);
	}

	void pop_back() volatile {
		remove_node(tail_);
	}

	T& front() volatile {
		return head_->info;
	}

	T& back() volatile {
		return tail_->info;
	}

	unsigned size() const {
		return size_;
	}

	bool empty() const volatile {
		return head_ == nullptr;
	}

	// Iterator class definition
	class Iterator {
	public:
		Iterator(Node* curr = nullptr) : current(curr) {}

		bool operator==(const Iterator& iter) {
			return this->current == iter.current;
		}

		bool operator!=(const Iterator& iter) {
			return this->current != iter.current;
		}

		T& operator*() {
			return current->info;
		}

		const T& operator*() const {
			return current->info;
		}

		Iterator& operator++() {
			current = current->next;
			return *this;
		}

		Iterator operator++(int) {
			Iterator old(current);
			current = current->next;
			return old;
		}

	private:
		friend class List<T>;
		Node* current;
	};

	Iterator begin() volatile {
		return Iterator(head_);
	}

	Iterator end() volatile {
		return Iterator();
	}

	// Inserts new element before a given iterator
	Iterator insert(Iterator iter, const T& val) volatile {
		if (iter == begin()) {
			push_front(val);
			return Iterator(head_);
		} else if (iter == end()) {
			push_back(val);
			return Iterator(tail_);
		} else {

			Node* new_node = new Node(val);
			new_node->next = iter.current;
			new_node->prev = iter.current->prev;
			iter.current->prev->next = new_node;
			iter.current->prev = new_node;
			return Iterator(new_node);
		}
	}

	Iterator remove_iterator(Iterator iter) volatile {
		Iterator oldIterator = iter++;
		remove_node(oldIterator.current);
		return iter;
	}



};

template <class T>
void List<T>::clear() volatile {
	while (head_) {
		Node* old = head_;
		head_ = head_->next;
		delete old;
	}
	head_ = tail_ = nullptr;
}


#endif /* LIST_H_ */
