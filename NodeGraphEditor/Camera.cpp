#include "Camera.h"



void Camera::updateTransform()
{
	transform = sf::Transform::Identity;
	transform.translate(windowPosition);
	transform.scale(scale, scale);
	transform.translate(position);
}

Camera::Camera() : position(0, 0), scale(1), windowPosition(0, 0)
{
	updateTransform();
}


Camera::~Camera()
{
}

const sf::Transform & Camera::getTransform() const
{
	return transform;
}

sf::Vector2f & Camera::getPosition()
{
	return position;
}

float & Camera::getScale()
{
	return scale;
}

//void Camera::setPosition(sf::Vector2f & _position)
//{
//	position = _position;
//}
//
//void Camera::setScale(float & _scale)
//{
//	scale = _scale;
//}

void Camera::setWindowPosition(sf::Vector2f & _position)
{
	windowPosition = _position;
}
