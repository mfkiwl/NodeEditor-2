#pragma once

#include <vector>

#include <SFML\Graphics.hpp>
#include "imgui\imgui.h"
#include "imgui\imgui-SFML.h"
#include "Camera.h"

///
/// Returns whether a circle contains a point
/// _circle - the circle
/// _point - the point
///
bool circleContainsPoint(std::pair<sf::Vector2f, float> _circle, sf::Vector2f _point);

///
/// Draws a connection line between two points
/// _drawList - the ImGui draw list
/// _camera - the current camera
/// _startPos - the start position of the connection
/// _endPos - the end position of the connection
/// _connectionType - the datatype of the connection
///
void drawConnectionRaw(ImDrawList * _drawList, Camera & _camera, const sf::Vector2f & _startPos, const sf::Vector2f & _endPos, int _connectionType);

///
/// Returns the colour for a given connection type
/// _connectionType - the datatype of the connection
///
ImU32 getConnectionColor(int _connectionType);