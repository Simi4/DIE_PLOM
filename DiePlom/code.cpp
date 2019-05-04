#ifdef _WIN32
#include <windows.h>
#endif
#include "common.h"


// кол-во многоканальных РЛС ( i = 1, ..., K )
const int K = 3;

// кол-во ГЦ ( j = 1, ..., N )
const int N = 3;


// для проверки алгоритма
#define CHECK_ALG


// важность j-ой ГЦ ( 0 <= Vj <= 100 )
Vector V(N);


// потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ ( 0 <= Qj <= 1000 )
Vector Q(N);


// располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ ( 0 <= Wi <= 5000 )
Vector W(K);


// потенциал i-ой РЛС, выделяемый на j-ую ГЦ ( 0 <= wij <= 1000 )
Matrix w(K, N);


// радиолокационный потенциал, выделенный всеми РЛС на j-ую ГЦ
Vector q(N);


// обеспечиваемая при обслуживании j-ой ГЦ важность
Vector v(N);


// важность, обеспечиваемая при обслуживании всех ГЦ
double SV;


// вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ ( 0 <= pij <= 1 )
Matrix p(K, N);


// важность, не обеспечиваемая при обслуживании всех ГЦ
double sV;


// коэффициент для ЦФ ( Сij = Vj / Qj * pij )
Matrix C(K, N);



int main()
{
	// Генератор случайных чисел
	default_random_engine gen((unsigned)time(nullptr));
	srand((unsigned)time(nullptr));

#ifdef _WIN32
	// Кодировка консоли
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif

	cout << std::fixed << std::left << std::setprecision(3);

	cout << "Количество ГЦ  (N): " << N << endl;
	cout << "Количество РЛС (K): " << K << endl << endl;

	// располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ ( 0 <= Wi <= 5000 )
#ifdef CHECK_ALG
	W[0] = 10.0;
	W[1] = 20.0;
	W[2] = 30.0;
#else
	W.fill_random(gen, 0, 5000);
#endif


	// потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ ( 0 <= Qj <= 1000 )
#ifdef CHECK_ALG
	Q[0] = 15.0;
	Q[1] = 20.0;
	Q[2] = 25.0;
#else
	Q.fill_random(gen, 0, 1000);
#endif


	// важность j-ой ГЦ ( 0 <= Vj <= 100 )
	V.fill_random(gen, 0, 100);
	// cout << "V[N]: " << V << endl;


	// вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ ( 0 <= pij <= 1 )
	p.fill_random(gen, 0.0, 1.0);
	// cout << "p[K][N]:" << endl << p << endl;



#ifdef CHECK_ALG
	C[0][0] = 5.0; C[0][1] = 3.0; C[0][2] = 1.0;
	C[1][0] = 3.0; C[1][1] = 2.0; C[1][2] = 4.0;
	C[2][0] = 4.0; C[2][1] = 1.0; C[2][2] = 2.0;
#else
	// коэффициент для ЦФ ( Сij = Vj/Qj*pij )
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < N; j++)
		{
			C[i][j] = (V[j] / Q[j]) * p[i][j];
		}
	}
#endif



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
	w.fill_random(gen, 0, 1000);
	// cout << "w[K][N]:" << endl << w;


	// обеспечиваемая при обслуживании j-ой ГЦ важность
	for (int j = 0; j < N; j++)
	{
		v[j] = V[j] * (q[j] / Q[j]);
	}




	// ПОСТРОЕНИЕ ОПОРНОГО ПЛАНА МЕТОДОМ СЕВЕРО-ЗАПАДНОГО УГЛА
	// НАЧАЛО МЕТОДА СЕВЕРО-ЗАПАДНОГО УГЛА

	auto solution = northwest_corner_method(C, W, Q);

	cout << "Опорный план" << endl;
	cout << solution.plan << endl;

	// целевая функция
	SV = solution.SV();

	cout << "Целевая функция SV = " << SV << endl << endl;

	// КОНЕЦ МЕТОДА СЕВЕРО-ЗАПАДНОГО УГЛА




	// УЛУЧШЕНИЕ ОПОРНОГО ПЛАНА МЕТОДОМ ПОТЕНЦИАЛОВ
	// НАЧАЛО МЕТОДА ПОТЕНЦИАЛОВ

	auto optimizer = PotentialsMethod(solution);

	while (!optimizer.is_optimal())
	{
		cout << endl;
		cout << "u[K]: " << optimizer.u << endl;
		cout << "v[N]: " << optimizer.v << endl;
		cout << "Опорный план не является оптимальным" << endl;
		optimizer.optimize();
	}

	cout << " ==================== "<< endl;
	cout << "u[K]: " << optimizer.u << endl;
	cout << "v[N]: " << optimizer.v << endl;
	cout << "Опорный план является оптимальным" << endl;

	SV = optimizer.table.SV();
	cout << "Целевая функция SV = " << SV << endl << endl;

	// КОНЕЦ МЕТОДА ПОТЕНЦИАЛОВ




	double sumV = V.sum();
	// важность, не обеспечиваемая при обслуживании всех ГЦ
	for (int j = 0; j < N; j++)
	{
		sV = sumV - SV;
	}


	std::cin.get();
	return 0;
}

