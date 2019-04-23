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

const int K = 3;														//���-�� �������������� ���(i=1,...,K)
const int N = 3;														//���-�� �� (j=1,...,N)

double V[N];															//�������� j-�� �� (0<=Vj<=100)

double Q[N], sumQ;														//��������� ���������������� ���������, ����������� ��� ������������ ���� ����� � �� (0<=Qj<=1000)
double W[K], sumW;														//������������� ���������������� ��������� ���, ����������� ��� ������������� � ����� ���� ����� � �� (0<=Wi<=5000)
double w[K][N];															//��������� i-�� ���, ���������� �� j-�� �� (0<=wij<=1000)
double q[N];															//���������������� ���������, ���������� ����� ��� �� j-�� ��

double v[N];															//�������������� ��� ������������ j-�� �� ��������
double SV;																//��������, �������������� ��� ������������ ���� ��
double p[K][N];															//����������� ����������� ������������ i-�� ��� j-�� �� (0<=pij<=1)

double sV;																//��������, �� �������������� ��� ������������ ���� ��

double C[K][N];															//����������� ��� �� (�ij= Vj/Qj*pij)

int main()
{

	//������ �������� �� �������
	cout << fixed;
	cout.precision(3);

	//����������� �������� �����
	setlocale(LC_ALL, "Russian");

	cout << "���������� ��:  " << N << endl;
	cout << "���������� ���: " << K << endl << endl;

	//������������� ���������������� ��������� ���, ����������� ��� ������������� � ����� ���� ����� � �� (0<=Wi<=5000)
	
	for (int i = 0; i < K; i++)
	{
		W[i] = rand() % 5000;
	}
	sumW = 0;
	for (int i = 0; i < K; i++)
	{
		sumW = sumW + W[i];
	}

	//��������� ���������������� ���������, ����������� ��� ������������ ���� ����� � �� (0<=Qj<=1000)
	
	for (int j = 0; j < N; j++)
	{
		Q[j] = rand() % 1000;
	}
	sumQ = 0;
	for (int j = 0; j < N; j++)
	{
		sumQ = sumQ + Q[j];
		
	}


	//����� W
	cout << "W[K]: ";
	for (int i = 0; i < K; i++)
	{
		cout << W[i] << "  ";
	}
	cout << endl << "������������� �����: " << sumW << endl << endl;

	//����� Q
	cout << "Q[N]: ";
	for (int j = 0; j < N; j++)
	{
		cout << Q[j] << "  ";
	}
	cout << endl << "��������� �����: " << sumQ << endl << endl;


	if (sumW < sumQ)
	{
		cout << "��������� ������! ������������� ����� ������ ���� ������ ���������!" << endl;
	}
	else
	{
		if (sumW > sumQ)
		{
			cout << "sumW > sumQ ( " << sumW << " > " << sumQ << " ), ������ ������ ��������� ���������� Q[" << N + 1 << "]" << endl << endl;
		}
	}
	//�������� j-�� �� (0<=Vj<=100)
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

	//����������� ����������� ������������ i-�� ��� j-�� �� (0<=pij<=1)
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
	//����������� ��� �� (�ij= Vj/Qj*pij)
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
// �������� �������� ���������� ������� �� ����� (� �����), � ������ Q[N] �������� Q[N+1]=sumW-sumQ
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

	////���������������� ���������, ���������� ����� ��� �� j-�� ��
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

	//��������� i-�� ���, ���������� �� j-�� �� (0<=wij<=1000)

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

	//�������������� ��� ������������ j-�� �� ��������
	for (int j = 0; j < N; j++)
	{
		v[j] = V[j]*(q[j] / Q[j]);
	}

	double sumV=0;
	for (int j = 0; j < N; j++)
	{
		sumV += V[j];
	}
	//��������, �� �������������� ��� ������������ ���� ��
	for (int j = 0; j < N; j++)
	{
		sV = sumV - SV;
	}


	//������-�������� ����---------------------------------

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

	cout << "������� ����" << endl<<endl;
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

