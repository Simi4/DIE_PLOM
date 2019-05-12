﻿#pragma once
#include "transportation_problem/potentials_method.h"

using namespace transportation_problem;


class Solver {
public:
	// кол-во многоканальных РЛС ( i = 1, ..., K )
	size_t K;

	// кол-во ГЦ ( j = 1, ..., N )
	size_t N;

	// важность j-ой ГЦ ( 0 <= Vj <= 100 )
	Vector V;

	// потребный радиолокационный потенциал, необходимый для обслуживания всех целей в ГЦ ( 0 < Qj <= 1000 )
	Vector Q;

	// располагаемый радиолокационный потенциал РЛС, необходимый для сопровождения и поиск всех целей в ГЦ ( 0 <= Wi <= 5000 )
	Vector W;

	// потенциал i-ой РЛС, выделяемый на j-ую ГЦ ( 0 <= wij <= 1000 )
	Matrix w;

	// радиолокационный потенциал, выделенный всеми РЛС на j-ую ГЦ
	Vector q;

	// обеспечиваемая при обслуживании j-ой ГЦ важность
	Vector v;

	// вероятность обеспечения обслуживания i-ой РЛС j-ой ГЦ ( 0 <= pij <= 1 )
	Matrix p;

	// коэффициент для ЦФ ( Сij = Vj / Qj * pij )
	Matrix C;

	// важность, обеспечиваемая при обслуживании всех ГЦ
	double SV;

	// важность, не обеспечиваемая при обслуживании всех ГЦ
	double sV;

	// Solver() = default;
	Solver(const size_t K, const size_t N);

	bool solve();
};
