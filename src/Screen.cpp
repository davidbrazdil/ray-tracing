/*
 * ScreenRect.cpp
 *
 *  Created on: Mar 16, 2012
 *      Author: db538
 */

#include "Screen.h"
#include <iostream>

Screen::Screen(pCamera camera, double distance, Ratio aspect_ratio, Color background_color)
	: mCamera(camera), mDistance(distance), mAspectRatio(aspect_ratio), mBackgroundColor(background_color) {
	init();
}

void Screen::init() {
	if (mDistance <= 0.0)
		throw std::invalid_argument("Distance has to be positive");
	if (mAspectRatio.getNumerator() == 0)
		throw std::invalid_argument("Can't have zero ratio");

	double field_of_view = mCamera->getFieldOfView() * 0.5;
	Vector3d direction = mCamera->getDirection();
	Vector3d screen_center = mDistance * direction;
	Vector3d left_edge = (mDistance / cos(field_of_view)) * (AngleAxis<double>(-field_of_view, mCamera->getUp()) * direction);
	Vector3d sideways = left_edge - screen_center;
	Vector3d upwards = mCamera->getUp() * sideways.norm() / mAspectRatio.getDouble();

	mTopLeftCorner = left_edge + upwards;
	mHorizontal = -2.0 * sideways;
	mVertical = -2.0 * upwards;
}

Vector3d Screen::getTopLeftCorner() const {
    return mTopLeftCorner;
}

Vector3d Screen::getHorizontal() const {
    return mHorizontal;
}

Ratio Screen::getAspectRatio() const {
    return mAspectRatio;
}

double Screen::getDistance() const {
    return mDistance;
}

Vector3d Screen::getVertical() const {
    return mVertical;
}

pCamera Screen::getCamera() const {
	return mCamera;
}

Screen::~Screen() {
}

