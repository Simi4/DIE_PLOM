#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <vector>



using std::cout;
using std::endl;
using std::default_random_engine;
using std::uniform_int_distribution;
using std::uniform_real_distribution;



// класс Vector
class Vector : public std::vector<double> {
private:
	//

public:
	Vector(const int size);
	Vector() = default;

	double sum() const;
	void fill_random(default_random_engine&, const int, const int);
	void fill_random(default_random_engine&, const double, const double);
};



// класс Matrix
class Matrix : public std::vector<Vector> {
public:
	Matrix(const int k, const int n);
	Matrix() = default;

	// Размерность матрицы
	int k() const;
	int n() const;

	// Элемент матрицы по координате
	double get(const int i, const int j) const;

	void matrix_resize(const int k, const int n);

	void fill_random(default_random_engine&, const int, const int);
	void fill_random(default_random_engine&, const double, const double);
};



class Table : public Matrix {
public:
	Vector suppliers; // поставщики
	Vector consumers; // потребители
	Matrix plan; // опорный план

	static Table northwest_corner(const Matrix&, Vector& w, Vector& q);
	Table() = default;

	// Целевая функция
	double SV() const;
};



// Функция вывода Vector
std::ostream& operator<< (std::ostream& os, const Vector& val);

// Функция вывода Matrix
std::ostream& operator<< (std::ostream& os, const Matrix& val);

// Функция вывода матрицы тарифов + запасы и потребности
std::ostream& operator<< (std::ostream& os, const Table& val);
