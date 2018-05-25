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
	nlohmann::json inputNodeConnectionsJson;

	for (auto & connection : inputConnections)
	{
		inputNodeConnectionsJson.push_back({ { "propertyIndex", connection.first }, { "connection", connection.second.serialise() } });
	}


	nlohmann::json outputNodeConnectionsJson;

	for (auto & connection : outputConnections)
	{
		outputNodeConnectionsJson.push_back({ { "propertyIndex", connection.first }, { "connection", connection.second.serialise() } });
	}


	return { 
		{ "id", id },
		{ "nodeTemplateID", nodeTemplateID },
		{ "position", position },
		{ "inputNodeConnections", inputNodeConnectionsJson },
		{ "outputNodeConnections", outputNodeConnectionsJson } 
	};
}

NodeData NodeData::deserialise(const nlohmann::json & _j)
{

	NodeData constructed = NodeData(_j.at("id").get<int>(), _j.at("nodeTemplateID").get<int>(), _j.at("position").get<sf::Vector2f>());


	/*constructed.nodeConnections.resize(nodeConnectionsJson.size());*/

	/*for (int i = 0; i < nodeConnectionsJson.size(); ++i)
	{
		NodeConnection nodeConnectionDeserialised = NodeConnection::deserialise(nodeConnectionsJson[i]);
		constructed.nodeConnections[i] = nodeConnectionDeserialised;
	}*/

	nlohmann::json inputNodeConnectionsJson = _j.at("inputNodeConnections");

	for (auto & connectionJson : inputNodeConnectionsJson)
	{
		constructed.inputConnections[connectionJson.at("propertyIndex").get<int>()] = NodeConnection::deserialise(connectionJson.at("connection"));
	}


	nlohmann::json outputNodeConnectionsJson = _j.at("outputNodeConnections");

	for (auto & connectionJson : outputNodeConnectionsJson)
	{
		constructed.outputConnections.emplace(std::pair<int, NodeConnection>{ connectionJson.at("propertyIndex").get<int>(), NodeConnection::deserialise(connectionJson.at("connection")) });
		//constructed.outputConnections[connectionJson.at("propertyIndex").get<int>()] = NodeConnection::deserialise(connectionJson.at("connection"));
	}

	return constructed;
}

NodeConnection::NodeConnection() : startNodeID(-1), startNodeOutputPropertyIndex(-1), endNodeID(-1), endNodeInputPropertyIndex(-1)
{
}

NodeConnection::NodeConnection(int _thisNodeID, int _thisNodeOutputPropertyIndex, int _otherNodeID, int _otherNodeInputPropertyIndex) :
	startNodeID(_thisNodeID),
	startNodeOutputPropertyIndex(_thisNodeOutputPropertyIndex),
	endNodeID(_otherNodeID), 
	endNodeInputPropertyIndex(_otherNodeInputPropertyIndex)
{
}

nlohmann::json NodeConnection::serialise() const
{
	return { { "startNode", startNodeID }, { "startNodeOutputPropertyIndex", startNodeOutputPropertyIndex }, { "endNode", endNodeID }, { "endNodeInputPropertyIndex", endNodeInputPropertyIndex } };
}

NodeConnection NodeConnection::deserialise(const nlohmann::json & _j)
{
	return NodeConnection(_j.at("startNode").get<int>(), _j.at("startNodeOutputPropertyIndex").get<int>(), _j.at("endNode").get<int>(), _j.at("endNodeInputPropertyIndex").get<int>());
}
