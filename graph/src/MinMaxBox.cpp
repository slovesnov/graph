/*
 * MinMaxBox.cpp
 *
 *  Created on: 13.04.2022
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#include "MinMaxBox.h"
#include "GraphWindow.h"

extern GraphWindow *pWindow;

static void input_changed(GtkWidget*, gpointer g) {
	((MinMaxBox*) g)->inputChanged();
}

void MinMaxBox::init(const char *name, bool invert/*=false*/,
		bool xy/*=true*/) {
	int i;
	GtkWidget *b;
	std::string s;
	m_graph = nullptr;
	m_signals = true;
	m_invert = invert;

	m_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	m_name = gtk_label_new(name);
	gtk_box_pack_start(GTK_BOX(m_box), m_name, FALSE, FALSE, 0);

	for (i = 0; i < 2; i++) {
		b = m_entry[i] = gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(m_box), b, 1, 1, 0);
		if (!i) {
			b = gtk_label_new("-");
			gtk_box_pack_start(GTK_BOX(m_box), b, FALSE, FALSE, 0);
		}
	}

	for (i = 0; i < 2; i++) {
		g_signal_connect(m_entry[i], "changed", G_CALLBACK(input_changed),
				this);
	}
}

void MinMaxBox::set(double min, double max, bool _updateEntries/*=false*/) {
	int i;
	m_min = min;
	m_max = max;
	for (i = 0; i < 2; i++) {
		m_ok[i] = min < max;
	}
	if (_updateEntries) {
		updateEntries();
		for (i = 0; i < 2; i++) {
			updateEntryColor(i);
		}
	}
}

void MinMaxBox::set(std::string const s[]) {
	m_signals = false;
	for (int i = 0; i < 2; i++) {
		gtk_entry_set_text(GTK_ENTRY(m_entry[i]), s[i].c_str());
		//updateEntryColor(i);
	}
	m_signals = true;
	inputChanged(false);
}

void MinMaxBox::set(std::string const &s1, std::string const &s2) {
	std::string s[] = { s1, s2 };
	set(s);
}

void MinMaxBox::setSize(int size) {
	m_size = size;
}

double MinMaxBox::toScreen(double v) {
	/* m_min = 0
	 * m_max = m_size
	 *
	 * invert
	 * m_min = m_size
	 * m_max = 0
	 */
	return (m_invert ? m_max - v : v - m_min) / (m_max - m_min) * m_size;
}

double MinMaxBox::fromScreen(int v) {
	auto a = v * (m_max - m_min) / m_size;
	return (m_invert ? m_max - a : a + m_min);
}

void MinMaxBox::scale(double k) {
	auto c = (m_min + m_max) / 2;
	auto l = (m_max - m_min) / 2;
	m_min = c - l * k;
	m_max = c + l * k;
}

void MinMaxBox::updateEntries() {
	int i;
	m_signals = false;
	for (i = 0; i < 2; i++) {
		auto s = forma(i ? m_max : m_min);
		gtk_entry_set_text(GTK_ENTRY(m_entry[i]), s.c_str());
	}
	m_signals = true;
}

void MinMaxBox::inputChanged(bool redraw/*=true*/) {
	if (!m_signals) {
		return;
	}
	int i;
	double v[2];
	std::string s;
	for (i = 0; i < 2; i++) {
		s = gtk_entry_get_text(GTK_ENTRY(m_entry[i]));
		try {
			v[i] = ExpressionEstimator::calculate(s);
			m_ok[i] = true;
		} catch (std::exception &e) {
			m_ok[i] = false;
		}
	}

	if (v[0] >= v[1]) {
		for (i = 0; i < 2; i++) {
			m_ok[i] = 0;
		}

	}

	for (i = 0; i < 2; i++) {
		updateEntryColor(i);
	}

	if (!redraw) {
		if (ok()) {
			set(v[0], v[1]);
		}
		return;
	}

	if (ok()) {
		set(v[0], v[1]);
		if (m_graph) {
			m_graph->recountAnyway();
			pWindow->redraw();
		} else {
			if (m_invert) { //not need to recount for y-axis changes
				pWindow->redraw();
			} else {
				pWindow->axisChanged(false);
			}
		}
	} else {
		if (m_graph) {
			m_graph->m_v.clear();
			pWindow->redraw();
		} else {
			pWindow->redraw();
		}

	}
}

void MinMaxBox::setName(const char *s) {
	gtk_label_set_text(GTK_LABEL(m_name), s);
}

bool MinMaxBox::ok() {
	return m_ok[0] && m_ok[1];
}

void MinMaxBox::updateEntryColor(int i) {
	addRemoveClass(m_entry[i], CERROR, !m_ok[i]); //red font
}

bool MinMaxBox::inEntry(GtkWidget *w) {
	return oneOf(w, m_entry, SIZEI(m_entry));
}

std::string MinMaxBox::toString() {
	std::string s;
	int i = 0;
	for (auto a : m_entry) {
		if (i) {
			s += ' ';
		}
		s += Graph::toSaveString(gtk_entry_get_text(GTK_ENTRY(a)));
		i++;
	}
	return s;
}
