#include <iostream>
#include <vector>

#include "Util.hpp"

float Eps = 0.001f;
float FRAMERATE = 0;

using namespace std;

float Euclidian(Vector2f p1, Vector2f p2) {
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;

	float dist = sqrt(dx * dx + dy * dy);

	return dist;
}

Vector2f repulsive(Vector2f p1, Vector2f p2, float l) {
	// Possible optimization, inverse square root
	float dist = max(Euclidian(p1, p2), Eps);
	
	// calculate unit vector from p2 to p1
	Vector2f unit = Vector2f(p1.x - p2.x, p1.y - p2.y);
	unit /= dist;

	// l^2 / dist
	float force = (l*l) / dist;
	return unit * force;
}

Vector2f attractive(Vector2f p1, Vector2f p2, float l) {
	// Possible optimization, inverse square root
	float dist = max(Euclidian(p1, p2), Eps);

	// unit vector from p1 to p2
	Vector2f unit = Vector2f(p2.x - p1.x, p2.y - p1.y);
	unit /= dist;

	// dist^2 / l
	float force = (dist*dist) / l;
	return unit * force;
}

FruchtermanParams calcFruchtParams(const int num_nodes, float C) {
	float area = CANVAS_HEIGHT * CANVAS_HEIGHT;
	float L = C * sqrt(area / num_nodes);
	return FruchtermanParams{ L, 0.99f, CANVAS_WIDTH, CANVAS_HEIGHT };
}