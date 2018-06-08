#pragma once

#include <string>

#include "json.hpp"

class Property
{
public:
	//Serialised
	///
	/// The display name of the property
	///
	std::string name;

	///
	/// The data type of the property
	///
	int type;

	//Non-serialised
	///
	/// The original (unscaled) width of the property name text
	///
	float textUnscaledWidth;

public:
	///
	/// Constructs an empty property
	///
	Property();

	///
	/// Constructs a property
	/// _name - the name of the property
	/// _type - the datatype of the property
	///
	Property(const std::string & _name, int _type);
	~Property();

	///
	/// Returns a JSON object representing the property
	///
	nlohmann::json serialise() const;

	///
	///  Constructs a property from a JSON object
	///
	static Property deserialise(const nlohmann::json & _j);
};

