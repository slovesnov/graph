#ifndef LINESEGMENTFORMULA_H_
#define LINESEGMENTFORMULA_H_

#include <vector>
#include "expressionEstimator.h"

class LineSegmentFormula {
public:
	double a, b;
	std::string s;
	ExpressionEstimator *p;
	LineSegmentFormula();
	~LineSegmentFormula();
	bool compile();
	void clear();
};

using VLineSegmentFormula=std::vector<LineSegmentFormula>;

#endif /* LINESEGMENTFORMULA_H_ */
