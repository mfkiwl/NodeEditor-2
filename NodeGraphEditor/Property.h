#pragma once

#include <string>

class Property
{
public:
	//Serialised
	std::string name;

	//Non-serialised
	float textUnscaledWidth;

public:
	Property(const std::string & _name);
	~Property();
};

