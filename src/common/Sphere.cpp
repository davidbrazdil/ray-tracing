/*
 * Sphere.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: db538
 */

#include "Sphere.h"
#include <limits>

using namespace tinyxml2;

Sphere::Sphere(Vector3d center, double radius)
	: mCenter(center), mRadius(radius), mNormalOutside(true),
	  mBoundingBox (BoundingBox(
						mCenter.data()[0] - mRadius,
						mCenter.data()[0] + mRadius,
						mCenter.data()[1] - mRadius,
						mCenter.data()[1] + mRadius,
						mCenter.data()[2] - mRadius,
						mCenter.data()[2] + mRadius)) {
	if (mRadius < 0.0)
		throw std::invalid_argument("Sphere radius has to be positive");
}

Sphere::~Sphere() {
}

vector< pair<const IObject*, double> > Sphere::ray_intersections(const Ray& ray) const {
	Vector3d vec_center_to_ray_point = ray.getOrigin() - mCenter;
	double a = ray.getDirection().dot(ray.getDirection());
	double b = vec_center_to_ray_point.dot(2.0 * ray.getDirection());
	double c = vec_center_to_ray_point.dot(vec_center_to_ray_point) - mRadius * mRadius;
	double d = b*b - 4*a*c;

	vector< pair<const IObject*, double> > intersections;
	if (isZero(d)) {
		// one intersection
		intersections.reserve(1);
		intersections.push_back(pair<const IObject*, double>(this, (-b) / (2*a)));
	} else if (d > 0.0) {
		// two intersections
		intersections.reserve(2);
		intersections.push_back(pair<const IObject*, double>(this, (-b + sqrt(d)) / (2*a)));
		intersections.push_back(pair<const IObject*, double>(this, (-b - sqrt(d)) / (2*a)));
	}

	return intersections;
}

const BoundingBox& Sphere::bounding_box() const {
	return mBoundingBox;
}

Vector3d Sphere::normal(const Vector3d& point_on_surface) const {
	Vector3d unit_normal = point_on_surface - mCenter;
	unit_normal.normalize();
	return unit_normal;
}
