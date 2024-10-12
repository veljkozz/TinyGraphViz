#include <iostream>

#include "Node.hpp"

int Node::id_gen = 0;


std::ostream& operator<<(std::ostream& os, const Node& obj) {
	os << "(id=" << obj.id << ", pos=(" << obj.pos.x << "," << obj.pos.y << "))";
	return os;
 }