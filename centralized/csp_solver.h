#ifndef CSP_SOLVER_H
#define CSP_SOLVER_H

#include <vector>
#include <array>

class Range
{
/* Representation of a range, adds a simple way to check if they overlap or not 
*/
public:
	Range(double start, double end) : start(start), end(end) {}
	~Range() {}

	double get_start() const { return start;}
	double get_end() const { return end;}

	// shifts the start and end of the range by the passed shift parameter
	void shift(double shift) { start += shift; end += shift; } 

	friend bool overlap(const Range &lhs, const Range &rhs);

private:
	double start, end;
};

class CSP_Solver
{
/* Solver class for our CSP problem based on bins (which can only be occupied by one range at a time), 
   range and bin sequence variables, and minimum and maximum starts, and hard (non shiftable) ranges. */
public:
	CSP_Solver() {};
	~CSP_Solver() {};
	
	std::vector< std::array<int, 5> > find_conflicts(const std::vector< std::vector<Range> >& range_sequences);
	
	// adds a non-shiftable constraint to a bin for a given range
	void add_range_bin_constraint(int bin, Range range);
	
	// adds a shiftable range bin sequence, which can only be shifted all at once
	void add_range_bin_sequence(double min_start, double max_start, std::vector<int> bin_sequence, std::vector<double> length_sequence);
	
	// uses iterative deepending to search for a solution and returns only the minimum end time solution
	std::vector<double> solve();
	
	// solve function used for recursive calling, adds solution to the passed solution vector, depth is the remaining depth
	void solve(std::vector< std::vector<double> >& solutions,
               int depth);

	// calculates how much a range should be shifted, shift array is an array with the following structure:
	// [range_id_1 bin_id_1 range_id_2 bin_id_2 hard_constraint]
	// the id_1 sequence is the one which is supposed to be shifted
	// the id_2 sequence is the one it has a conflict with
	// hard constraint is 1 if id_2 is just a hard constraint range not a rangebin_sequence
	// it is 0 if id_2 is also a shiftable range_bin sequence
	double get_shift(const std::array<int, 5>& shift_array) const;

private:
	std::vector<int> fixed_bins;
	std::vector<Range> fixed_ranges;
	std::vector<std::vector<Range>> range_sequences;
	std::vector<std::vector<int>> bin_sequences;
	std::vector<double> min_starts;
	std::vector<double> max_starts;
};



bool overlap(const Range &lhs, const Range &rhs);

#endif
