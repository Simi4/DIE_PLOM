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


struct PlanItem {
	double orig;
	double value = 0.0;
	bool is_plan = false;
	int mark = 0; // -1, 0, +1
};


class PlanMatrix : public std::vector<std::vector<PlanItem>> {
public:
	PlanMatrix(const int k, const int n);
	PlanMatrix() = default;

	void reset_marks();

	// Размерность матрицы
	int k = 0, n = 0;
};


class Table : public Matrix {
public:
	Vector suppliers; // поставщики
	Vector consumers; // потребители
	PlanMatrix plan; // опорный план

	Table() = default;

	// Целевая функция
	double SV() const;
};


// Метод северо-западного угла
Table northwest_corner_method(const Matrix&, Vector& w, Vector& q);


// Метод потенциалов
class PotentialsMethod {
private:
	// вспомогательный stuff для метода потенциалов
	struct Pos {
		int i, j;
		Pos(int i, int j) : i(i), j(j) {}
		bool operator== (Pos& other) { return i == other.i && j == other.j; }
	};

	// Ага, смартпоинтеры
	typedef std::shared_ptr<Pos> PosPtr;
	typedef std::vector<PosPtr> Cycle;
	typedef std::vector<Cycle> Cycles;

public:
	Matrix differences;
	Table table;
	Vector u;
	Vector v;

	PotentialsMethod() = default;
	PotentialsMethod(Table table);

	void calc_potentials();
	void calc_differences();
	bool is_optimal();
	void optimize();

	// Нахождение цикла многоугольника
	Cycle build_cycle(PosPtr top);
	bool check_cycle_final(const Cycle& cycle) const;
	void make_cycle_branches(Cycles& cycles, Cycle& cycle);
};



// Функция вывода Vector
std::ostream& operator<< (std::ostream& os, const Vector& val);

// Функция вывода Matrix
std::ostream& operator<< (std::ostream& os, const Matrix& val);

// Функция вывода матрицы тарифов + запасы и потребности
std::ostream& operator<< (std::ostream& os, const Table& val);

// Функция вывода матрицы опорного плана
std::ostream& operator<< (std::ostream& os, const PlanMatrix& val);
