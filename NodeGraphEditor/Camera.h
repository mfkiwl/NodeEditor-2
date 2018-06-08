#pragma once

#include <SFML\Graphics.hpp>

class Camera
{
private:
	sf::Vector2f windowPosition;
	sf::Vector2f position;
	float scale;

	sf::Transform transform;


public:
	Camera();
	~Camera();

	///
	/// Returns the transform of the camera
	///
	const sf::Transform & getTransform() const;

	///
	/// Returns the position of the camera
	///
	sf::Vector2f & getPosition();
	
	///
	/// Returns the scale (zoom) of the camera
	///
	float & getScale();

	///
	/// Returns the scale (zoom) of the camera
	///
	const float & getScale() const;

	void setWindowPosition(sf::Vector2f & _position);

	///
	/// Updates the transform based on changes to position/scale
	///
	void updateTransform();
};

