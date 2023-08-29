#pragma once
#ifndef CURVES_CLASSES_H
#define CURVES_CLASSES_H

#include <cmath>
#include <numbers>
#include <vector>
#include <memory>

class Point3D {
public:
	double x, y, z;

	Point3D(double _x, double _y, double _z);
};

class Vector3D {
public:
	double x, y, z;

	Vector3D(double _x, double _y, double _z);
};

class Curve {
public:
	virtual double getRadius() const = 0;
	virtual Point3D getPoint(double t) const = 0;
	virtual Vector3D getDerivative(double t) const = 0;
	virtual ~Curve() {}
};

class Circle : public Curve {
private:
	double radius;

public:
	Circle(double r);

	double getRadius() const override;

	Point3D getPoint(double t) const override;

	Vector3D getDerivative(double t) const override;
};

class Ellipse : public Curve {
private:
	double radiusX;
	double radiusY;

public:
	Ellipse(double rx, double ry);

	double getRadius() const override;

	Point3D getPoint(double t) const override;

	Vector3D getDerivative(double t) const override;
};

class Helix : public Curve {
private:
	double radius;
	double step;

public:
	Helix(double r, double s);

	double getRadius() const override;

	Point3D getPoint(double t) const override;

	Vector3D getDerivative(double t) const override;
};

#endif // CURVES_CLASSES_H
