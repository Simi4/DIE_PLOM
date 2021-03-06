﻿#include "solver.h"
#include <iostream>

using namespace std;


Solver::Solver(const size_t K, const size_t N)
	: K(K)
	, N(N)
	, V(N)
	, Q(N)
	, W(K)
	, w(K, N)
	, q(N)
	, v(N)
	, p(K, N)
	, C(K, N)
	, F(numeric_limits<double>::quiet_NaN())
	, sV(numeric_limits<double>::quiet_NaN())
{
	cout << "Количество РЛС (K): " << K << endl ;
	cout << "Количество ГЦ  (N): " << N << endl << endl;
}



bool Solver::solve()
{
	// Выполняем проверку на принадлежность диапозону ( 0 < Vj <= 100 )
	for (size_t j = 0; j < N; ++j)
	{
		if (V[j] <= numeric_limits<double>::epsilon() || V[j] > 100.0)
		{
			cerr << "Ошибка: V[" << j << "] = " << V[j] << endl;
			cerr << "Должно выполняться условие: ( 0 < Vj <= 100 )" << endl;
			return false;
		}
	}

	// Выполняем проверку на принадлежность диапозону ( 0 < Qj <= 1000 )
	for (size_t j = 0; j < N; ++j)
	{
		if (Q[j] <= numeric_limits<double>::epsilon() || Q[j] > 1000.0)
		{
			cerr << "Ошибка: Q[" << j << "] = " << Q[j] << endl;
			cerr << "Должно выполняться условие: ( 0 < Qj <= 1000 )" << endl;
			return false;
		}
	}

	// Выполняем проверку на принадлежность диапозону ( 0 <= Wi <= 5000 )
	for (size_t i = 0; i < K; ++i)
	{
		if (W[i] < 0.0 || W[i] > 5000.0)
		{
			cerr << "Ошибка: W[" << i << "] = " << W[i] << endl;
			cerr << "Должно выполняться условие: ( 0 <= Wi <= 5000 )" << endl;
			return false;
		}
	}

	// Выполняем проверку на принадлежность диапозону ( 0 <= wij <= 1000 )
	for (size_t i = 0; i < K; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			if (w[i][j] < 0.0 || w[i][j] > 1000.0)
			{
				cerr << "Ошибка: w[" << i << "][" << j << "] = " << w[i][j] << endl;
				cerr << "Должно выполняться условие: ( 0 <= wij <= 1000 )" << endl;
				return false;
			}
		}
	}

	// Выполняем проверку на выполнения условия: сумма элементов каждой строки матрицы w
	// не должна превышать соответствующего значения W
	for (size_t i = 0; i < K; ++i)
	{
		//сумма строки матрицы w
		double summ_w = 0.0;
		for (size_t j = 0; j < N; ++j)
		{
			summ_w += w[i][j];
		}
		//cout <<" summ: " << summ_w << "\n";

		if (summ_w > W[i])
		{
			cerr << "Ошибка! Должно выполняться условие: sum wij<= Wi ("<< W[i] <<")"<< endl;
			return false;
		}
	}

	//Выполняем проверку на p (0<p<1)
	for (size_t i = 0; i < K; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			if (p[i][j] < 0.0 || p[i][j] > 1.0)
			{
				cerr << "Ошибка: p[" << i << "][" << j << "] = " << p[i][j] << endl;
				cerr << "Должно выполняться условие: ( 0 <= p <= 1 )" << endl;
				return false;
			}
		}
	}

	//Выполняем проверку на sum p*w<=Q
	for (size_t j = 0; j < N; ++j)
	{
		//сумма строки матриц w и p
		double summ_pw = 0.0;
		for (size_t i = 0; i < K; ++i)
		{
			summ_pw += p[i][j]*w[i][j];
			/*cout << i << " : " << summ_pw << "\t";*/
			
		}


		if (summ_pw > Q[j])
		{
			cerr << "Ошибка! Должно выполняться условие: sum pij*wij<=Qj (" << Q[j] << ")" << endl;
			return false;
		}
		
	}

	//for j = 0 ... {
	//	тут сброс значения sum
	//		for i = 0 ... {
	//			sum += wij * pij
	//		}
	//	тут проверка sum < Qj

	// Вычисление qj
	for (size_t i = 0; i < K; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			q[j] += w[i][j];
		}
	}

	// Вычисление vj
	for (size_t j = 0; j < N; ++j)
	{
		v[j] = V[j] * (q[j] / Q[j]);
	}

	// Вычисление Cij
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
	{
		solution = TableNCM(C, W, Q);
	}
	catch (const exception e)
	{
		cerr << "Ошибка: " << e.what();
		return false;
	}

	cout << "Опорный план" << endl;
	cout << solution.plan << endl;

	// целевая функция
	F = solution.f();

	cout << "ЦФ опорного плана F = " << F << endl << endl;

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

	F = optimizer.table.f();
	cout << "ЦФ оптимального плана F* = " << F << endl << endl;

	// КОНЕЦ МЕТОДА ПОТЕНЦИАЛОВ



	double sumV = V.sum();
	// важность, не обеспечиваемая при обслуживании всех ГЦ
	for (size_t j = 0; j < N; j++)
	{
		sV = sumV - F;
	}

	return true;
}
