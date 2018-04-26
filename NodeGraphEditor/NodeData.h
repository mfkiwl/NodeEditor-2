#pragma once

#include <SFML\Graphics.hpp>

class NodeConnection
{
public:
	int thisNodeID;
	int thisNodeOutputPropertyIndex;

	int otherNodeID;
	int otherNodeInputPropertyIndex;

	NodeConnection(int, int, int, int);
};

class NodeData
{
public:
	//Serialised
	int id;
	int nodeTemplateID;
	sf::Vector2f position;

	std::vector<NodeConnection> nodeConnections;

public:
	NodeData(int _id, int _nodeTemplateID, const sf::Vector2f & _position);
	~NodeData();
};

