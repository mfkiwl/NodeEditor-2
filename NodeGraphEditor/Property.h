#pragma once

#include <string>

class Property
{
public:
	//Serialised
	std::string name;
	int type;

	//Non-serialised
	float textUnscaledWidth;

public:
	Property(const std::string & _name, int _type);
	~Property();
};

