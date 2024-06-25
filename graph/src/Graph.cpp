/*
 * Graph.cpp
 *
 *  Created on: 13.04.2022
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#include <cmath>

#include "aslov.h"
#include "Graph.h"
#include "GraphWindow.h"
extern GraphWindow *pWindow;

const char *name[] = { "y(x)=", "r(a)=", "x(t)=", "y(t)=" };
const char *formula[] = { "tan(x)", "3*sin(3*a)", "3*cos(t)", "3*sin(t)" };
const std::string GRAPH_PARAMETERS[] = { "0", "2*pi", "5 * 1000" }; //min,max,step

//cann't use Graph *pGraph; because many graphs
static void button_clicked(GtkWidget *w, Graph* g) {
	g->buttonClicked(w);
}

static void input_changed(GtkWidget *w, Graph* g) {
	g->inputChanged(w);
}

static gboolean combo_changed(GtkComboBox *w, Graph*g) {
	g->changeType(gtk_combo_box_get_active(w));
	return TRUE;
}

Graph::Graph(GraphType type, int colorIndex) {
	int i;
	m_signals = false;
	m_colorIndex = colorIndex % pWindow->m_vcolor.size();
	m_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	for (auto &a : m_entry) {
		a = gtk_entry_new();
		g_signal_connect(a, "changed", G_CALLBACK(input_changed), this);
	}

	m_combo = gtk_combo_box_text_new();
	g_signal_connect(m_combo, "changed", G_CALLBACK(combo_changed), this);

	for (i = 0; i < 2; i++) {
		m_name[i] = gtk_label_new("");
		addClass(m_name[i], format("c%d", m_colorIndex));
		gtk_box_pack_start(GTK_BOX(m_box), m_name[i], FALSE, FALSE, 0);

		gtk_box_pack_start(GTK_BOX(m_box), m_entry[i], 1, 1, 0);
	}
	m_name[i] = gtk_label_new("");

	m_typel = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(m_box), m_typel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(m_box), m_combo, FALSE, FALSE, 0);

	m_minmax.init("", false, false);

	gtk_box_pack_start(GTK_BOX(m_box), m_minmax.m_box, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(m_box), m_name[2], FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(m_box), m_entry[2], 0, 0, 0);

	for(i=0;i<SIZEI(m_button);i++){
		auto b = m_button[i]=gtk_button_new();
		if (!i) {
			gtk_button_set_image(GTK_BUTTON(b), image("minus.png"));
		}
		g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(button_clicked), this);
		gtk_box_pack_start(GTK_BOX(m_box), b, FALSE, FALSE, 0);
	}
	setUpdateButton1(true);

	setDefault(type, false);

	m_minmax.set(GRAPH_PARAMETERS);

	m_signals = false;
	gtk_entry_set_text(GTK_ENTRY(m_entry[2]), GRAPH_PARAMETERS[2].c_str());
	m_signals = true;
	setSteps();
}

void Graph::recount() {
	recount(m_minmax.m_min, m_minmax.m_max, m_steps);
}

void Graph::recount(double min, double max, int steps) {
	if (!m_ok[0] || (!m_ok[1] && m_type == GraphType::PARAMETRICAL)) {
		return;
	}
	if (m_minmax.m_min == min && m_minmax.m_max == max && m_steps == steps) {
		return;
	}
	m_minmax.set(min, max);
	m_steps = steps;

	if (m_minmax.ok()) {
		recountAnyway();
	} else {
		m_v.clear();
		pWindow->redraw();
	}
}

void Graph::recountAnyway() {
	double t, x, y, v;
	if (m_points) {
		return;
	}
	m_v.clear();
	if (m_steps == 0 || !m_minmax.ok()) {
		return;
	}

	//DO NOT REMOVE
	if((m_minmax.m_max - m_minmax.m_min) / m_steps==0){
		printl("error",int(m_type),m_minmax.m_max, m_minmax.m_min, m_steps)
		exit(0);
	}
	for (v = m_minmax.m_min; v <= m_minmax.m_max;
			v += (m_minmax.m_max - m_minmax.m_min) / m_steps) {
		try {
			t = calculate(0, v);
			if (m_type == GraphType::SIMPLE) {
				x = v;
				y = t;
			} else if (m_type == GraphType::POLAR) {
				x = t * cos(v);
				y = t * sin(v);
			} else {
				x = t;
				y = calculate(1, v);
				//printl(y)
			}
			m_v.push_back( { x, y });
		} catch (std::exception &e) {
			printl(e.what())
			;
			break;
		}
	}
}

void Graph::setDefault(GraphType type, bool resetColor/*=false*/,
		bool recount/*=false*/) {
	int i;
	if (resetColor) {
		removeClass(m_name[0], format("c%d", m_colorIndex));
		m_colorIndex = 0;
		addClass(m_name[0], format("c%d", m_colorIndex));
	}

	m_type = type;
	m_points = false;
	int t = int(type);
	std::string vf[] = { formula[t],
			type == GraphType::PARAMETRICAL ? formula[3] : "" };

	m_signals = false;

	for (i = 0; i < (type == GraphType::PARAMETRICAL ? 2 : 1); i++) {
		setFormula(vf[i], i);
		assert(m_ok[i]);
		gtk_label_set_text(GTK_LABEL(m_name[i]), name[i == 1 ? 3 : t]);
	}
	if (type != GraphType::SIMPLE) {
		m_minmax.setName(type == GraphType::PARAMETRICAL ? "t" : "a");
	}

	showHideWidgets();

	updateLanguage();

	m_minmax.m_graph = this;

	m_signals = false;
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_combo), t);
	m_signals = true;

	if (recount) {
		recountAnyway();
	}
}

void Graph::updateLanguage() {
	int i;

	gtk_label_set_text(GTK_LABEL(m_typel), pWindow->getLanguageString(TYPE));
	gtk_label_set_text(GTK_LABEL(m_name[2]), pWindow->getLanguageString(STEPS));

	m_signals = false;
	gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(m_combo));
	for (i = 0; i < 3; i++) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(m_combo),
				pWindow->getLanguageString(STRING_ENUM(STANDARD + i)));
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_combo), int(m_type));
	m_signals = true;
}

void Graph::changeType(int type) {
	if (!m_signals) {
		return;
	}
	setDefault(GraphType(type), false);
	m_minmax.inputChanged(true); //have to change min&max for new graph type if switch from parametrical to standard
	setSteps();
	recountAnyway();
	pWindow->redraw();
}

void Graph::showHideWidgets() {
	auto glambda = [](auto a, bool b) {
		gtk_widget_set_no_show_all(a, !b);
		showHideWidget(a, b);
	};

	glambda(m_name[1], m_type == GraphType::PARAMETRICAL);
	glambda(m_entry[1], m_type == GraphType::PARAMETRICAL);
	glambda(m_name[2], m_type != GraphType::SIMPLE);
	glambda(m_entry[2], m_type != GraphType::SIMPLE);

	gtk_widget_set_no_show_all(m_minmax.m_box, m_type == GraphType::SIMPLE);
	if (m_type == GraphType::SIMPLE) {
		gtk_widget_hide(m_minmax.m_box);
	} else {
		gtk_widget_show_all(m_minmax.m_box); //need show all
	}

}

void Graph::inputChanged(GtkWidget *w) {
	if (!m_signals) {
		return;
	}

	size_t j;
	int i = 0;
	const char *s = gtk_entry_get_text(GTK_ENTRY(w));
	for (auto a : m_entry) {
		if (a == w) {
			if (i == 2) { //steps
				if (setSteps()) {
					recountAnyway();
				}
				pWindow->redraw();
			} else {
				m_v.clear();
				if (m_type == GraphType::SIMPLE && i == 0) {
					VString v = splitr(trim(s), "\\s+");
					std::vector<Point> vp;
					Point p;
					if (v.size() % 2 == 0) {
						for (j = 0; j < v.size(); j += 2) {
							if (!parseString(v[j], p.x)
									|| !parseString(v[j + 1], p.y)) {
								break;
							}
							vp.push_back(p);
						}
						if (j == v.size()) {
							m_v = vp;
							removeClass(m_entry[0], CERROR);
						}
					}
				}
				m_points = !m_v.empty();
				if (!m_points) {
					setFormula(s, i);
					if (m_ok[i]) {
						recountAnyway();
					}
				}
				pWindow->redraw();
			}
			break;
		}
		i++;
	}
}

void Graph::setFormula(std::string s, int i) {
	const std::string a[] = { "x", "a", "t" };
//	printl(s,i,int(m_type))
	try {
		m_formula[i] = s;
		m_estimator[i].compile(m_formula[i], a[int(m_type)]);
		m_ok[i] = true;
	} catch (std::exception &e) {
		m_v.clear();
		m_ok[i] = false;
	}
	gtk_entry_set_text(GTK_ENTRY(m_entry[i]), m_formula[i].c_str());
	addRemoveClass(m_entry[i], CERROR, !m_ok[i]);
}

double Graph::calculate(int i, double v) {
	//getArguments() =1 for "y=1"
	return m_estimator[i].calculate(v);
}

void Graph::updateEnableClose() {
	gtk_widget_set_sensitive(m_button[BUTTON_REMOVE_INDEX],
			pWindow->m_g.size() > 1);
}

bool Graph::setSteps() {
	double v;
	auto w = m_entry[2];
	try {
		v = ExpressionEstimator::calculate(gtk_entry_get_text(GTK_ENTRY(w)));
		if (v > 0 && v == int(v)) {
			m_steps = v;
			removeClass(w, CERROR);
			return true;
		}
	} catch (std::exception &e) {

	}

	m_steps = 0;
	m_v.clear();
	addClass(w, CERROR);
	return false;
}

bool Graph::inEntry(GtkWidget *w) {
	return m_minmax.inEntry(w) || oneOf(w, m_entry, SIZEI(m_entry));
}

std::string Graph::toString() {
	std::string s = format("type=%c%d%c color=%c%d%c", SEPARATOR, int(m_type),
			SEPARATOR, SEPARATOR, m_colorIndex, SEPARATOR);
	int i, j;
	for (j = 0; j <= int(m_type); j++) {
		i = m_type == GraphType::POLAR && j == 1 ? 2 : j;
		s += format(" %s=%c%s%c", i == 2 ? "steps" : "formula", SEPARATOR,
				gtk_entry_get_text(GTK_ENTRY(m_entry[i])), SEPARATOR);
	}
	if (m_type != GraphType::SIMPLE) {
		s += " minmax=" + m_minmax.toString();
	}
	s += format(" show=%c%d%c", SEPARATOR,int(m_show),SEPARATOR);
	return s;
}

void Graph::setStepsMinMax(std::string &steps, std::string &min,
		std::string &max) {
	gtk_entry_set_text(GTK_ENTRY(m_entry[2]), steps.c_str());
	setSteps();
	m_minmax.set(min, max);
}

void Graph::buttonClicked(GtkWidget *w) {
	int i = INDEX_OF(w, m_button);
	if (i == BUTTON_REMOVE_INDEX) {
		pWindow->removeGraph(w);
	} else {
		setUpdateButton1(!m_show);
		pWindow->redraw();
	}
}

void Graph::setUpdateButton1(bool show) {
	m_show=show;
	gtk_button_set_image(GTK_BUTTON(m_button[1]),
			image(IMAGE_BUTTONS[m_show ? IBUTTON_ON : IBUTTON_OFF]));
}
