#include <fstream>
#include <iostream>
#include <vector>
#include <limits>
#include <stdio.h>
#include "conio.h"
#include "time.h"
#include <random>
#include <cmath>  
#include <cstdlib>
#include <algorithm>

using namespace std;

const int K = 3;														//кол-во многоканальных РЛС(i=1,...,K)
const int N = 3;														//кол-во ГЦ (j=1,...,N)

double V[N];															//важность j-ой ГЦ (0<=Vj<=100)

double Q[N], sumQ;														//потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ (0<=Qj<=1000)
double W[K], sumW;														//располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ (0<=Wi<=5000)
double w[K][N];															//потенциал i-ой РЛС, выделяемый на j-ую ГЦ (0<=wij<=1000)
double q[N];															//радиолокационный потенциал, выделенный всеми РЛС на j-ую ГЦ

double v[N];															//обеспечиваемая при обслуживании j-ой ГЦ важность
double SV;																//важность, обеспечиваемая при обслуживании всех ГЦ
double p[K][N];															//вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ (0<=pij<=1)

double sV;																//важность, не обеспечиваемая при обслуживании всех ГЦ

double C[K][N];															//коэффициент для ЦФ (Сij= Vj/Qj*pij)

int main()
{

	//задаем точность до десятых
	cout << fixed;
	cout.precision(3);

	//подключение русского языка
	setlocale(LC_ALL, "Russian");

	cout << "Количество ГЦ:  " << N << endl;
	cout << "Количество РЛС: " << K << endl << endl;

	//располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ (0<=Wi<=5000)
	
	for (int i = 0; i < K; i++)
	{
		W[i] = rand() % 5000;
	}
	sumW = 0;
	for (int i = 0; i < K; i++)
	{
		sumW = sumW + W[i];
	}

	//потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ (0<=Qj<=1000)
	
	for (int j = 0; j < N; j++)
	{
		Q[j] = rand() % 1000;
	}
	sumQ = 0;
	for (int j = 0; j < N; j++)
	{
		sumQ = sumQ + Q[j];
		
	}


	//вывод W
	cout << "W[K]: ";
	for (int i = 0; i < K; i++)
	{
		cout << W[i] << "  ";
	}
	cout << endl << "Располагаемая сумма: " << sumW << endl << endl;

	//вывод Q
	cout << "Q[N]: ";
	for (int j = 0; j < N; j++)
	{
		cout << Q[j] << "  ";
	}
	cout << endl << "Потребная сумма: " << sumQ << endl << endl;


	if (sumW < sumQ)
	{
		cout << "Проверьте баланс! Располагаемая сумма должна быть больше потребной!" << endl;
	}
	else
	{
		if (sumW > sumQ)
		{
			cout << "sumW > sumQ ( " << sumW << " > " << sumQ << " ), значит вводим фиктивную переменную Q[" << N + 1 << "]" << endl << endl;
		}
	}
	//важность j-ой ГЦ (0<=Vj<=100)
	for (int j = 0; j < N; j++)
	{
		V[j] = rand() % 100;
	}
	//cout << "V[N]: ";
	//for (int j = 0; j < N; j++)
	//{
	//	cout << V[j] << "  ";
	//}
	//cout << endl << endl;

	//вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ (0<=pij<=1)
	random_device rd;
	default_random_engine generator;
	uniform_real_distribution<double>distribution(0.0, 1.0);
	srand(time(0));

	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < N; j++)
		{
			p[i][j] = distribution(generator);
		}
	}
	//cout << "p[K][N]: " << endl << endl;
	//for (int i = 0; i < K; i++)
	//{
	//	for (int j = 0; j < N; j++)
	//	{
	//		cout<< p[i][j]<< "\t";
	//	}
	//	cout << "\n";
	//}
	//cout << endl;

	int NN = N+1;
	//коэффициент для ЦФ (Сij= Vj/Qj*pij)
	cout << "C[K][N]: "<<endl<<endl;
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


		for (int i = 0; i < K; i++)
		{
			for (int j = 0; j < N; j++)
			{
				cout << C[i][j] << "\t";
			}
			cout << "\n";
		}
	cout << endl << endl;

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
	//cout << "w[K][N]: " << endl << endl;
	//for (int i = 0; i < K; i++)
	//{
	//	for (int j = 0; j < N; j++)
	//	{
	//		cout << w[i][j] << "\t";
	//	}
	//	cout << "\n";
	//}
	//cout << endl << endl;

	//обеспечиваемая при обслуживании j-ой ГЦ важность
	for (int j = 0; j < N; j++)
	{
		v[j] = V[j]*(q[j] / Q[j]);
	}

	double sumV=0;
	for (int j = 0; j < N; j++)
	{
		sumV += V[j];
	}
	//важность, не обеспечиваемая при обслуживании всех ГЦ
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

	cout << "Опорный план" << endl<<endl;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			cout << C[i][j] << "\t";
		}
		cout << endl;
		
	}
	cout << endl;

	system("pause");
	return 0;
}

