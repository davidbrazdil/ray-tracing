/*
 * Camera.cpp
 *
 *  Created on: Mar 16, 2012
 *      Author: db538
 */

#include "Camera.h"
#include <iostream>

Camera::Camera() {
}

Camera::Camera(const Vector3d& position, const Vector3d& look_at, double field_of_view)
	: mPosition(position), mLookAt(look_at), mFieldOfView(field_of_view) {
	init();
}

Camera::Camera(XMLElement* xml) {
	// Check the element name
	if (!xml || !XMLUtil::StringEqual(xml->Name(), "camera"))
		throw std::invalid_argument("Not a \"camera\" element");

	// Get position
	XMLElement* elem_position = xml->FirstChildElement("position");
	if (!elem_position)
		throw std::invalid_argument("Camera description doesn't contain position");
	XMLElement* elem_position_vector = elem_position->FirstChildElement("vector3");
	if (!elem_position_vector)
		throw std::invalid_argument("Camera description doesn't contain position vector");
	mPosition = parseVector3d(elem_position_vector);

	// Get look-at
	XMLElement* elem_lookat = xml->FirstChildElement("look-at");
	if (!elem_lookat)
		throw std::invalid_argument("Camera description doesn't contain look-at");
	XMLElement* elem_lookat_vector = elem_lookat->FirstChildElement("vector3");
	if (!elem_lookat_vector)
		throw std::invalid_argument("Camera description doesn't contain look-at vector");
	mLookAt = parseVector3d(elem_lookat_vector);

	// Get field-of-view
	XMLElement* elem_fov = xml->FirstChildElement("field-of-view");
	if (!elem_fov)
		throw std::invalid_argument("Camera description doesn't contain field-of-view");
	XMLElement* elem_fov_angle = elem_fov->FirstChildElement("angle");
	if (!elem_fov_angle)
		throw std::invalid_argument("Camera description doesn't contain field-of-view angle");
	mFieldOfView = parseAngle(elem_fov_angle);

	// Initialize camera
	init();
}

void Camera::init() {
	if ((mFieldOfView <= 0.0) || (mFieldOfView >= M_PI))
		throw std::invalid_argument("Viewing angle has to be between 0 and pi");

	mDirection = mLookAt - mPosition;
	mDirection.normalize();

	// The up vector is determined by computing how a z-pos vector must have been rotated
	// to align with the direction vector.

	// (i) Rotate the direction vector into the yz plane

	double direction_x = mDirection.dot(Vector3d::UnitX());
	double direction_z = mDirection.dot(Vector3d::UnitZ());

	double angle_y = 0.0;
	if (isZero(direction_x)) {
		if (isZero(direction_z))
			throw std::invalid_argument("Can't have camera looking straight up or down");
		else if (direction_z > 0.0)
			angle_y = M_PI;
		else
			angle_y = -M_PI;
	} else
		angle_y = atan(direction_x / direction_z);

	Vector3d direction_yz = AngleAxis<double>(-angle_y, Vector3d::UnitY()) * mDirection;

	// (ii) Align the vector in yz plane with z axis

	double direction_yz_x = direction_yz.dot(Vector3d::UnitX());
	double direction_yz_y = direction_yz.dot(Vector3d::UnitY());
	double direction_yz_z = direction_yz.dot(Vector3d::UnitZ());

	if (!isZero(direction_yz_x))
		throw std::runtime_error("Failed to rotate the direction vector into yz plane");
	else if (isZero(direction_yz_z))
		throw std::invalid_argument("Can't have camera looking straight up or down");

	double angle_x = -atan(direction_yz_y / direction_yz_z);

	// (iii) Rotate the y-pos vector by the same amount the direction vector had to rotate from z-pos.

	mUp = AngleAxis<double>(angle_y, Vector3d::UnitY()) *
			(AngleAxis<double>(angle_x, Vector3d::UnitX()) *
				Vector3d::UnitY());
	mUp.normalize();

	// Compute the sideways vector
	mSideways = mUp.cross(mDirection);
	mSideways.normalize();
}

Vector3d Camera::getDirection() const {
    return mDirection;
}

Vector3d Camera::getLookAt() const {
    return mLookAt;
}

Vector3d Camera::getPosition() const {
    return mPosition;
}

Vector3d Camera::getSideways() const {
    return mSideways;
}

Vector3d Camera::getUp() const {
    return mUp;
}

double Camera::getFieldOfView() const {
    return mFieldOfView;
}

Camera::~Camera() {
}

