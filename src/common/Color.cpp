/*
 * Color.cpp
 *
 *  Created on: Mar 20, 2012
 *      Author: db538
 */

#include "Color.h"
#include "utils.h"
#include <stdexcept>

Color::Color()
	: mR(0.0f), mG(0.0f), mB(0.0f) {
	init();
}

Color::Color(float r, float g, float b)
	: mR(r), mG(g), mB(b) {
	init();
}

Color::Color(unsigned char r, unsigned char g, unsigned char b)
	: mR((float)r / 255.0f), mG((float)g / 255.0f), mB((float)b / 255.0f) {
	init();
}

void Color::init() {
	if (mR < 0.0) mR = 0.0;
	else if (mR > 1.0) mR = 1.0;

	if (mG < 0.0) mG = 0.0;
	else if (mG > 1.0) mG = 1.0;

	if (mB < 0.0) mB = 0.0;
	else if (mB > 1.0) mB = 1.0;
}

Color::~Color() {
}

Color& Color::operator+=(const Color &rhs) {
	this->mR += rhs.mR;
	this->mG += rhs.mG;
	this->mB += rhs.mB;
	init();
	return *this;
}

Color Color::operator*(const Color &rhs) const {
	return Color(
		this->mR * rhs.mR,
		this->mG * rhs.mG,
		this->mB * rhs.mB);
}

Color Color::operator*(const double &rhs) const {
	return Color(
		(float) (this->mR * rhs),
		(float) (this->mG * rhs),
		(float) (this->mB * rhs));
}

unsigned char Color::getUnsignedCharRed() const {
	return (unsigned char) (mR * 255.0);
}

unsigned char Color::getUnsignedCharGreen() const {
	return (unsigned char) (mG * 255.0);
}

unsigned char Color::getUnsignedCharBlue() const {
	return (unsigned char) (mB * 255.0);
}
