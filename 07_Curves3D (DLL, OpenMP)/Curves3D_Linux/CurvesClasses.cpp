#include "CurvesClasses.h"

Point3D::Point3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

Vector3D::Vector3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

Circle::Circle(double r) : radius(r) {}

double Circle::getRadius() const {
	return radius;
}

Point3D Circle::getPoint(double t) const {
	double x = radius * cos(t);
	double y = radius * sin(t);
	double z = 0.0;
	return Point3D(x, y, z);
}

Vector3D Circle::getDerivative(double t) const {
	double dx_dt = -radius * sin(t);
	double dy_dt = radius * cos(t);
	double dz_dt = 0.0;
	return Vector3D(dx_dt, dy_dt, dz_dt);
}

Ellipse::Ellipse(double rx, double ry) : radiusX(rx), radiusY(ry) {}

double Ellipse::getRadius() const {
	return radiusX;
}

Point3D Ellipse::getPoint(double t) const {
	double x = radiusX * cos(t);
	double y = radiusY * sin(t);
	double z = 0.0;
	return Point3D(x, y, z);
}

Vector3D Ellipse::getDerivative(double t) const {
	double dx_dt = -radiusX * sin(t);
	double dy_dt = radiusY * cos(t);
	double dz_dt = 0.0;
	return Vector3D(dx_dt, dy_dt, dz_dt);
}

Helix::Helix(double r, double s) : radius(r), step(s) {}

double Helix::getRadius() const {
	return radius;
}

Point3D Helix::getPoint(double t) const {
	double x = radius * cos(t);
	double y = radius * sin(t);
	double z = step * (t / (2 * std::numbers::pi));
	return Point3D(x, y, z);
}

Vector3D Helix::getDerivative(double t) const {
	double dx_dt = -radius * sin(t);
	double dy_dt = radius * cos(t);
	double dz_dt = step / (2 * std::numbers::pi);
	return Vector3D(dx_dt, dy_dt, dz_dt);
}