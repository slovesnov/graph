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

#include "MinMaxBox.h"
#include "Graph.h"
#include "Grid.h"

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

static const char *languageString[][64] =
		{ { "plotter", "type", "standard", "polar (a is angle)", "parametrical",
				"steps", "version", "ok", "reset", "cancel", "open", "save",
				"graph files", "error", "error file is corrupted", "grid",
				"x-axis", "y-axis", "show lines", "step",
				"maximum number of steps", "pixels", "precision",
				"invalid number of parameters",
				"invalid check should be 0 or 1",
				"cann't parse double or parse negative value",
				"cann't parse integer or parse negative value",
				"maximum steps should be greater than zero",
				"cann't parse integer or value less than" },
				{ "построитель графиков", "тип", "стандартный",
						"полярный (a - угол)", "параметрический", "шагов",
						"версия", "ок", "сброс", "отмена", "открыть",
						"сохранить", "файлы графиков", "ошибка",
						"ошибка файл повреждён", "сетка", "ось x", "ось y",
						"показать линии", "шаг", "максимальное число шагов",
						"пикселей", "цифр после запятой",
						"неверное число параметров",
						"неверный чек должно быть 0 или 1",
						"невозможно распознать число с плавающей точкой или распознано негативное число",
						"невозможно распознать целое число или распознано негативное число",
						"максимальное число шагов должно быть больше нуля",
						"невозможно распознать целое число или значение меньше чем" } };

enum STRING_ENUM {
	PLOTTER,
	TYPE,
	STANDARD,
	POLAR,
	PARAMETRICAL,
	STEPS,
	VERSION,
	OK,
	RESET,
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
	PRECISION,
	INVALID_NUMBER_OF_PARAMETERS,
	INVALID_CHECK_SHOULD_BE_0_OR_1,
	CANNT_PARSE_DOUBLE_OR_PARSE_NEGATIVE_VALUE,
	CANNT_PARSE_INTEGER_OR_PARSE_NEGATIVE_VALUE,
	MAXIMUM_STEPS_SHOULD_BE_GREATER_THAN_ZERO,
	CANNT_PARSE_INTEGER_OR_PARSE_VALUE,
};

const char DEFAULT_NAME[] = "untitled";
const char DEFAULT_EXTENSION[] = "gr";

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
	GtkWidget *m_area, *m_coordinates, *m_gridCheck[GRID_CHECK_SIZE],
			*m_gridEntry[GRID_ENTRY_SIZE], *m_modal, *m_modalButton[3],
			*m_modalLabel;
	static double adjustAxis(double v);
	std::vector<Graph*> m_g;
	int m_dragx, m_dragy, m_dragxe, m_dragye;
	static const int NO_DRAG = -1;
	std::vector<GdkRGBA> m_vcolor;
	bool m_setaxisOnDraw;
	std::string m_path;
	Grid m_grid, m_gridStart;

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
	void load(std::string s);
	void clearGraphs(bool removeFromContainer = true);
	void addGraph(GraphType type, int colorIndex);
	std::string filechooser(bool save);
	void updateTriangleButton();
	bool isGraphsVisible();
	void updateTitle();
	void setPathUpdateTitle(std::string &s);
	void setDefaultPathUpdateTitle();
	void showGridDialog();
	void inputChanged(GtkWidget *w);
	void checkChanged(GtkWidget *w);
	static std::string toSaveString(const gchar *p);
	static std::string toSaveString(int i);
	static std::string toSaveString(double i);
	gint showModalDialog(std::string title, std::string text);
	gint showModalDialog(std::string title, GtkWidget *w, bool simple = true);
	void gridDialogButtonClicked(STRING_ENUM e);
};

#endif /* GRAPHWINDOW_H_ */
