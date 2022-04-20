/*
 * MinMaxBox.h
 *
 *  Created on: 13.04.2022
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef MINMAXBOX_H_
#define MINMAXBOX_H_

#include "aslov.h"
const char CERROR[] = "cerror";
const double MINY=-5;
const double MAXY=-MINY;

class Graph;

class MinMaxBox {
public:
	bool m_signals;
	int m_size;
	GtkWidget *m_entry[2], *m_box, *m_name;
	double m_min, m_max;
	bool m_ok[2], m_invert;
	Graph *m_graph;

	void init(const char *name, bool invert = false, bool xy = true);
	void set(double min, double max, bool _updateEntries = false);
	void setSize(int size);
	double toScreen(double v);
	double fromScreen(int v);
	void scale(double k);
	void updateEntries();
	void inputChanged();
	void setName(const char *s);
	bool ok();
	void updateEntryColor(int i);
};

#endif /* MINMAXBOX_H_ */
