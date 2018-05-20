#include "NodeData.h"

#include "JsonExtensions.h"


NodeData::NodeData(int _id, int _nodeTemplateID, const sf::Vector2f & _position) : 
	id(_id),
	nodeTemplateID(_nodeTemplateID),
	position(_position)
{
}


NodeData::~NodeData()
{
}

nlohmann::json NodeData::serialise() const
{
	nlohmann::json nodeConnectionsJson;

	for (int i = 0; i < nodeConnections.size(); ++i)
	{
		nodeConnectionsJson[i] = nodeConnections[i].serialise();
	}

	return { { "id", id }, { "nodeTemplateID", nodeTemplateID }, { "position", position }, { "nodeConnections", nodeConnectionsJson } };
}

NodeData NodeData::deserialise(const nlohmann::json & _j)
{

	NodeData constructed = NodeData(_j.at("id").get<int>(), _j.at("nodeTemplateID").get<int>(), _j.at("position").get<sf::Vector2f>());

	nlohmann::json nodeConnectionsJson = _j.at("nodeConnections");

	constructed.nodeConnections.resize(nodeConnectionsJson.size());

	for (int i = 0; i < nodeConnectionsJson.size(); ++i)
	{
		NodeConnection nodeConnectionDeserialised = NodeConnection::deserialise(nodeConnectionsJson[i]);
		constructed.nodeConnections[i] = nodeConnectionDeserialised;
	}

	return constructed;
}

NodeConnection::NodeConnection() : thisNodeID(-1), thisNodeOutputPropertyIndex(-1), otherNodeID(-1), otherNodeInputPropertyIndex(-1)
{
}

NodeConnection::NodeConnection(int _thisNodeID, int _thisNodeOutputPropertyIndex, int _otherNodeID, int _otherNodeInputPropertyIndex) :
	thisNodeID(_thisNodeID),
	thisNodeOutputPropertyIndex(_thisNodeOutputPropertyIndex),
	otherNodeID(_otherNodeID), 
	otherNodeInputPropertyIndex(_otherNodeInputPropertyIndex)
{
}

nlohmann::json NodeConnection::serialise() const
{
	return { { "thisNode", thisNodeID }, { "thisNodeOutputPropertyIndex", thisNodeOutputPropertyIndex }, { "otherNode", otherNodeID }, { "otherNodeInputPropertyIndex", otherNodeInputPropertyIndex } };
}

NodeConnection NodeConnection::deserialise(const nlohmann::json & _j)
{
	return NodeConnection(_j.at("thisNode").get<int>(), _j.at("thisNodeOutputPropertyIndex").get<int>(), _j.at("otherNode").get<int>(), _j.at("otherNodeInputPropertyIndex").get<int>());
}
