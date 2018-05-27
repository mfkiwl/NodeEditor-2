#include "Property.h"



Property::Property()
{
}

Property::Property(const std::string & _name, int _type) : name(_name), type(_type)
{
}


Property::~Property()
{
}

nlohmann::json Property::serialise() const
{
	return nlohmann::json();
}

Property Property::deserialise(const nlohmann::json & _j)
{
	return Property{ _j.at("name"), _j.at("type").get<int>() };
}
