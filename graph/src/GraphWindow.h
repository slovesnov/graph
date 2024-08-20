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

enum IBUTTON {
	IBUTTON_PLUS,
	IBUTTON_RESET,
	IBUTTON_LOAD,
	IBUTTON_SAVE,
	IBUTTON_VIEWMAG_PLUS,
	IBUTTON_VIEWMAG_MINUS,
	IBUTTON_FULLSCREEN,
	IBUTTON_TRIANGLE,
	IBUTTON_GRID,
	IBUTTON_HELP,
	IBUTTON_OFF,
	IBUTTON_ON,
	IBUTTON_SIZE
};
const std::string TRIANGLE_UP = "triangleup.png";
const std::string TRIANGLE_DOWN = "triangledown.png";
static std::string IMAGE_BUTTONS[] = { "plus.png", "new.png", "folder.png",
		"save.png", "viewmag+.png", "viewmag-.png", "fullscreen.png",
		TRIANGLE_UP, "grid.png", "help.png", "off.png", "on.png" };
static_assert(SIZEI(IMAGE_BUTTONS)==IBUTTON_SIZE);

static const char *languageString[][64] = { { "plotter", "type", "standard",
		"polar (a is angle)", "parametrical", "steps", "version", "cancel",
		"open", "save", "graph files", "error", "error file is corrupted",
		"grid", "x-axis", "y-axis", "show lines", "step",
		"maximum number of steps", "pixels", "precision" }, {
		"����������� ��������", "���", "�����������", "�������� (a - ����)",
		"���������������", "�����", "������", "������", "�������", "���������",
		"����� ��������", "������", "������ ���� ��������", "�����", "��� x",
		"��� y", "�������� �����", "���", "������������ ����� �����",
		"��������", "���� ����� �������" } };

enum STRING_ENUM {
	PLOTTER,
	TYPE,
	STANDARD,
	POLAR,
	PARAMETRICAL,
	STEPS,
	VERSION,
	CANCEL,
	OPEN,
	SAVE,
	GRAPH_FILES,
	ERROR,
	ERROR_FILE_IS_CORRUPTED,
	GRID,
	X_AXIS,
	Y_AXIS,
	SHOW_LINES,
	STEP,
	MAXIMUM_NUMBER_OF_STEPS,
	PIXELS,
	PRECISION
};

const char DEFAULT_NAME[] = "untitled";
const char DEFAULT_EXTENSION[] = "gr";

const size_t GRID_CHECK_INDEX[] = { 0, 2, 4, 6 };
const int GRID_CHECK_SHOW_X = 0;
const int GRID_CHECK_PIXELS_X = 1;
const int GRID_CHECK_SHOW_Y = 2;
const int GRID_CHECK_PIXELS_Y = 3;
const int GRID_CHECK_SIZE = SIZEI(GRID_CHECK_INDEX);

const int GRID_ENTRY_STEP_X = 0;
const int GRID_ENTRY_DIGITS_X = 1;
const int GRID_ENTRY_STEP_Y = 2;
const int GRID_ENTRY_DIGITS_Y = 3;
const int GRID_ENTRY_MAXSTEPS = 4;
const int GRID_ENTRY_SIZE = 5;

class GraphWindow {
public:
	int m_language;
	GtkWidget *m_window, *m_vb, *m_combo;
	MinMaxBox m_xy[2];
	GtkWidget *m_ibutton[IBUTTON_SIZE];
	/*in *.gr file show_x(bool) step_x pixels_x(bool) digits_x show_y(bool) step_y pixels_y(bool) digits_y maxsteps
	 * m_grid_check_w = show_x pixels_x show_y pixels_y
	 * m_grid_entry = step_x digits_x step_y digits_y maxsteps
	 */
	GtkWidget *m_area, *m_coordinates, *m_grid_check_w[GRID_CHECK_SIZE],
			*m_grid_entry[GRID_ENTRY_SIZE];
	bool m_grid_check[GRID_CHECK_SIZE];
	double m_grid_value[GRID_ENTRY_SIZE];
	static double adjustAxis(double v);
	std::vector<Graph*> m_g;
	int m_dragx, m_dragy, m_dragxe, m_dragye;
	static const int NO_DRAG = -1;
	std::vector<GdkRGBA> m_vcolor;
	bool m_setaxisOnDraw;
	std::string m_path;

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

	//all screen functions arguments should be double NOT int
	double fromScreenX(double v);
	double fromScreenY(double v);
	double toScreenX(double v);
	double toScreenY(double v);
	Point toScreen(double x, double y);
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
	void save();
	void load();
	void load(std::string s); //for debug
	void clearGraphs(bool removeFromContainer = true);
	void addGraph(GraphType type, int colorIndex);
	std::string filechooser(bool save);
	void updateTriangleButton();
	bool isGraphsVisible();
	void updateTitle();
	void setPathUpdateTitle(std::string &s);
	void setDefaultPathUpdateTitle();
	void message(std::string title, std::string text);
	void message(std::string title, GtkWidget *w);
	void showGridDialog();
	void inputChanged(GtkWidget *w);
	void checkChanged(GtkWidget *w);
	void loadGridParameters();
	void storeGridParameters();
	void setDefaultGrid();
	static std::string toSaveString(const gchar *p);
	static std::string toSaveString(int i);
	static std::string toSaveString(double i);

};

#endif /* GRAPHWINDOW_H_ */
