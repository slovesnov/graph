#ifndef MINMAXBOX_H_
#define MINMAXBOX_H_

#include "aslov.h"
const char CERROR[] = "cerror";
const double MINY = -5;
const double MAXY = -MINY;
const char SEPARATOR = 8;

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
	void set(double mins, double maxs, bool _updateEntries = false);
	void set(std::string const s[]);
	void set(std::string const &s1, std::string const &s2);
	void setSize(int size);
	double toScreen(double v);
	double fromScreen(double v);
	void scale(double k);
	void updateEntries();
	void inputChanged(bool redraw = true);
	void setName(const char *s);
	bool ok();
	void updateEntryColor(int i);
	bool inEntry(GtkWidget *w);
	std::string toString();
};

#endif /* MINMAXBOX_H_ */
