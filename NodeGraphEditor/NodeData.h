#pragma once

#include <SFML\Graphics.hpp>

#include "json.hpp"

class NodeConnection
{
public:
	///
	/// The ID of the start node
	///
	int startNodeID;
	
	///
	/// The index of the start node's output property
	///
	int startNodeOutputPropertyIndex;

	///
	/// The ID of the end node
	///
	int endNodeID;

	///
	/// The index of the end node's input property
	///
	int endNodeInputPropertyIndex;

	///
	/// Constructs an empty node connection
	///
	NodeConnection();

	///
	/// Constructs a node connection
	///
	NodeConnection(int, int, int, int);

	///
	/// Returns a JSON object representing the node connection
	///
	nlohmann::json serialise() const;

	///
	///  Constructs a node connection from a JSON object
	///
	static NodeConnection deserialise(const nlohmann::json & _j);
};

class NodeData
{
public:
	//Serialised
	///
	/// The ID of the NodeData
	///
	int id;

	///
	/// The ID of the associated node template
	///
	int nodeTemplateID;

	///
	/// Returns the position of the node
	///
	sf::Vector2f position;

	///
	/// A map of the node's input connections
	///
	std::map<int, NodeConnection> inputConnections;

	///
	/// A map of the node's output connections
	///
	std::multimap<int, NodeConnection> outputConnections;

public:
	///
	/// Constructs a NodeData object
	/// _id - the ID of the node
	/// _nodeTemplateID - the ID of the associated node template
	/// _position - the position of the node
	///
	NodeData(int _id, int _nodeTemplateID, const sf::Vector2f & _position);
	~NodeData();

	///
	/// Returns a JSON object representing the node
	///
	nlohmann::json serialise() const;

	///
	///  Constructs a node from a JSON object
	///
	static NodeData deserialise(const nlohmann::json & _j);
};

