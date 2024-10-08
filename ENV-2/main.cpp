#include <iostream>
#include <vector>
#include "Complex.h"
#include "Sort.h"

int main() {

std::vector<Complex> numbers;
numbers.emplace_back(1, 2);
numbers.emplace_back(2, 4);
numbers.emplace_back(7, 3);
numbers.emplace_back(8, 5);

Complex a(2, 3);
Complex b(3, 4);

a = a + b;

int n = 15;

b = a * n;

Sort(numbers);

}
