#pragma once
#include "northwest_corner_method.h"


namespace transportation_problem
{
	class PotentialsMethod {
	private:
		typedef std::pair<size_t, size_t> Point;
		Matrix differences;
		Vector u;
		Vector v;

		void calc_differences();
		bool calc_potentials();

		void recalc_x_zero();

		Point top;
		std::vector<Point> cycle;

		bool find_cycle();
		bool find_cycle_row(Point& pos);
		bool find_cycle_column(Point& pos);

	public:
		TableNCM table;

		PotentialsMethod(TableNCM table);
		PotentialsMethod() = default;

		bool is_optimal();
		void optimize();
	};
}
