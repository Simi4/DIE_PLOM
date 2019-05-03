#include <windows.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <limits>
#include <random>
#include <cmath>
#include <algorithm>
#include <array>
#include <numeric>
#include <cassert>


using namespace std;

// для проверки алгоритма
// #define CHECK_ALG
#define SEED (unsigned)time(nullptr)


// кол-во многоканальных РЛС ( i = 1, ..., K )
const auto K = 3;

// кол-во ГЦ ( j = 1, ..., N )
const auto NUM_GC = 3;
auto N = NUM_GC;


// Тип данных - массив N-элементов
typedef array<double, NUM_GC + 1> array_j;

// Тип данных - массив K-элементов
typedef array<double, K> array_i;

// Тип данных - матрица (K строк, N столбцов)
typedef array<array_j, K> matrix_ij;

// Функция нахождения суммы в массиве
auto SUM(const array_j& X) { return accumulate(begin(X), begin(X) + N, 0.0); }
auto SUM(const array_i& X) { return accumulate(begin(X), begin(X) + K, 0.0); }


// Функция вывода массива array_j
ostream& operator<< (ostream& os, const array_j& val)
{
	for (auto it = begin(val); it != begin(val) + N; it++)
	{
		os << setw(10) << *it << " ";
	}
	return os;
}

// Функция вывода массива array_i
ostream& operator<< (ostream& os, const array_i& val)
{
	for (auto it = begin(val); it != begin(val) + K; it++)
	{
		os << setw(10) << *it << " ";
	}
	return os;
}

// Функция вывода матрицы
ostream& operator<< (ostream& os, const matrix_ij& val)
{
	for (const auto& row : val)
	{
		os << row << endl;
	}
	return os;
}


// важность j-ой ГЦ ( 0 <= Vj <= 100 )
array_j V;


// потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ ( 0 <= Qj <= 1000 )
array_j Q;

void fill_Q()
{
#ifndef CHECK_ALG
	// Генератор случайных чисел
	static default_random_engine gen(SEED);
	static uniform_int_distribution<unsigned> dist(0, 1000);

	for (int j = 0; j < N; j++)
	{
		Q[j] = dist(gen);
	}
#else
	Q = { 300, 200, 100 };
#endif
}


// располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ ( 0 <= Wi <= 5000 )
array_i W;

void fill_W()
{
#ifndef CHECK_ALG
	// Генератор случайных чисел
	static default_random_engine gen(SEED);
	static uniform_int_distribution<unsigned> dist(0, 5000);

	for (int i = 0; i < K; i++)
	{
		W[i] = dist(gen);
	}
#else
	W = { 100, 200, 300 };
#endif
}


// потенциал i-ой РЛС, выделяемый на j-ую ГЦ ( 0 <= wij <= 1000 )
matrix_ij w;

void fill_w()
{
#ifndef CHECK_ALG
	// Генератор случайных чисел
	static default_random_engine gen(SEED);
	static uniform_int_distribution<unsigned> dist(0, 1000);

	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < N; j++)
		{
			w[i][j] = dist(gen);
		}
	}
#else
	w[0] = { 100, 200, 300 };
	w[1] = { 200, 300, 400 };
	w[2] = { 300, 400, 500 };
#endif
}


// радиолокационный потенциал, выделенный всеми РЛС на j-ую ГЦ
array_j q;


// обеспечиваемая при обслуживании j-ой ГЦ важность
array_j v;

void fill_V()
{
#ifndef CHECK_ALG
	// Генератор случайных чисел
	static default_random_engine gen(SEED);
	static uniform_int_distribution<unsigned> dist(0, 100);

	for (int j = 0; j < N; j++)
	{
		V[j] = dist(gen);
	}
#else
	V = { 20, 30, 40 };
#endif
}

// важность, обеспечиваемая при обслуживании всех ГЦ
double SV;

// вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ ( 0 <= pij <= 1 )
matrix_ij p;

void fill_p()
{
#ifndef CHECK_ALG
	// Генератор случайных чисел
	default_random_engine gen(SEED);
	uniform_real_distribution<double> dist(0.0, 1.0);

	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < N; j++)
		{
			p[i][j] = dist(gen);
		}
	}
#else
	p[0] = { 0.5, 0.2, 0.3 };
	p[1] = { 0.1, 0.8, 0.4 };
	p[2] = { 0.1, 0.3, 0.7 };
#endif
}

// важность, не обеспечиваемая при обслуживании всех ГЦ
double sV;


// коэффициент для ЦФ ( Сij = Vj / Qj * pij )
matrix_ij C;


// Опорный план
matrix_ij plan;

// Функция заполнения опорного плана "NaN"
void fill_plan()
{
	for (auto& row : plan)
	{
		fill(begin(row), end(row), numeric_limits<double>::quiet_NaN());
	}
}

// Функция вывода матрицы опорного плана
void print_plan()
{
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (isnan(plan[i][j]))
			{
				cout << setw(8) << C[i][j] << setw(13) << " ";
			}
			else
			{
				cout << setw(6) << C[i][j] << "[" << setw(8) << plan[i][j] << setw(6) << "]";
			}
		}
		cout << endl;
	}
}


int main()
{
	srand(SEED);

	// Кодировка консоли
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	// Задаем точность до сотых
	cout << fixed << left;
	cout.precision(3);

	cout << "Количество ГЦ  (N): " << N << endl;
	cout << "Количество РЛС (K): " << K << endl << endl;

	// располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ (0<=Wi<=5000)
	fill_W();
	cout << "W[K]: " << W << endl;


	double sumW = SUM(W);
	cout << "Располагаемая сумма: " << sumW << endl << endl;


	// потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ (0<=Qj<=1000)
	fill_Q();
	cout << "Q[N]: " << Q << endl;

	double sumQ = SUM(Q);
	cout << "Потребная сумма: " << sumQ << endl << endl;


	if (sumW < sumQ)
	{
		cout << "Проверьте баланс! Располагаемая сумма должна быть больше потребной!" << endl;
	}
	else if (sumW > sumQ)
	{
		cout << "sumW > sumQ ( " << sumW << " > " << sumQ << " ), значит вводим фиктивную переменную Q[" << N + 1 << "]" << endl << endl;
	}

	// важность j-ой ГЦ ( 0 <= Vj <= 100 )
	fill_V();
	// cout << "V[N]: " << V << endl;


	// вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ ( 0 <= pij <= 1 )
	fill_p();
	// cout << "p[K][N]:" << endl << p << endl;


	// коэффициент для ЦФ ( Сij = Vj/Qj*pij )
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < N; j++)
		{
			C[i][j] = (V[j] / Q[j]) * p[i][j];
		}
	}



	//---------------------------------------------------------------------------------------! ! ! ----
	//	if (sumW > sumQ)
	// добавления столбца из нулей (в конец), в массив Q[N] добавить Q[N+1]=sumW-sumQ
	//---------------------------------------------------------------------------------------! ! ! ----

	if (sumW > sumQ)
	{
		N = N + 1;
		Q[N - 1] = sumW - sumQ;
	}


	cout << "C[K][N]:" << endl << C << endl;
	cout << "Q[N]: " << Q << endl << endl;

	////радиолокационный потенциал, выделенный всеми РЛС на j-ую ГЦ
	//cout << ". q[N]: ";
	//for (int i = 0; i < K; i++)
	//{
	//	for (int j = 0; j < N; j++)
	//	{
	//		q[j] += w[i][j];
	//	}
	//}
	//for (int j = 0; j < N; j++)
	//{
	//	cout << q[j] << "\t";
	//}
	//cout << endl;

	// потенциал i-ой РЛС, выделяемый на j-ую ГЦ (0<=wij<=1000)
	fill_w();
	// cout << "w[K][N]:" << endl << w;

	// обеспечиваемая при обслуживании j-ой ГЦ важность
	for (int j = 0; j < N; j++)
	{
		v[j] = V[j] * (q[j] / Q[j]);
	}

	double sumV = SUM(V);
	// важность, не обеспечиваемая при обслуживании всех ГЦ
	for (int j = 0; j < N; j++)
	{
		sV = sumV - SV;
	}


	//СЕВЕРО-ЗАПАДНЫЙ УГОЛ---------------------------------


	// Опорный план - заполнение "NaN"
	fill_plan();


	double min_val;
	int kol_X = 0;
	for (int i = 0; i < K; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (Q[j] == 0)
				continue;

			min_val = min(W[i], Q[j]);
			SV += C[i][j] * min_val;
			plan[i][j] = min_val;
			kol_X = kol_X + 1;
			W[i] -= min_val;
			Q[j] -= min_val;
			
			if (W[i] == 0)
				break;
		}
	}

	cout << "Опорный план" << endl; print_plan();
	cout << "Целевая функция SV = " << SV << endl << endl;

#ifdef CHECK_ALG
	if (SV < 41.65 || SV > 41.67)
	{
		assert(!"CHECK_ALG failed!");
	}
#endif

	cout << "Число базисных клеток = " << kol_X;

	if (kol_X < (N + K - 1))
	{
		cout << ", что меньше N + K - 1, значит план вырожденный и ";
		cout << "его необходимо пополнить, проставив в недостающем числе клеток нулевую перевозку." << endl;
	}
	else
	{
		if ((kol_X = (N + K - 1)))
			cout << ", что равно N + K - 1, значит план невырожденный.";
		else
			cout << ", что больше N + K - 1, значит план невырожденный.";
	}
	cout << endl << endl;


	//УЛУЧШЕНИЕ ОПОРНОГО ПЛАНА МЕТОДОМ ПОТЕНЦИАЛОВ---------------------------------


	cin.get();
	return 0;
}