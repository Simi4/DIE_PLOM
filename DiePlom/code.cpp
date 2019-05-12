// Если компилируем под Win32
#ifdef _WIN32
#  include <windows.h>
#endif

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "solver.h"

using namespace std;



template <typename T>
void file_input(ifstream& file, T& x)
{
	string s; file >> s; file >> x;
	if (!file.good())
	{
		cerr << "Error: File incorrect!" << endl; cin.get();
		exit(EXIT_FAILURE);
	}
}



int main()
{
#ifdef _WIN32
	// Кодировка консоли
	// В Visual Studio 2013 не работает
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif

	cout << fixed << left << setprecision(3);
	cerr << setprecision(8);


	// открываем файл для чтения
	ifstream file("../input.txt");
	if (!file.is_open())
	{
		cerr << "Error: Cannot open file!" << endl; cin.get();
		return EXIT_FAILURE;
	}


<<<<<<< HEAD

	cout << "Количество РЛС (K): " << K << endl;
	cout << "Количество ГЦ  (N): " << N << endl << endl;


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
=======
	size_t K, N;
	file_input(file, K);
	file_input(file, N);
>>>>>>> 730be3c3ecb083bd9653f607ac33f4bf4ca8c58f

	Solver solver(K, N);

	// Вводим данные из файла в solver
	// Порядок ввода как в файле!
	file_input(file, solver.V);
	file_input(file, solver.Q);
	file_input(file, solver.W);
	file_input(file, solver.w);
	file_input(file, solver.p);

	// Закроем файл - больше не нужен
	file.close();

<<<<<<< HEAD

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


	cout << "C:" << endl << C << endl;
	cout << "W: " << W << endl;
	cout << "Q: " << Q << endl << endl;

	// ПОСТРОЕНИЕ ОПОРНОГО ПЛАНА МЕТОДОМ СЕВЕРО-ЗАПАДНОГО УГЛА
	// НАЧАЛО МЕТОДА СЕВЕРО-ЗАПАДНОГО УГЛА

	TableNCM solution;

	try
=======
	// Вычисление
	if (!solver.solve())
>>>>>>> 730be3c3ecb083bd9653f607ac33f4bf4ca8c58f
	{
		cin.get();
		return EXIT_FAILURE;
	}

	cin.get();
	return 0;
}

