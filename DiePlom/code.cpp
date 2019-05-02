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


using namespace std;


// кол-во многоканальных РЛС ( i = 1, ..., K )
const int K = 3;

// кол-во ГЦ ( j = 1, ..., N )
const int N = 3;


// Тип данных - массив N-элементов
typedef array<double, N> array_j;

// Тип данных - массив K-элементов
typedef array<double, K> array_i;

// Тип данных - матрица (K строк, N столбцов)
typedef array<array_j, K> matrix_ij;

// Макрос нахождения суммы в массиве
#define SUM(X) accumulate(begin(X), end(X), 0.0)

// Макрос вывода массива
#define PRINT_ARRAY(str, X) { cout << str; for (auto it : X) cout << it << "  "; cout << endl; }

// Макрос вывода матрицы
#define PRINT_MATRIX(str, X) { \
	cout << str << endl; \
	for (auto row : X) { \
		for (auto it : row) \
			cout << it << "\t"; \
		cout << endl; \
	} cout << endl; }


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

	cout << "Количество ГЦ:  " << N << endl;
	cout << "Количество РЛС: " << K << endl << endl;

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
//---------------------------------------------------------------------------------------! ! ! ----
// встатить действие добавления столбца из нулей (в конец), в массив Q[N] добавить Q[N+1]=sumW-sumQ
//---------------------------------------------------------------------------------------! ! ! ----
	if (sumW > sumQ)
	{
		int Kk = K;
		int Nn = N;
		// Выполняем цикл для каждой строки i e [0..row-1] матрицы matrix 
		for (int i = 0; i < Kk; i++)
		{
			// Выделяем временный буфер temp под текущую строку i размером cols + 1 элементов
			// Для вставки столбца необходимо чтобы каждая строка i имела размер cols + 1 на 
			// один элемент больше исходной строки matrix[i]
			int* temp = new int[Nn + 1];
			// Копируем каждый элемент j строки i матрицы во временный буфер temp
			for (int j = 0; j < Nn; j++)
				temp[j] = C[i][j];

			int index =0;

			// Выполняем сдвиг всех элементов с индексами от cols до index 
			// временного буфера temp на один элемент вправо
			for (int k = Nn; k >= index; k--)
				temp[k + 1] = temp[k];

			// Элементу временного буфера с индексом index присваиваем случайное число
			temp[index] = 0;

			//if (temp != NULL)
			//{
			//	// Присваиваем значение указателя temp указателю i в массиве указателей matrix
			//	C[i] = temp;
			//}
		}
		Nn++; // Увеличиваем значение переменной количество столбцов cols на 1
	}

	PRINT_MATRIX( "C[K][N]:", C );

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
		v[j] = V[j]*(q[j] / Q[j]);
	}

	double sumV = SUM(V);
	// важность, не обеспечиваемая при обслуживании всех ГЦ
	for (int j = 0; j < N; j++)
	{
		sV = sumV - SV;
	}


	//СЕВЕРО-ЗАПАДНЫЙ УГОЛ---------------------------------

	double min_val;
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (Q[i] == 0)
				continue;

			min_val = min(W[i], Q[j]);
			C[i][j] = min_val;
			W[i] -= min_val;
			Q[j] -= min_val;

			if (W[i] == 0)
				break;
		}
	}


	PRINT_MATRIX( "Опорный план", C );
	
	system("pause");
	return 0;
}




