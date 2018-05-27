#pragma once

#include <string>

#include "json.hpp"

class Property
{
public:
	//Serialised
	std::string name;
	int type;

	//Non-serialised
	float textUnscaledWidth;

public:
	Property();
	Property(const std::string & _name, int _type);
	~Property();

	nlohmann::json serialise() const;
	static Property deserialise(const nlohmann::json & _j);
};

