#pragma once
#include <memory>

template<typename T>
class Node {
	std::shared_ptr<T> data;
	std::shared_ptr<Node> next;
public:
	Node() {
		data = nullptr;
		next = nullptr;
	}
	Node(std::shared_ptr<T> data) {
		this->data = data;
	}
	Node(std::shared_ptr<T> data, std::shared_ptr<Node<T>> next):Node(data) {
		this->next = next;
	}
	std::shared_ptr<T> get_data() {
		return data;
	}
	std::shared_ptr<Node<T>> get_next() {
		return next;
	}
	void set_data(std::shared_ptr<T> data) {
		this->data = data;
	}
	void set_next(std::shared_ptr<Node<T>> next) {
		this->next = next;
	}
};

template<typename T>
class CircularList{
	std::shared_ptr<Node<T>> last;
public:
	CircularList() {
		last = nullptr;
	}
	void add_back(std::shared_ptr<T> data) {			//inserts to the end node, the current node becomes the second to last
		std::shared_ptr<Node<T>> current = last;
		if (last == nullptr) {							//if list is empty
			last = std::make_shared<Node<T>>(data);
			last->set_next(last);
		}
		else {
			std::shared_ptr<Node<T>> head = last->get_next();
			std::shared_ptr<Node<T>> new_node = std::make_shared<Node<T>>(data, head);
			last->set_next(new_node);
			last = new_node;
		}
	}
	void add_back(T node_data) {						//constructs a shared ptr if one was not given as argument	
		std::shared_ptr<T> data = std::make_shared<T>(node_data);
		add_ptr_back(data);
	}
	bool isempty() {
		return last == nullptr;
	}
	std::shared_ptr<Node<T>> get_last() {
		return last;
	}
};