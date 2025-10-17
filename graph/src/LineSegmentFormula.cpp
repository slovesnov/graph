#include "aslov.h"
#include "LineSegmentFormula.h"

LineSegmentFormula::LineSegmentFormula() {
	a = b = 0;
	p = nullptr;
}
LineSegmentFormula::~LineSegmentFormula() {
	clear();
}

bool LineSegmentFormula::compile() {
	try {
		clear();
		p = new ExpressionEstimator();
		p->compile(s, "x");
		return true;
	} catch (std::exception &e) {
		clear();
		return false;
	}
}

void LineSegmentFormula::clear() {
	if (p) {
		delete p;
		p = nullptr;
	}

}
