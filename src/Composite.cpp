/*
 * Composite.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: db538
 */

#include "Composite.h"
#include <stdexcept>

Composite::Composite(vector<pIObject> objects)
	: mObjects(objects) {

	if (mObjects.empty())
		throw std::invalid_argument("Composite can't be empty");

	vector<pIObject>::iterator it = mObjects.begin();
	mBoundingBox = (*it)->bounding_box();
	for (; it < mObjects.end(); it++)
		mBoundingBox = mBoundingBox.merge((*it)->bounding_box());
}

Composite::~Composite() {
}

vector<double> Composite::ray_intersections(Ray& ray) {
	vector< vector<double> > sub_intersections(mObjects.size());
	int total = 0;

	// get all sub_intersections and count how many there are
	for(vector<pIObject>::iterator it = mObjects.begin(); it < mObjects.end(); it++) {
		vector<double> it_intersections = (*it)->ray_intersections(ray);
		sub_intersections.push_back(it_intersections);
		total += it_intersections.size();
	}

	// merge them into one big intersection list
	vector<double> result(total);
	for(vector< vector<double> >::iterator it = sub_intersections.begin(); it < sub_intersections.end(); it++) {
		for (vector<double>::iterator it2 = (*it).begin(); it2 < (*it).end(); it2++) {
			result.push_back((*it2));
		}
	}

	return result;
}

BoundingBox& Composite::bounding_box() {
	return mBoundingBox;
}
