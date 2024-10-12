#pragma once

#include <memory>
#include <iostream>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "Util.hpp"

using namespace std;
using namespace sf;

constexpr auto DEFAULT_RADIUS = 8.f;

class Node {
	static int id_gen;

public:
	Vector2f pos;
	int id;
	string label;
	CircleShape shape;

	Node(float r = DEFAULT_RADIUS): id(0) {
		shape = CircleShape(r);
		shape.setOrigin(r, r);
	}


	Node(float posX, float posY, float r = DEFAULT_RADIUS)
		: id(this->id_gen++), pos(Vector2f(posX, posY))
	{
		shape = CircleShape(r);
		shape.setOrigin(r, r);
		shape.setPosition(this->pos);
	}

	 static Node from_id(int id, string label = "", float r = DEFAULT_RADIUS) {
		 Node n;

		 n.shape = CircleShape(r);
		 n.shape.setOrigin(r, r);
		 n.id = id;
		 n.label = label;
		 if (label == "") {
			 n.label = to_string(id);
		 }
		 return n;
	 }

	 friend std::ostream& operator<<(std::ostream& os, const Node& obj);
};