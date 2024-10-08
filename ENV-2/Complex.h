
#pragma once

class Complex {
private:
	double a;
	double b;

public:
	Complex(double real = 0, double i = 0);
	Complex& operator+(const Complex& other);
	Complex& operator-(const Complex& other);
	Complex& operator*(double value);

	double abs();
};
