#include <windows.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <limits>
#include <cstdio>
#include <ctime>
#include <random>
#include <cmath>
#include <algorithm>
#include <array>
#include <numeric>
#include <string>


using namespace std;



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

// Макрос нахождения суммы в массиве
auto SUM(array_j X) { return accumulate(begin(X), begin(X) + N, 0.0); }
auto SUM(array_i X) { return accumulate(begin(X), begin(X) + K, 0.0); }

// Макрос вывода массива
template<typename T>
void PRINT_ARRAY_EX(string str, T X, int cnt) {
	cout << str;
	for (auto it = begin(X); it != begin(X) + cnt; it++)
		cout << *it << " ";
	cout << endl;
}
void PRINT_ARRAY(string str, array_j X) { PRINT_ARRAY_EX(str, X, N); }
void PRINT_ARRAY(string str, array_i X) { PRINT_ARRAY_EX(str, X, K); }


// Макрос вывода матрицы
void PRINT_MATRIX(string str, matrix_ij X) {
	cout << str << endl;
	for (auto row : X) {
		for (auto it = begin(row); it != begin(row) + N; it++)
			cout << *it << "\t";
		cout << endl;
	} cout << endl;
}


// важность j-ой ГЦ ( 0 <= Vj <= 100 )
array_j V;


// потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ ( 0 <= Qj <= 1000 )
array_j Q;


// располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ ( 0 <= Wi <= 5000 )
array_i W;


// потенциал i-ой РЛС, выделяемый на j-ую ГЦ ( 0 <= wij <= 1000 )
matrix_ij w;


// радиолокационный потенциал, выделенный всеми РЛС на j-ую ГЦ
array_j q;


// обеспечиваемая при обслуживании j-ой ГЦ важность
array_j v;


// важность, обеспечиваемая при обслуживании всех ГЦ
double SV;

// вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ ( 0 <= pij <= 1 )
matrix_ij p;


// важность, не обеспечиваемая при обслуживании всех ГЦ
double sV;


// коэффициент для ЦФ ( Сij = Vj / Qj * pij )
matrix_ij C;


int main()
{
	// Генератор случайных чисел
	default_random_engine generator((unsigned)time(nullptr));
	srand((unsigned)time(nullptr));

	// Кодировка консоли
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	// Задаем точность до сотых
	cout << fixed;
	cout.precision(3);

	cout << "Количество ГЦ  (N): " << N << endl;
	cout << "Количество РЛС (K): " << K << endl << endl;

	// располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ (0<=Wi<=5000)

	for (int i = 0; i < K; i++)
	{
		W[i] = rand() % 5000;
	}

	// вывод W
	PRINT_ARRAY("W[K]: ", W);

	double sumW = SUM(W);
	cout << "Располагаемая сумма: " << sumW << endl << endl;


	//потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ (0<=Qj<=1000)

	for (int j = 0; j < N; j++)
	{
		Q[j] = rand() % 1000;
	}

	// вывод Q
	PRINT_ARRAY("Q[N]: ", Q);

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

	//важность j-ой ГЦ (0<=Vj<=100)
	for (int j = 0; j < N; j++)
	{
		V[j] = rand() % 100;
	}

	// PRINT_ARRAY( "V[N]: ", V );


	//вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ (0<=pij<=1)
	uniform_real_distribution<double>distribution(0.0, 1.0);

	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < N; j++)
		{
			p[i][j] = distribution(generator);
		}
	}
	//PRINT_MATRIX( "p[K][N]:", p );


	//коэффициент для ЦФ (Сij= Vj/Qj*pij)
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

	if (sumW > sumQ) {
		N = N + 1;
		Q[N - 1] = sumW - sumQ;
	}


	PRINT_MATRIX("C[K][N]:", C);
	PRINT_ARRAY("", Q);

	cout << endl;

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

	//потенциал i-ой РЛС, выделяемый на j-ую ГЦ (0<=wij<=1000)

	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < N; j++)
		{
			w[i][j] = rand() % 1000;
		}
	}
	// PRINT_MATRIX("w[K][N]:", w);

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
			C[i][j] = min_val;
			kol_X = kol_X + 1;
			W[i] -= min_val;
			Q[j] -= min_val;
			
			if (W[i] == 0)
				break;

		}
	}

	PRINT_MATRIX("Опорный план", C);
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
	cout << "УЛУЧШЕНИЕ ОПОРНОГО ПЛАНА. ИТЕРАЦИЯ ПЕРВАЯ" << endl << endl;









	system("pause");
	return 0;
}

