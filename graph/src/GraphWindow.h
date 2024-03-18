/*
 * GraphWindow.h
 *
 *  Created on: 13.04.2022
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef GRAPHWINDOW_H_
#define GRAPHWINDOW_H_

#include "aslov.h"
#include "MinMaxBox.h"
#include "Graph.h"

enum IBUTTON{
	IBUTTON_PLUS,
	IBUTTON_VIEWMAG_PLUS,
	IBUTTON_VIEWMAG_MINUS,
	IBUTTON_FULLSCREEN,
	IBUTTON_RESET,
	IBUTTON_HELP,
	IBUTTON_SIZE
};
static std::string IMAGE_BUTTONS[] = { "plus.png", "viewmag+.png",
		"viewmag-.png","fullscreen.png", "", "help.png" };
static_assert(SIZEI(IMAGE_BUTTONS)==IBUTTON_SIZE);

static const char *languageString[][64] = { { "plotter", "reset", "type",
		"standard", "polar (a is angle)", "parametrical", "steps", "version" },
		{ "построитель графиков", "сброс", "тип", "стандартный",
				"пол€рный (a - угол)", "параметрический", "шагов", "верси€" } };

enum STRING_ENUM {
	PLOTTER, RESET, TYPE, STANDARD, POLAR, PARAMETRICAL, STEPS, VERSION
};

class GraphWindow {
public:
	int m_language;
	GtkWidget *m_window, *m_vb, *m_combo;
	MinMaxBox m_xy[2];
	GtkWidget *m_ibutton[IBUTTON_SIZE];
	GtkWidget *m_area, *m_coordinates;
	static double adjustAxis(double v);
	std::vector<Graph*> m_g;
	int m_dragx, m_dragy, m_dragxe, m_dragye;
	static const int NO_DRAG = -1;
	std::vector<GdkRGBA> m_vcolor;
	bool m_setaxisOnDraw;

	GraphWindow();
	virtual ~GraphWindow();
	void changeLanguage(int language);
	void updateLanguage();

	void clickButton(GtkWidget *widget);
	void clickButton(IBUTTON i);
	void draw(cairo_t *cr, int w, int h);

	void mouseButtonDown(GdkEventButton *event);
	void mouseButtonUp(GdkEventButton *event);
	void mouseMove(GdkEventButton *event);
	void mouseLeave(GdkEventCrossing *event);
	gboolean keyPress(GdkEventKey *event);

	double fromScreenX(int v);
	double fromScreenY(int v);
	int toScreenX(double v);
	int toScreenY(double v);
	Point toScreen(double x, double y) {
		return {double(toScreenX(x)),double(toScreenY(y))};
	}
	void axisChanged(bool update = true);
	void createLanguageCombo();

	void loadConfig();

	const gchar* getLanguageString(STRING_ENUM e) {
		const char *p = languageString[m_language][e];
		return g_locale_to_utf8(p, strlen(p), NULL, NULL, NULL);
	}

	void removeGraph(GtkWidget *w);
	void redraw();
	void updateEnableClose();
};

#endif /* GRAPHWINDOW_H_ */
