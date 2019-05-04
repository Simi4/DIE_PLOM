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



PlanMatrix::PlanMatrix(const int k, const int n) : k(k), n(n)
{
	resize(k);
	for (auto it = begin(); it != end(); it++)
	{
		it->resize(n);
	}
}


void PlanMatrix::reset_marks()
{
	for (size_t i = 0; i < k; i++)
	{
		for (size_t j = 0; j < n; j++)
		{
			(*this)[i][j].mark = 0;
			// (*this)[i][j].is_plan = (*this)[i][j].value != 0.0;
		}
	}
}



// Метод северо-западного угла
Table northwest_corner_method(const Matrix& mat, Vector& w, Vector& q)
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
	ret.plan = PlanMatrix(k, n);
	for (size_t i = 0; i < k; i++)
	{
		for (size_t j = 0; j < mat[i].size(); j++)
		{
			ret.plan[i][j].orig = mat[i][j]; // сохраняем значение Cij
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

			ret.plan[i][j].value = min_val;
			ret.plan[i][j].is_plan = true;

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
			ret += get(i, j) * plan[i][j].value;
		}
	}
	return ret;
}



// Конструктор: метод потенциалов
PotentialsMethod::PotentialsMethod(Table table) : differences(table.k(), table.n()),table(table), u(table.k()), v(table.n())
{
	//
}



void PotentialsMethod::calc_potentials()
{
	fill(u.begin(), u.end(), 0.0);
	fill(v.begin(), v.end(), 0.0);

	std::vector<bool> rows(table.k());
	fill(rows.begin(), rows.end(), false);
	std::vector<bool> columns(table.n());
	fill(columns.begin(), columns.end(), false);

	rows[0] = true;

	while (true)
	{
		auto cnt1 = std::count(rows.cbegin(), rows.cend(), false);
		auto cnt2 = std::count(columns.cbegin(), columns.cend(), false);

		if (!cnt1 && !cnt2)
		{
			break;
		}

		for (int i = 0; i < table.k(); ++i)
		{
			for (int j = 0; j < table.n(); ++j)
			{
				if (!table.plan[i][j].is_plan)
				{
					continue;
				}

				if (!rows[i] && !columns[j])
				{
					continue;
				}

				if (!rows[i])
				{
					rows[i] = true;
					u[i] = table[i][j] - v[j];
				}

				if (!columns[j])
				{
					columns[j] = true;
					v[j] = table[i][j] - u[i];
				}
			}
		}
	}
}

void PotentialsMethod::calc_differences()
{
	for (int i = 0; i < table.k(); ++i)
	{
		for (int j = 0; j < table.n(); ++j)
		{
			differences[i][j] = table[i][j] - (u[i] + v[j]);
		}
	}
}



bool PotentialsMethod::is_optimal()
{
	calc_potentials();
	calc_differences();

	for (int i = 0; i < table.k(); ++i)
	{
		for (int j = 0; j < table.n(); ++j)
		{
			if (differences[i][j] > 0.0)
			{
				return false;
			}
		}
	}
	return true;
}



void PotentialsMethod::optimize()
{
	// Индекс вершины многоугольника (пометим как [+])
	PosPtr top;

	double abs_max = 0.0;
	for (int i = 0; i < table.k(); ++i)
	{
		for (int j = 0; j < table.n(); ++j)
		{
			if (differences[i][j] > abs_max)
			{
				abs_max = differences[i][j];
				top = std::make_shared<Pos>(i, j);
			}
		}
	}
	cout << top->i << " " << top->j << endl;

	// строим цикл
	Cycle cycle = build_cycle(top);

	// Очищаем метки
	table.plan.reset_marks();

	// наименьшее из стоящих в минусовых клетках
	double mval = 0.0;
	int mi = -1, mj = -1;

	int next = 1;
	for (auto& pos : cycle)
	{
		table.plan[pos->i][pos->j].mark = next;

		if (next == -1)
		{
			if (mi == -1 || mval > table.plan[pos->i][pos->j].value)
			{
				mi = pos->i;
				mj = pos->j;
				mval = table.plan[mi][mj].value;
			}
		}

		next *= -1;
	}

	//cout << table.plan << endl << endl;

	for (int i = 0; i < table.plan.k; ++i)
	{
		for (int j = 0; j < table.plan.n; ++j)
		{
			if (table.plan[i][j].mark == -1)
			{
				table.plan[i][j].value -= mval;
			}
			else if (table.plan[i][j].mark == 1)
			{
				table.plan[i][j].is_plan = true;
				table.plan[i][j].value += mval;
			}
		}
	}

	cout << table.plan << endl;
	table.plan[mi][mj].is_plan = false;
}


// Поиск цикла вершин многоугольника
PotentialsMethod::Cycle PotentialsMethod::build_cycle(PosPtr top)
{
	Cycle firstCycle = { top };
	Cycles cycles = { firstCycle };

	while (true)
	{
		for (auto i = 0; i < cycles.size(); ++i)
		{
			if (check_cycle_final(cycles[i]))
			{
				// удаляем последний элемент
				cycles[i].pop_back();
				return cycles[i];
			}
			make_cycle_branches(cycles, cycles[i]);
		}
	}
}

bool PotentialsMethod::check_cycle_final(const Cycle& cycle) const
{
	// Ну минимум прямоугольник
	if (cycle.size() < 4)
	{
		return false;
	}

	auto first = cycle.front();
	auto last = cycle.back();

	// Цикл замкнулся?
	return *first == *last;
}

void PotentialsMethod::make_cycle_branches(Cycles& cycles, Cycle& cycle)
{
	auto first = cycle.front();

	auto last = cycle.back();
	auto secondLast = cycle.size() > 1 ? *(cycle.rbegin() + 1) : nullptr;

	Cycle originalCycle;
	copy(cycle.begin(), cycle.end(), back_inserter(originalCycle));

	bool sameCycle = true;

	// проходим по строке
	for (auto j = 0; j < table.n(); ++j)
	{
		PosPtr pos = std::make_shared<Pos>(last->i, j);

		if (*pos == *last)
		{
			continue;
		}

		if (secondLast && secondLast->i == pos->i && j > last->j - secondLast->j)
		{
			continue;
		}

		if (!table.plan[pos->i][pos->j].is_plan && !(*pos == *first))
		{
			continue;
		}

		if (sameCycle)
		{
			cycle.push_back(pos);
			sameCycle = false;
		}
		else
		{
			Cycle newBranch;
			copy(originalCycle.begin(), originalCycle.end(), back_inserter(newBranch));
			newBranch.push_back(pos);
			cycles.push_back(newBranch);
		}
	}

	// проходим по столбцу
	for (auto i = 0; i < table.k(); ++i)
	{
		PosPtr pos = std::make_shared<Pos>(i, last->j);

		if (*pos == *last)
		{
			continue;
		}

		if (!table.plan[pos->i][pos->j].is_plan)
		{
			continue;
		}

		if (secondLast && secondLast->j == pos->j && pos->i > last->j - secondLast->i)
		{
			continue;
		}

		Cycle newBranch;
		copy(originalCycle.begin(), originalCycle.end(), back_inserter(newBranch));
		newBranch.push_back(pos);
		cycles.push_back(newBranch);
	}
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


// Функция вывода матрицы опорного плана
std::ostream& operator<< (std::ostream& os, const PlanMatrix& val)
{
	auto prec = os.precision();

	for (int i = 0; i < val.k; i++)
	{
		for (int j = 0; j < val.n; j++)
		{
			if (!val[i][j].is_plan)
			{
				os << std::setw(3 + prec) << val[i][j].orig << std::setw(11);
			}
			else
			{
				os << std::setw(3 + prec) << val[i][j].orig << "[" << std::setw(4) << std::setprecision(0) << val[i][j].value << std::setprecision(prec) << "]" << std::setw(5);
			}
			if (val[i][j].mark == 1)
			{
				os << "[+]";
			}
			else if (val[i][j].mark == -1)
			{
				os << "[-]";
			}
			else
			{
				os << "";
			}
		}
		os << endl;
	}
	return os;
}
