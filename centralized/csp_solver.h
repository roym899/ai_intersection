#ifndef CSP_SOLVER_H
#define CSP_SOLVER_H

#include <vector>
#include <array>

class Range
{
public:
	Range(double start, double end) : start(start), end(end) {}
	~Range() {}

	double get_start() const { return start;}
	double get_end() const { return end;}

	void shift(double shift) { start += shift; end += shift; } 

	friend bool overlap(const Range &lhs, const Range &rhs);

private:
	double start, end;
};

class CSP_Solver
{
public:
	CSP_Solver() {};
	~CSP_Solver() {};
	
	std::vector< std::array<int, 4> > find_conflicts(const std::vector< std::vector<Range> >& range_sequences);
	void add_range_bin_sequence(double min_start, double max_start, std::vector<int> bin_sequence, std::vector<double> length_sequence);
	std::vector<double> solve();
	void solve(std::vector< std::vector<double> >& solutions,
               int depth);

	double get_shift(const std::array<int, 4>& shift_array) const;

private:
	std::vector<std::vector<Range>> range_sequences;
	std::vector<std::vector<int>> bin_sequences;
	std::vector<double> min_starts;
	std::vector<double> max_starts;
};



bool overlap(const Range &lhs, const Range &rhs);

#endif