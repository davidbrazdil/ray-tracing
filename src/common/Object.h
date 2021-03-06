/*
 * Object.h
 *
 *  Created on: 22 Apr 2012
 *      Author: db538
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "Ray.h"
#include "BoundingBox.h"
#include "Color.h"
#include "Material.h"

#include "utils.h"
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

using namespace std;

class Object;
typedef boost::shared_ptr<Object> pObject;

class SurfaceObject;
typedef pair<const SurfaceObject*, double> IntersectionPair;

class Object {
private:

	friend class Composite;
	friend class Light;

	/*
	 * Object passes "this" to the filter function, which should return true/false
	 * whether it should be included in the result. The only difference is Composite,
	 * which aggregates results of its sub-objects.
	 */
	virtual void filter(list<const Object*>& result, bool (*fn)(const Object*)) const;

	/*
	 * Return list of all intersections between the object and a ray.
	 * Each intersection is represented by the scalar multiplier of ray's direction vector, and the ELEMENTARY object it hit.
	 * Should only return positive intersections!
	 */
	virtual void ray_intersections(const Ray &ray, list<IntersectionPair> &result) const = 0;

	/*
	 * Return axis-aligned bounding box of the object
	 */
	virtual const BoundingBox& bounding_box() const = 0;

public:
	Object();
	virtual ~Object();

	/*
	 * Returns first incident SurfaceObject and the point of intersection
	 */
	IntersectionPair getFirstIntersection(const Ray& ray) const;
	class no_intersection_exception : public std::exception { };

	/*
	 * Returns whether line given by two points intersects the object
	 */
	bool isIntersected(const Vector3d& v1, const Vector3d& v2) const;

	/*
	 * Translate
	 */
	virtual pObject translate(const Vector3d& delta) const = 0;

	/*
	 * Scale
	 */
	virtual pObject scale(double factor) const = 0;

	/*
	 * Print out information about the object
	 */
	virtual std::string print_debug(unsigned int indent = 0) const = 0;
};

#endif /* OBJECT_H_ */
