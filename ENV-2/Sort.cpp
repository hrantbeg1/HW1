#include "Sort.h"
#include <algorithm>

bool compare(const Complex& left, const Complex& right) {
	return left.abs() < right.abs();
}

void Sort(std::vector<Complex>& numbers) {
	std::sort(numbers.begin(), numbers.end(), compare);
}
