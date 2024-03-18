/*
 * Graph.h
 *
 *  Created on: 13.04.2022
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <string>
#include <vector>

#include "aslov.h"
#include "Point.h"
#include "expressionEstimator.h"
#include "MinMaxBox.h"

enum class GraphType {
	SIMPLE, POLAR, PARAMETRICAL
};

class Graph {
public:
	MinMaxBox m_minmax;
	GtkWidget *m_box, *m_name[3], *m_entry[3], *m_combo, *m_typel, *m_button;
	bool m_ok[2], m_signals;
	int m_steps;
	std::string m_formula[2];
	ExpressionEstimator m_estimator[2];
	GraphType m_type;
	std::vector<Point> m_v;
	int m_colorIndex;

	Graph(GraphType type, int colorIndex);
	void setDefault(GraphType type, bool resetColor = false);
	void recount(double min, double max, int steps);
	void recount();
	void recountAnyway();
	void updateLanguage();
	void changeType(int type);
	void showHideWidgets();
	void inputChanged(GtkWidget *w);
	void setFormula(std::string s, int i);
	double calculate(int i, double v);
	void updateEnableClose();
	bool setSteps();
	bool inEntry(GtkWidget *w);
};

#endif /* GRAPH_H_ */
