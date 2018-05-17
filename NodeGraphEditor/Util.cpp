#include "Util.h"

bool circleContainsPoint(std::pair<sf::Vector2f, float> _circle, sf::Vector2f _point)
{
	sf::Vector2f del = _point - _circle.first;
	float mag2 = del.x * del.x + del.y * del.y;

	return mag2 <= _circle.second * _circle.second;
}