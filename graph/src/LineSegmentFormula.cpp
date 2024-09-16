/*
 * LineSegmentFormula.cpp
 *
 *  Created on: 16.09.2024
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#include "aslov.h"
#include "LineSegmentFormula.h"

LineSegmentFormula::LineSegmentFormula() {
	a = b = 0;
	p =nullptr;
}
LineSegmentFormula::~LineSegmentFormula() {
	clear();
}

bool LineSegmentFormula::compile() {
	try {
		p =new ExpressionEstimator();
		p->compile(s,"x");
		return true;
	} catch (std::exception &e) {
		return false;
	}
}

/*
LineSegmentFormula& LineSegmentFormula::operator =(LineSegmentFormula &&o) {
	a=o.a;
	b=o.b;
	s=o.s;
	//s=std::move(o.s);
	clear();
	p=o.p;
	return *this;
}
*/

void LineSegmentFormula::clear() {
	if (p) {
		delete p;
	}

}
