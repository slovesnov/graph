/*
 * LineSegmentFormula.h
 *
 *  Created on: 16.09.2024
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef LINESEGMENTFORMULA_H_
#define LINESEGMENTFORMULA_H_

#include <vector>
#include "expressionEstimator.h"

class LineSegmentFormula {
public:
	double a,b;
	std::string s;
	ExpressionEstimator*p;
	LineSegmentFormula();
	~LineSegmentFormula();
	bool compile();
	void clear();
};

using VLineSegmentFormula=std::vector<LineSegmentFormula>;

#endif /* LINESEGMENTFORMULA_H_ */
