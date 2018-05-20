#pragma once

#include "json.hpp"
#include <SFML\Graphics.hpp>

using nlohmann::json;

namespace sf
{
	void to_json(json & j, const sf::Vector2f & _val)
	{
		j = { {"x", _val.x}, {"y", _val.y} };
	}
	void from_json(const json & j, sf::Vector2f & _val)
	{
		_val.x = j.at("x").get<float>();
		_val.y = j.at("y").get<float>();
	}
}