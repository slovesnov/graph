/*
 * Graph.cpp
 *
 *  Created on: 13.04.2022
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#include "aslov.h"
#include "Graph.h"
#include "GraphWindow.h"
extern GraphWindow *pWindow;

const char *name[] = { "y(x)=", "r(a)=", "x(t)=", "y(t)=" };
const char *formula[] = { "tan(x)", "3*sin(3*a)", "3*cos(t)", "3*sin(t)" };
const double GRAPH_PARAMETERS[] = {0, 100, 5000};//min,max,step

static void button_clicked(GtkWidget *w, gpointer) {
	pWindow->removeGraph(w);
}

static void input_changed(GtkWidget*w, gpointer g) {
	((Graph*)g)->inputChanged(w);
}

static gboolean combo_changed(GtkComboBox *w, gpointer g) {
	((Graph*)g)->changeType(gtk_combo_box_get_active(w));
	return TRUE;
}

Graph::Graph(GraphType type,int colorIndex) {
	int i;
	m_signals=false;
	m_colorIndex = colorIndex % pWindow->m_vcolor.size();
	m_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	for(auto&a:m_entry){
		a=gtk_entry_new();
		g_signal_connect(a, "changed", G_CALLBACK(input_changed), this);
	}

	m_combo=gtk_combo_box_text_new();
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

	m_minmax.init( "",false,false);

	gtk_box_pack_start(GTK_BOX(m_box), m_minmax.m_box, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(m_box), m_name[2], FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(m_box), m_entry[2], 0, 0, 0);

	auto b = m_button = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(b), image("minus.png"));
	g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(button_clicked), 0);
	gtk_box_pack_start(GTK_BOX(m_box), b, FALSE, FALSE, 0);

	setDefault(type,false);
	m_minmax.set(GRAPH_PARAMETERS[0], GRAPH_PARAMETERS[1], true);
	m_steps = GRAPH_PARAMETERS[2];
	gtk_entry_set_text(GTK_ENTRY(m_entry[2]),
			std::to_string(m_steps).c_str());
	removeClass(m_entry[2], CERROR);

}

void Graph::recount() {
	recount(m_minmax.m_min,m_minmax.m_max,m_steps);
}

void Graph::recount(double min, double max, int steps) {
	if(!m_ok[0] || (!m_ok[1] && m_type==GraphType::PARAMETRICAL)){
		return;
	}
	if(m_minmax.m_min==min &&  m_minmax.m_max==max && m_steps==steps ){
		return;
	}
	m_minmax.set(min,max);
	m_steps=steps;

	if(m_minmax.ok()){
		recountAnyway();
	}
	else{
		m_v.clear();
		pWindow->redraw();
	}
}

void Graph::recountAnyway(){
	double t, x, y, v;

	m_v.clear();
	if(m_steps==0 || !m_minmax.ok()){
		return;
	}
	//printl(m_minmax.m_ok[0],m_minmax.m_ok[1])
	//printl(m_minmax.m_min,m_minmax.m_max,m_steps)
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
			}
			m_v.push_back( { x, y });
		} catch (std::exception &e) {
			printl(e.what())
			;
			break;
		}
	}
}

void Graph::setDefault(GraphType type,bool resetColor/*=false*/) {
	int i;
	if(resetColor){
		removeClass(m_name[0], format("c%d", m_colorIndex));
		m_colorIndex=0;
		addClass(m_name[0], format("c%d", m_colorIndex));
	}

	m_type = type;
	int t = int(type);
	std::string vf[] = { formula[t],
			type == GraphType::PARAMETRICAL ? formula[3] : "" };

	m_signals=false;

	for (i = 0; i < (type == GraphType::PARAMETRICAL ? 2 : 1); i++) {
		setFormula(vf[i], i);
		assert(m_ok[i]);
		gtk_entry_set_text(GTK_ENTRY(m_entry[i]), m_formula[i].c_str());
		gtk_label_set_text(GTK_LABEL(m_name[i]), name[i == 1 ? 3 : t]);
	}
	if(type != GraphType::SIMPLE){
		m_minmax.setName(type == GraphType::PARAMETRICAL ? "t" : "a");
	}

	showHideWidgets();

	updateLanguage();

	m_minmax.m_graph=this;

	m_signals=false;
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_combo), t);
	m_signals=true;
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
	setDefault(GraphType(type),false);
	recountAnyway();
	pWindow->redraw();
}

void Graph::showHideWidgets() {
	auto glambda = [](auto a,bool b) {
		gtk_widget_set_no_show_all(a, !b);
		showHideWidget(a, b);
	};

	glambda(m_name[1], m_type == GraphType::PARAMETRICAL);
	glambda(m_entry[1], m_type == GraphType::PARAMETRICAL);
	glambda(m_name[2], m_type != GraphType::SIMPLE);
	glambda(m_entry[2], m_type != GraphType::SIMPLE);

	gtk_widget_set_no_show_all(m_minmax.m_box, m_type == GraphType::SIMPLE);
	if(m_type == GraphType::SIMPLE){
		gtk_widget_hide(m_minmax.m_box);
	}
	else{
		gtk_widget_show_all(m_minmax.m_box);//need show all
	}

}

void Graph::inputChanged(GtkWidget *w) {
	if(!m_signals){
		return;
	}

	int i=0,j;
	const char* s=gtk_entry_get_text(GTK_ENTRY(w));
	for(auto a:m_entry){
		if(a==w){
			if(i==2){//steps
				if(parseString(s, j) && j>0){
					if(m_steps!=j){
						m_steps=j;
						recountAnyway();
						pWindow->redraw();
						removeClass(w, CERROR);
					}
				}
				else{
					m_steps=0;
					m_v.clear();
					pWindow->redraw();
					addClass(w, CERROR);
				}
			}
			else{
				setFormula(s,i);
				if(m_ok[i]){
					recountAnyway();
				}
				pWindow->redraw();
			}
			break;
		}
		i++;
	}
}

void Graph::setFormula(std::string s,int i) {
	const std::string a[] = { "x", "a", "t" };
	try {
		m_formula[i] = s;
		m_estimator[i].compile(m_formula[i], a[int(m_type)]);
		m_ok[i] = true;
	} catch (std::exception &e) {
		m_v.clear();
		m_ok[i] = false;
	}
	addRemoveClass(m_entry[i], CERROR,!m_ok[i]);
}

double Graph::calculate(int i, double v) {
	//getArguments() =1 for "y=1"
	return m_estimator[i].calculate(v);
}

void Graph::updateEnableClose() {
	gtk_widget_set_sensitive (m_button, pWindow->m_g.size()>1);
}
