#pragma once

#include <SFML\Graphics.hpp>
#include "imgui\imgui.h"

#include <vector>
#include <string>

#include "Camera.h"
#include "Property.h"

class NodeData;

class NodeTemplate
{
private:
	//Serialised
	int id;
	std::string name;
	std::vector<Property> inputs;
	std::vector<Property> outputs;


	//Non-serialised
	//Drawing vars
	sf::Vector2f size;
	float lineHeight;

	void drawProperties(ImDrawList * _drawList, Camera & _camera, const NodeData & _data, float _yOffsetBeforeContent) const;
	void drawNodeConnections(ImDrawList * _drawList, Camera & _camera, const NodeData & _data) const;

	static float getNodeFontSize(Camera & _camera);
/*
	sf::Vector2f getInputJointPos(int propertyIndex, Camera & _camera, const NodeData & _data) const;
	sf::Vector2f getOutputJointPos(int propertyIndex, Camera & _camera, const NodeData & _data) const;*/

public:
	NodeTemplate(int _id, const std::string & _name, const std::vector<Property> & _inputs, const std::vector<Property> & _outputs);
	~NodeTemplate();

	void draw(ImDrawList * _drawList, Camera & _camera, const NodeData & _data, bool _isSelection);

	void recalculateSize();

	sf::FloatRect getHeaderRegion(const Camera & _camera, const NodeData & _data) const;

	/// first: position, second: radius
	std::pair<sf::Vector2f, float> getInputJointCircleRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const;
	/// first: position, second: radius
	std::pair<sf::Vector2f, float> getOutputJointCircleRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const;

	sf::FloatRect getInputJointRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const;
	sf::FloatRect getOutputJointRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const;

	int getInputsCount() const;
	int getOutputsCount() const;

	const Property & getInputProperty(int _inputPropertyIndex) const;
	const Property & getOutputProperty(int _outputPropertyIndex) const;
};

