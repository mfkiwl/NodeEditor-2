#include "Util.h"

#include "Program.h"

bool circleContainsPoint(std::pair<sf::Vector2f, float> _circle, sf::Vector2f _point)
{
	sf::Vector2f del = _point - _circle.first;
	float mag2 = del.x * del.x + del.y * del.y;

	return mag2 <= _circle.second * _circle.second;
}

void drawConnectionRaw(ImDrawList * _drawList, Camera & _camera, const sf::Vector2f & _startPos, const sf::Vector2f & _endPos, int _connectionType)
{
	auto c1 = _startPos + 0.3f * (_endPos - _startPos);
	c1.y = _startPos.y;

	auto c2 = _startPos + 0.3f * (_endPos - _startPos);
	c2.y = _endPos.y;

	_drawList->AddBezierCurve(_startPos, c1, c2, _endPos, getConnectionColor(_connectionType), 1.f);
}

ImU32 getConnectionColor(int _connectionType)
{
	return ImColor::HSV(_connectionType / (float)Program::get().propertyTypeCount, 1.0f, 1.0f);
}
