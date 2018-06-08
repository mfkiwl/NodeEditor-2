#pragma once

#include <SFML\Graphics.hpp>
#include "imgui\imgui.h"
#include "json.hpp"

#include <vector>
#include <string>

#include "Camera.h"
#include "Property.h"

class NodeData;

class NodeTemplate
{
public:
	//Serialised
	///
	/// The ID of the node template
	///
	int id;

	///
	/// The display name of the node template
	///
	std::string name;

	///
	/// A vector of the node template's input properties
	///
	std::vector<Property> inputs;

	///
	/// A vector of the node template's output properties
	///
	std::vector<Property> outputs;

private:
	//Non-serialised
	//Drawing vars

	///
	/// The display size of the node
	///
	sf::Vector2f size;

	///
	/// The height of each text line
	///
	float lineHeight;

	void drawProperties(ImDrawList * _drawList, Camera & _camera, const NodeData & _data, float _yOffsetBeforeContent) const;
	void drawNodeConnections(ImDrawList * _drawList, Camera & _camera, const NodeData & _data) const;

	static float getNodeFontSize(Camera & _camera);

	void recalculateSize();

public:
	///
	/// Constructs a node template
	/// _id - the ID of the node template
	/// _name - the display name of the node template
	/// _inputs - the inputs of the node template
	/// _outputs - the outputs of the node template
	///
	NodeTemplate(int _id, const std::string & _name, const std::vector<Property> & _inputs, const std::vector<Property> & _outputs);
	~NodeTemplate();

	///
	/// Returns a JSON object representing the node template
	///
	nlohmann::json serialise() const;

	///
	/// Constructs a node template from a JSON object
	///
	static NodeTemplate deserialise(const nlohmann::json & _j);

	///
	/// Draws an instance of the node template
	/// _drawList - the ImGui draw list
	/// _camera - the current camera
	/// _data - the node instance
	/// _isSelection - is the node selected
	///
	void draw(ImDrawList * _drawList, Camera & _camera, const NodeData & _data, bool _isSelection);

	///
	/// Returns the rectangle describing the header region of the node
	/// _camera - the current camera
	/// _data - the node instance
	///
	sf::FloatRect getHeaderRegion(const Camera & _camera, const NodeData & _data) const;

	///
	/// Returns the circle region of a given input joint
	/// _propertyIndex - the index of the input property
	/// _camera - the current camera
	/// _data - the node instance
	///
	std::pair<sf::Vector2f, float> getInputJointCircleRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const;

	///
	/// Returns the circle region of a given output joint
	/// _propertyIndex - the index of the output property
	/// _camera - the current camera
	/// _data - the node instance
	///
	std::pair<sf::Vector2f, float> getOutputJointCircleRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const;

	///
	/// Returns the rectangle region for a given input joint
	/// _propertyIndex - the index of the output property
	/// _camera - the current camera
	/// _data - the node instance
	///
	sf::FloatRect getInputJointRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const;

	///
	/// Returns the rectangle region for a given output joint
	/// _propertyIndex - the index of the output property
	/// _camera - the current camera
	/// _data - the node instance
	///
	sf::FloatRect getOutputJointRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const;

	///
	/// Returns the number of input properties for this node template
	///
	int getInputsCount() const;

	///
	/// Returns the number of output properties for this node template
	///
	int getOutputsCount() const;

	///
	/// Returns an input property
	/// _inputPropertyIndex - the index of the input property
	///
	const Property & getInputProperty(int _inputPropertyIndex) const;

	///
	/// Returns an output property
	/// _inputPropertyIndex - the index of the output property
	///
	const Property & getOutputProperty(int _outputPropertyIndex) const;
};

