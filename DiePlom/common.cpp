#include "common.h"


// Конструктор класса Vector
Vector::Vector(const int size)
{
	resize(size);
	fill(begin(), end(), 0.0);
}



// Функция нахождения суммы в векторе
double Vector::sum() const
{
	return accumulate(begin(), end(), 0.0);
}



// Метод заполнения вектора целыми случайными числам (a .. b)
void Vector::fill_random(default_random_engine& gen, const int a, const int b)
{
	uniform_int_distribution<unsigned> dist(a, b);
	for (auto it = begin(); it != end(); it++)
	{
		*it = static_cast<double>(dist(gen));
	}
}



// Метод заполнения вектора случайными числам (a .. b)
void Vector::fill_random(default_random_engine& gen, const double a, const double b)
{
	uniform_real_distribution<double> dist(a, b);
	for (auto it = begin(); it != end(); it++)
	{
		*it = dist(gen);
	}
}



// Конструктор класса Matrix
Matrix::Matrix(const int k, const int n)
{
	matrix_resize(k, n);
}



// Изменение размерности матрицы
void Matrix::matrix_resize(const int k, const int n)
{
	resize(k);
	for (auto it = begin(); it != end(); it++)
	{
		it->resize(n);
		fill(it->begin(), it->end(), 0.0);
	}
}



// Метод заполнения матрицы целыми случайными числам (a .. b)
void Matrix::fill_random(default_random_engine& gen, const int a, const int b)
{
	for (auto it = begin(); it != end(); it++)
	{
		it->fill_random(gen, a, b);
	}
}



// Метод заполнения матрицы случайными числам (a .. b)
void Matrix::fill_random(default_random_engine& gen, const double a, const double b)
{
	for (auto it = begin(); it != end(); it++)
	{
		it->fill_random(gen, a, b);
	}
}



int Matrix::k() const
{
	return size();
}

int Matrix::n() const
{
	return (*this)[0].size();
}

double Matrix::get(const int i, const int j) const
{
	return (*this)[i][j];
}



// Метод северо-западного угла
Table Table::northwest_corner(const Matrix& mat, Vector& w, Vector& q)
{
	Table ret;

	auto k = w.size();
	auto n = q.size();

	ret.suppliers = w;
	ret.consumers = q;

	auto sum_w = w.sum();
	auto sum_q = q.sum();

	cout << "W[K]: " << ret.suppliers << endl;
	cout << "Располагаемая сумма: " << sum_w << endl;
	cout << endl;

	cout << "Q[N]: " << ret.consumers << endl;
	cout << "Потребная сумма: " << sum_q << endl;
	cout << endl;

	if (sum_w > sum_q)
	{
		cout << "sumW > sumQ ( " << sum_w << " > " << sum_q << " ), ";
		cout << "значит вводим фиктивную переменную Q[" << n + 1 << "]" << endl;

		n = n + 1;
		ret.consumers.resize(n);
		ret.consumers[n - 1] = sum_w - sum_q;
	}
	else if (sum_w < sum_q)
	{
		throw std::runtime_error("Проверьте баланс! Располагаемая сумма должна быть больше потребной!");
	}
	else /* if (sum_w == sum_q) */
	{
		// Nothing to do
	}

	// Заполнение матрицы тарифов
	ret.matrix_resize(k, n);
	ret.plan.matrix_resize(k, n);
	for (size_t i = 0; i < k; i++)
	{
		for (size_t j = 0; j < mat[i].size(); j++)
		{
			ret[i][j] = mat[i][j];
		}
	}


	cout << "Матрица тарифов" << endl;
	cout << ret << endl << endl;


	// Опорный план: заполнение
	double min_val = 0.0;
	for (size_t i = 0; i < k; ++i)
	{
		for (size_t j = 0; j < n; ++j)
		{
			if (ret.consumers[j] == 0.0)
			{
				continue;
			}

			min_val = std::min(ret.suppliers[i], ret.consumers[j]);

			ret.plan[i][j] = min_val;

			ret.suppliers[i] -= min_val;
			ret.consumers[j] -= min_val;

			if (ret.suppliers[i] == 0.0)
			{
				break;
			}
		}
	}

	return ret;
}


// Целевая функция
double Table::SV() const
{
	double ret = 0.0;
	for (int i = 0; i < k(); ++i)
	{
		for (int j = 0; j < n(); ++j)
		{
			ret += get(i, j) * plan[i][j];
		}
	}
	return ret;
}



// Функция вывода Vector
std::ostream& operator<< (std::ostream& os, const Vector& val)
{
	for (const auto& it : val)
	{
		os << std::setw(8 + os.precision()) << it;
	}
	return os;
}



// Функция вывода Matrix
std::ostream& operator<< (std::ostream& os, const Matrix& val)
{
	for (const auto& it : val)
	{
		os << it << endl;
	}
	return os;
}


// Функция вывода матрицы тарифов + запасы и потребности
std::ostream& operator<< (std::ostream& os, const Table& val)
{
	auto prec = os.precision();
	os << std::setw(6 + val.n() * (prec + 8)) << "C[K][N]" << "W[K]" << endl;

	for (int i = 0; i < val.k(); i++)
	{
		os << std::setw(6) << "";
		for (int j = 0; j < val.n(); j++)
		{
			os << std::setw(prec + 8) << val[i][j];
		}
		os << std::setprecision(0) << val.suppliers[i] << std::setprecision(prec) << endl;
	}
	os << std::setw(6) << "Q[N]" << val.consumers << endl;
	return os;
}
