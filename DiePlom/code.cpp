// Если компилируем под Win32
#ifdef _WIN32
#  include <windows.h>
#endif

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "transportation_problem/potentials_method.h"

using namespace transportation_problem;
using namespace std;



template <typename T>
void file_input(ifstream& file, T& x)
{
	string s; file >> s; file >> x;
	if (!file.good())
	{
		cout << "Файл имеет некорректную структуру!" << endl; cin.get();
		exit(EXIT_FAILURE);
	}
}



int main()
{
#ifdef _WIN32
	// Кодировка консоли
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif

	cout << fixed << left << setprecision(3);

	// кол-во многоканальных РЛС ( i = 1, ..., K )
	size_t K;

	// кол-во ГЦ ( j = 1, ..., N )
	size_t N;

	// открываем файл для чтения
	ifstream file("../input.txt");

	if (!file.is_open())
	{
		cout << "Невозможно открыть файл!" << endl; cin.get();
		return EXIT_FAILURE;
	}

	file >> K >> N;

	if (!file.good())
	{
		cout << "Файл имеет некорректную структуру!" << endl; cin.get();
		return EXIT_FAILURE;
	}


	cout << "Количество ГЦ  (N): " << N << endl;
	cout << "Количество РЛС (K): " << K << endl << endl;


	// важность j-ой ГЦ ( 0 <= Vj <= 100 )
	Vector V(N);
	file_input(file, V);


	// потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ ( 0 <= Qj <= 1000 )
	Vector Q(N);
	file_input(file, Q);


	// располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ ( 0 <= Wi <= 5000 )
	Vector W(K);
	file_input(file, W);


	// потенциал i-ой РЛС, выделяемый на j-ую ГЦ ( 0 <= wij <= 1000 )
	Matrix w(K, N);
	file_input(file, w);


	// радиолокационный потенциал, выделенный всеми РЛС на j-ую ГЦ
	Vector q(N);

	for (size_t i = 0; i < K; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			q[j] += w[i][j];
		}
	}


	// обеспечиваемая при обслуживании j-ой ГЦ важность
	Vector v(N);

	for (size_t j = 0; j < N; ++j)
	{
		v[j] = V[j] * (q[j] / Q[j]);
	}


	// важность, обеспечиваемая при обслуживании всех ГЦ
	double SV;


	// вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ ( 0 <= pij <= 1 )
	Matrix p(K, N);
	file_input(file, p);


	// Закроем файл - больше не нужен
	file.close();


	// важность, не обеспечиваемая при обслуживании всех ГЦ
	double sV;


	// коэффициент для ЦФ ( Сij = Vj / Qj * pij )
	Matrix C(K, N);

	for (size_t i = 0; i < K; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			C[i][j] = (V[j] / Q[j]) * p[i][j];
		}
	}


	cout << "C:" << endl << C;
	cout << "W: " << W << endl;
	cout << "Q: " << Q << endl << endl;

	// ПОСТРОЕНИЕ ОПОРНОГО ПЛАНА МЕТОДОМ СЕВЕРО-ЗАПАДНОГО УГЛА
	// НАЧАЛО МЕТОДА СЕВЕРО-ЗАПАДНОГО УГЛА

	TableNCM solution;

	try
	{
		solution = TableNCM(C, W, Q);
	}
	catch (const exception e)
	{
		cout << "Ошибка: " << e.what(); cin.get();
		return EXIT_FAILURE;
	}

	cout << "Опорный план" << endl;
	cout << solution.plan << endl;

	// целевая функция
	SV = solution.f();

	cout << "Целевая функция SV = " << SV << endl << endl;

	// КОНЕЦ МЕТОДА СЕВЕРО-ЗАПАДНОГО УГЛА




	// УЛУЧШЕНИЕ ОПОРНОГО ПЛАНА МЕТОДОМ ПОТЕНЦИАЛОВ
	// НАЧАЛО МЕТОДА ПОТЕНЦИАЛОВ

	auto optimizer = PotentialsMethod(solution);

	while (!optimizer.is_optimal())
	{
		optimizer.optimize();
	}

	cout << "Оптимальный план" << endl;
	cout << optimizer.table.plan << endl;

	SV = optimizer.table.f();
	cout << "Целевая функция SV = " << SV << endl << endl;

	// КОНЕЦ МЕТОДА ПОТЕНЦИАЛОВ



	double sumV = V.sum();
	// важность, не обеспечиваемая при обслуживании всех ГЦ
	for (size_t j = 0; j < N; j++)
	{
		sV = sumV - SV;
	}


	cin.get();
	return 0;
}

