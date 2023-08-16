#pragma once

#ifndef CURVES_CLASSES_H
#define CURVES_CLASSES_H

#ifdef CURVES_DLL_EXPORTS
#define CURVES_API __declspec(dllexport)
#else
#define CURVES_API __declspec(dllimport)
#endif

// Link against the import library "CurveClasses.lib"
#pragma comment(lib, "CurveClasses.lib")

#include <cmath>
#include <numbers>
#include <vector>
#include <memory>

class CURVES_API Point3D {
public:
	double x, y, z;

	Point3D(double _x, double _y, double _z);
};

class CURVES_API Vector3D {
public:
	double x, y, z;

	Vector3D(double _x, double _y, double _z);
};

class CURVES_API Curve {
public:
	virtual double getRadius() const = 0;
	virtual Point3D getPoint(double t) const = 0;
	virtual Vector3D getDerivative(double t) const = 0;
	virtual ~Curve() {}
};

class CURVES_API Circle : public Curve {
private:
	double radius;

public:
	Circle(double r);

	double getRadius() const override;

	Point3D getPoint(double t) const override;

	Vector3D getDerivative(double t) const override;
};

class CURVES_API Ellipse : public Curve {
private:
	double radiusX;
	double radiusY;

public:
	Ellipse(double rx, double ry);

	double getRadius() const override;

	Point3D getPoint(double t) const override;

	Vector3D getDerivative(double t) const override;
};

class CURVES_API Helix : public Curve {
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
