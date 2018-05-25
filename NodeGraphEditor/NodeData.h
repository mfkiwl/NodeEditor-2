#pragma once

#include <SFML\Graphics.hpp>

#include "json.hpp"

class NodeConnection
{
public:
	int startNodeID;
	int startNodeOutputPropertyIndex;

	int endNodeID;
	int endNodeInputPropertyIndex;

	NodeConnection();
	NodeConnection(int, int, int, int);


	nlohmann::json serialise() const;
	static NodeConnection deserialise(const nlohmann::json & _j);
};

class NodeData
{
public:
	//Serialised
	int id;
	int nodeTemplateID;
	sf::Vector2f position;

	std::map<int, NodeConnection> inputConnections;
	std::multimap<int, NodeConnection> outputConnections;

public:
	NodeData(int _id, int _nodeTemplateID, const sf::Vector2f & _position);
	~NodeData();

	nlohmann::json serialise() const;
	static NodeData deserialise(const nlohmann::json & _j);
};

