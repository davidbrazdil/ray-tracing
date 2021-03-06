/*
 * XML.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: db538
 */

#include "XML.h"
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace boost;

std::string XML::getProperty(XMLElement* elem, std::string property, std::string type) {
	XMLElement* subelem = elem->FirstChildElement(property.c_str());
	if (!subelem)
		throw std::invalid_argument(str(format("XML parse expected property \"%s\"") % property));
	return getParameter(subelem, type);
}

std::string XML::getParameter(XMLElement *elem, std::string type) {
	XMLAttribute* subelem_type = elem->FindAttribute(type.c_str());
	if (!subelem_type)
		throw std::invalid_argument(str(format("XML parse expected type \"%s\"") % type));
	return std::string(subelem_type->Value());
}

pMaterial XML::getMaterial(std::string name, list<pMaterial> materials) {
	BOOST_FOREACH(pMaterial mat, materials)
		if (mat->getName() == name)
			return mat;
	throw std::invalid_argument(str(format("Material \"%s\" not found") % name));
}

double XML::parseDouble(XMLElement* xml, std::string property) {
	return parseDouble(getProperty(xml, property, "double"));
}

double XML::parseDouble(XMLElement* xml) {
	return parseDouble(getParameter(xml, "double"));
}

double XML::parseDouble(std::string value) {
	std::istringstream stream(value);

	double val;
	stream >> val;

	return val;
}

double XML::parseAngle(XMLElement* xml, std::string property) {
	return parseAngle(getProperty(xml, property, "angle"));
}

double XML::parseAngle(XMLElement* xml) {
	return parseAngle(getParameter(xml, "angle"));
}

double XML::parseAngle(std::string value) {
	std::istringstream stream(value);

	double val;
	std::string unit;
	stream >> val >> unit;

	if (unit == "deg")
		val *= M_PI / 180.0;
	else if (unit != "rad")
		throw std::invalid_argument("Angle property \"%s\" must have units either \"deg\" or \"rad\"");

	return val;
}

Vector3d XML::parseVector3d(XMLElement* xml, std::string property) {
	return parseVector3d(getProperty(xml, property, "vector3"));
}

Vector3d XML::parseVector3d(XMLElement* xml) {
	return parseVector3d(getParameter(xml, "vector3"));
}

Vector3d XML::parseVector3d(std::string value) {
	std::istringstream stream(value);

	double x, y, z;
	stream >> x >> y >> z;

	Vector3d res;
	res << 	x, y, z;

	return res;
}

Ratio XML::parseRatio(XMLElement* xml, std::string property) {
	return parseRatio(getProperty(xml, property, "ratio"));
}

Ratio XML::parseRatio(XMLElement* xml) {
	return parseRatio(getParameter(xml, "ratio"));
}

Ratio XML::parseRatio(std::string value) {
	std::istringstream stream(value);

	unsigned int numerator, denominator;
	stream >> numerator >> denominator;

	return	Ratio(numerator, denominator);
}

unsigned char XML::getHex(char hex) {
	switch (hex) {
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	case 'A': return 10;
	case 'B': return 11;
	case 'C': return 12;
	case 'D': return 13;
	case 'E': return 14;
	case 'F': return 15;
	default: throw std::invalid_argument("Color property \"%s\" must be in 6-digit HEX");
	}
}

Color XML::parseColor(XMLElement* xml, std::string property) {
	return parseColor(getProperty(xml, property, "color"));
}

Color XML::parseColor(XMLElement* xml) {
	return parseColor(getParameter(xml, "color"));
}

Color XML::parseColor(std::string value) {
	if (value.length() != 6)
		throw std::invalid_argument("Property \"%s\" must be in 6-digit HEX");

	unsigned char r, g, b;
	r = (getHex(value[0]) << 4) + getHex(value[1]);
	g = (getHex(value[2]) << 4) + getHex(value[3]);
	b = (getHex(value[4]) << 4) + getHex(value[5]);

	return Color(r, g, b);
}

pCamera XML::parseCamera(XMLElement* xml_root) {
	XMLElement* xml_elem_camera = xml_root->FirstChildElement("camera");
	if (!xml_elem_camera)
		throw std::runtime_error("Scene description doesn't contain camera information");

	return pCamera(
				new Camera(
					parseVector3d(xml_elem_camera, "position"),
					parseVector3d(xml_elem_camera, "look-at"),
					parseAngle(xml_elem_camera, "field-of-view")));
}

pScreen XML::parseScreen(XMLElement* xml_root, pCamera camera) {
	XMLElement* xml_elem_screen = xml_root->FirstChildElement("screen");
	if (!xml_elem_screen)
		throw std::runtime_error("Scene description doesn't contain screen information");

	return pScreen(
				new Screen(
					camera,
					parseDouble(xml_elem_screen, "distance"),
					parseRatio(xml_elem_screen, "aspect-ratio"),
					parseColor(xml_elem_screen, "background")));
}

list<pMaterial> XML::parseMaterials(XMLElement* xml_root) {
	XMLElement* xml_elem_materials = xml_root->FirstChildElement("materials");
	if (!xml_elem_materials)
		throw std::runtime_error("Scene description doesn't contain materials information");

	list<pMaterial> materials;

	XMLElement* xml_child = xml_elem_materials->FirstChildElement();
	while (xml_child) {
		std::string mat_type(xml_child->Name());

		if (mat_type == "solid")
			materials.push_back(parseSolidMaterial(xml_child));
		else
			throw std::invalid_argument(boost::str(boost::format("Invalid material type \"%s\"") % mat_type));

		xml_child = xml_child->NextSiblingElement();
	}

	return materials;
}

pMaterial XML::parseSolidMaterial(XMLElement* xml) {
	return pMaterial(new SolidMaterial(
			getParameter(xml, "id"),
			parseColor(xml, "diffuse"),
			parseColor(xml, "specular"),
			parseColor(xml, "transmit"),
			parseDouble(xml, "refractive-index")));
}

pObject XML::parseObjects(XMLElement* xml_root, list<pMaterial> materials) {
	XMLElement* xml_elem_objects = xml_root->FirstChildElement("objects");
	if (!xml_elem_objects)
		throw std::runtime_error("Scene description doesn't contain objects information");

	return parseObjectOrOperation(xml_elem_objects->FirstChildElement(), materials);
}

pObject XML::parseObjectOrOperation(XMLElement* xml_elem, list<pMaterial> materials) {
	if (!xml_elem)
		throw std::invalid_argument("Expected object/operation");

	vector<pObject> objects;

	while (xml_elem) {
		std::string name(xml_elem->Name());

		if (name == "sphere")
			objects.push_back(parseObject_Sphere(xml_elem, materials));
		else if (name == "point-light")
			objects.push_back(parseObject_PointLight(xml_elem));
		else if (name == "translate")
			objects.push_back(parseOperation_Translate(xml_elem, materials));
		else if (name == "scale")
			objects.push_back(parseOperation_Scale(xml_elem, materials));
		else
			throw std::invalid_argument(boost::str(boost::format("Invalid object/operation type \"%s\"") % name));

		xml_elem = xml_elem->NextSiblingElement();
	}

	if (objects.empty())
		throw std::invalid_argument("Expected at least one sub-object/operation");
	else if (objects.size() == 1)
		return objects[0];
	else
		return pObject(new Composite(objects));

}

pObject XML::parseObject_Sphere(XMLElement* xml_elem, list<pMaterial> materials) {
	return pObject(new Sphere(
		getMaterial(
			getParameter(xml_elem, "material"),
			materials)));
}

pObject XML::parseObject_PointLight(XMLElement* xml_elem) {
	return pObject(new PointLight(
		parseColor(xml_elem, "intensity")));
}

pObject XML::parseOperation_Translate(XMLElement *xml_elem, list<pMaterial> materials) {
	Vector3d delta(parseVector3d(xml_elem));
	pObject obj = parseObjectOrOperation(xml_elem->FirstChildElement(), materials);
	return obj->translate(delta);
}

pObject XML::parseOperation_Scale(XMLElement *xml_elem, list<pMaterial> materials) {
	double factor(parseDouble(xml_elem));
	pObject obj = parseObjectOrOperation(xml_elem->FirstChildElement(), materials);
	return obj->scale(factor);
}
