#pragma once

#include <vector>
#include <iostream>
#include <SFML/System/Vector2.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#define LINE_COLOR Color::Yellow

using namespace sf;
using namespace std;

class sfLine : public Drawable
{
public:
	sfLine(const Vector2f& point1, const Vector2f& point2, float thickness = 5.f) :
		color(LINE_COLOR), thickness(thickness)
	{
		Vector2f direction = point2 - point1;
		Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
		Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

		Vector2f offset = (thickness / 2.f) * unitPerpendicular;

		vertices[0].position = point1 + offset;
		vertices[1].position = point2 + offset;
		vertices[2].position = point2 - offset;
		vertices[3].position = point1 - offset;

		for (int i = 0; i < 4; ++i)
			vertices[i].color = color;
	}

	void draw(RenderTarget& target, RenderStates states) const
	{
		target.draw(vertices, 4, Quads);
	}


private:
	Vertex vertices[4];
	float thickness;
	Color color;
};