#pragma once

#include <vector>

#include <SFML\Graphics.hpp>
#include "imgui\imgui.h"
#include "imgui\imgui-SFML.h"
#include "Camera.h"

bool circleContainsPoint(std::pair<sf::Vector2f, float> _circle, sf::Vector2f _point);

void drawConnectionRaw(ImDrawList * _drawList, Camera & _camera, const sf::Vector2f & _startPos, const sf::Vector2f & _endPos, int _connectionType);