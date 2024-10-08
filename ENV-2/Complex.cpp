#include "Complex.h"
#include <cmath>

Complex::Complex(double real = 0, double i = 0) : a(real), b(i) {}

Complex Complex::operator+(const Complex& other) {
	return Complex(a + other.a, b + other.b);
}

Complex Complex::operator-(const Complex& other) {
        return Complex(a - other.a, b - other.b);
}

Complex Complex::operator*(const double value) {
	return Complex(a * value, b * value)
}

double Complex::abs() {
	return std::sqrt(real * real + i * i);
}



