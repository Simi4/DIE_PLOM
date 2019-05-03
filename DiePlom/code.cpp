#ifdef _WIN32
#include <windows.h>
#endif
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>
#include <random>


// кол-во многоканальных РЛС ( i = 1, ..., K )
const auto K = 3;

// кол-во ГЦ ( j = 1, ..., N )
const auto NUM_GC = 3;
auto N = NUM_GC;


using namespace std;

// Зерно генератора случайных чисел
#define SEED (unsigned)time(nullptr)

// Задаем точность вывода до сотых
const auto PRECISION = 3;


// Генератор случайных чисел
default_random_engine gen(SEED);


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
		os << setw(8 + PRECISION) << *it;
	}
	return os;
}

// Функция вывода массива array_i
ostream& operator<< (ostream& os, const array_i& val)
{
	for (auto it = begin(val); it != begin(val) + K; it++)
	{
		os << setw(8 + PRECISION) << *it;
	}
	return os;
}

// Функция вывода матрицы
ostream& operator<< (ostream& os, const matrix_ij& val)
{
	for (const array_j& row : val)
	{
		os << row << endl;
	}
	return os;
}


// важность j-ой ГЦ ( 0 <= Vj <= 100 )
array_j V;

// Функция заполнения массива Vj целыми случайными числам 0 .. 100
void fill_V()
{
	uniform_int_distribution<unsigned> dist(0, 100);
	generate(begin(V), begin(V) + N, bind(dist, gen));
}


// потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ ( 0 <= Qj <= 1000 )
array_j Q;

// Функция заполнения массива Qj целыми случайными числам 0 .. 1000
void fill_Q()
{
	uniform_int_distribution<unsigned> dist(0, 1000);
	generate(begin(Q), begin(Q) + N, bind(dist, gen));
}


// располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ ( 0 <= Wi <= 5000 )
array_i W;

// Функция заполнения массива Wi целыми случайными числам 0 .. 5000
void fill_W()
{
	uniform_int_distribution<unsigned> dist(0, 5000);
	generate(begin(W), end(W), bind(dist, gen));
}


// потенциал i-ой РЛС, выделяемый на j-ую ГЦ ( 0 <= wij <= 1000 )
matrix_ij w;

// Функция заполнения матрицы wij целыми случайными числам 0 .. 1000
void fill_w()
{
	uniform_int_distribution<unsigned> dist(0, 1000);
	for (array_j& row : w)
	{
		for (auto& val : row)
		{
			val = dist(gen);
		}
	}
}


// радиолокационный потенциал, выделенный всеми РЛС на j-ую ГЦ
array_j q;


// обеспечиваемая при обслуживании j-ой ГЦ важность
array_j v;

// важность, обеспечиваемая при обслуживании всех ГЦ
double SV;

// вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ ( 0 <= pij <= 1 )
matrix_ij p;

// Функция заполнения матрицы pij случайными числам 0.0 .. 1.0
void fill_p()
{
	uniform_real_distribution<double> dist(0.0, 1.0);
	for (array_j& row : p)
	{
		for (auto& val : row)
		{
			val = dist(gen);
		}
	}
}

// важность, не обеспечиваемая при обслуживании всех ГЦ
double sV;


// коэффициент для ЦФ ( Сij = Vj / Qj * pij )
matrix_ij C;


// Опорный план
matrix_ij plan;

// Функция заполнения матрицы опорного плана значением "NaN"
void fill_plan()
{
	for (array_j& row : plan)
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
				cout << setw(12 + PRECISION) << C[i][j];
			}
			else
			{
				cout << setw(3 + PRECISION) << C[i][j] << "[" << setw(4) << setprecision(0) << plan[i][j] << setprecision(PRECISION) << setw(4) << "]";
			}
		}
		cout << endl;
	}
}


// Функция вывода матрицы тарифов + запасы и потребности
void print_full()
{
	streamsize col_width = PRECISION + 8;
	cout << setw((streamsize)6 + (streamsize)N * col_width) << "C[K][N]" << "W[K]" << endl;

	for (int i = 0; i < K; i++)
	{
		cout << setw(6) << "";
		for (int j = 0; j < N; j++)
		{
			cout << setw(col_width) << C[i][j];
		}
		cout << setprecision(0) << W[i] << setprecision(PRECISION) << endl;
	}
	cout << setw(6) << "Q[N]" << setprecision(0) << Q << setprecision(PRECISION) << endl << endl;
}


int main()
{
	srand(SEED);

#ifdef _WIN32
	// Кодировка консоли
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif

	cout << fixed << left << setprecision(PRECISION);

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


	// вывод матрицы тарифов + запасы и потребности
	print_full();


	// радиолокационный потенциал, выделенный всеми РЛС на j-ую ГЦ
	//for (int i = 0; i < K; i++)
	//{
	//	for (int j = 0; j < N; j++)
	//	{
	//		q[j] += w[i][j];
	//	}
	//}
	//cout << "q[N]: " << q << endl;

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

	array_i p_U;
	array_j p_V;

	fill(begin(p_U), end(p_U), 0.0);
	fill(begin(p_V), end(p_V), 0.0);

	for (int i = 0; i < K; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (!isnan(plan[i][j]))
			{
				p_V[j] = C[i][j] - p_U[i];
				p_U[i] = C[i][j] - p_V[j];
			}
		}
	}

	cout << "p_U[K]: " << p_U << endl;
	cout << "p_V[N]: " << p_V << endl;

	cin.get();
	return 0;
}