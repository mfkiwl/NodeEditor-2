#include "NodeData.h"



NodeData::NodeData(int _id, int _nodeTemplateID, const sf::Vector2f & _position) : 
	id(_id),
	nodeTemplateID(_nodeTemplateID),
	position(_position)
{
}


NodeData::~NodeData()
{
}

NodeConnection::NodeConnection(int _thisNodeID, int _thisNodeOutputPropertyIndex, int _otherNodeID, int _otherNodeInputPropertyIndex) : 
	thisNodeID(_thisNodeID),
	thisNodeOutputPropertyIndex(_thisNodeOutputPropertyIndex),
	otherNodeID(_otherNodeID), 
	otherNodeInputPropertyIndex(_otherNodeInputPropertyIndex)
{
}
