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

	const sf::Transform & getTransform() const;

	sf::Vector2f & getPosition();
	float & getScale();

	//void setPosition(sf::Vector2f & _position);
	//void setScale(float & _scale);
	void setWindowPosition(sf::Vector2f & _position);

	void updateTransform();
};

