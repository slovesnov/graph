/*
 * GraphWindow.cpp
 *
 *  Created on: 13.04.2022
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#include <cmath>

#include "GraphWindow.h"
#include <regex>

GraphWindow *pWindow;

const std::string CONFIG_TAGS[] = { "version", "language" };
const std::string URL = "http://slovesnov.users.sourceforge.net?graph";
const int startGraphs = 1;

enum {
	PIXBUF_COL, TEXT_COL
};

const std::string LNG[] = { "en", "ru" };

std::string removeEndingZero(std::string s) {
	/*
	 std::string a[]={"1.2300","45.00","800","340."};
	 1.2300 1.23
	 45.00 45
	 800 800
	 340. 340
	 */
	std::regex re("(\\.[1-9]*)(0+)$");
	std::string r;
	r = std::regex_replace(s, re, "$1");
	size_t i = r.length() - 1;
	if (!r.empty() && r[i] == '.') {
		r = r.substr(0, i);
	}
	return r;
}

std::string removeEndingZerosMinusZero(std::string s) {
	std::string r=removeEndingZero(s);
	return r=="-0"?"0":r;
}

static void grid_gialog_button_clicked(GtkWidget *widget, STRING_ENUM e) {
	pWindow->gridDialogButtonClicked(e);
}

static void button_clicked(GtkWidget *widget, gpointer) {
	pWindow->clickButton(widget);
}

static void input_changed(GtkWidget *w) {
	pWindow->inputChanged(w);
}

static void check_changed(GtkWidget *w) {
	pWindow->checkChanged(w);
}

static gboolean mouse_press_event(GtkWidget *widget, GdkEventButton *event,
		gpointer) {
	//event->button =1 left mouse button
	//event->button =2 middle mouse button
	//event->button =3 right mouse button
	pWindow->mouseButtonDown(event);
	return TRUE;
}

static gboolean mouse_release_event(GtkWidget *widget, GdkEventButton *event,
		gpointer) {
	pWindow->mouseButtonUp(event);
	return TRUE;
}

static gboolean mouse_move_event(GtkWidget *widget, GdkEventButton *event,
		gpointer) {
	pWindow->mouseMove(event);
	return TRUE;
}

//Mouse out signal. Note second parameter GdkEventCrossing not GdkEventButton
static gboolean mouse_leave_event(GtkWidget *widget, GdkEventCrossing *event,
		gpointer) {
	pWindow->mouseLeave(event);
	return TRUE;
}

static gboolean key_press(GtkWidget *widget, GdkEventKey *event,
		gpointer data) {
	//println("%x %x %s",event->keyval,event->hardware_keycode,event->string)
	return pWindow->keyPress(event);
}

static gboolean combo_changed(GtkComboBox *comboBox, gpointer) {
	pWindow->changeLanguage(gtk_combo_box_get_active(comboBox));
	return TRUE;
}

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
//	printl("draw",gtk_widget_get_allocated_width(widget),gtk_widget_get_allocated_height(widget));
	pWindow->draw(cr, gtk_widget_get_allocated_width(widget),
			gtk_widget_get_allocated_height(widget));
	return FALSE;
}

GraphWindow::GraphWindow() {
	int i, j;
	std::size_t p;
	const std::string cl = ".c";
	GtkWidget *hb, *b;

	pWindow = this;
	loadConfig();
	m_setaxisOnDraw = true;

	loadCSS();
	//load colors
	auto s = fileGetContent(getApplicationName() + ".css");
	auto v = split(s, "\n");
	j = 0;
	for (auto &a : v) {
		if ((p = a.find(cl)) != std::string::npos) {
			//note stoi "0{" throws exception
			i = atoi(a.substr(p + cl.length()).c_str());
			if (i > j) {
				j = i;
			}
		}
	}
	j++;
	for (i = 0; i < j; i++) {
		auto b = gtk_label_new("");
		addClass(b, "c" + std::to_string(i));
		GtkStyleContext *c = gtk_widget_get_style_context(b);
		GdkRGBA color;
		gtk_style_context_get_color(c, GTK_STATE_FLAG_NORMAL, &color);
		m_vcolor.push_back(color);
	}

	for (i = 0; i < startGraphs; i++) {
		m_g.push_back(new Graph(GraphType(i), i));
	}
	if (startGraphs > 1) {
		m_g[1]->recount();
		if (startGraphs > 2) {
			m_g[2]->recount();
		}
	}
	updateEnableClose();

	m_dragx = NO_DRAG;

	m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(m_window), GTK_WIN_POS_CENTER);
	hb = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);

	i = 0;
	for (auto &b : m_ibutton) {
		b = m_ibutton[i] = gtk_button_new();
		gtk_button_set_image(GTK_BUTTON(b), image(IMAGE_BUTTONS[i]));
		g_signal_connect(G_OBJECT(b), "clicked", G_CALLBACK(button_clicked), 0);
		if (i < int(IBUTTON_HELP)) {
			gtk_box_pack_start(GTK_BOX(hb), b, FALSE, FALSE, 0);
		}
		i++;
	}

	i = 0;
	for (auto &a : m_xy) {
		a.init(i ? "y" : "x", i);
		gtk_box_pack_start(GTK_BOX(hb), a.m_box, FALSE, FALSE, 0);
		i++;
	}

	b = m_coordinates = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(hb), b, 1, 1, 0);

	createLanguageCombo();
	gtk_box_pack_start(GTK_BOX(hb), m_combo, FALSE, FALSE, 0);

	for (i = IBUTTON_HELP; i < IBUTTON_SIZE; i++) {
		gtk_box_pack_start(GTK_BOX(hb), m_ibutton[i], FALSE, FALSE, 0);
	}

	m_area = gtk_drawing_area_new();
	g_signal_connect(G_OBJECT (m_area), "draw", G_CALLBACK (draw_callback),
			NULL);

	gtk_widget_add_events(m_area,
			GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK
					| GDK_POINTER_MOTION_MASK | GDK_LEAVE_NOTIFY_MASK);
	g_signal_connect(m_area, "button_press_event",
			G_CALLBACK(mouse_press_event), NULL);
	g_signal_connect(m_area, "button-release-event",
			G_CALLBACK(mouse_release_event), NULL);
	g_signal_connect(m_area, "motion-notify-event",
			G_CALLBACK(mouse_move_event), NULL);
	g_signal_connect(m_area, "leave-notify-event",
			G_CALLBACK(mouse_leave_event), NULL);
	g_signal_connect(m_window, "key-press-event", G_CALLBACK (key_press), NULL);

	gtk_widget_set_vexpand(m_area, 1);

	m_vb = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	gtk_container_add(GTK_CONTAINER(m_vb), hb);

	for (auto &a : m_g) {
		gtk_container_add(GTK_CONTAINER(m_vb), a->m_box);
	}

	gtk_container_add(GTK_CONTAINER(m_vb), m_area);

	gtk_container_add(GTK_CONTAINER(m_window), m_vb);

	updateLanguage();

	gtk_window_maximize(GTK_WINDOW(m_window));

	gtk_widget_show_all(m_window);

	//set dot as decimal separator, standard locale, for output
	setlocale(LC_NUMERIC, "C");

	g_signal_connect_swapped(G_OBJECT(m_window), "destroy",
			G_CALLBACK(gtk_main_quit), G_OBJECT(m_window));

	setDefaultPathUpdateTitle();
	m_grid.reset();
	//load("C:\\Users\\user\\git\\graph\\graph\\mass0.gr");
	gtk_main();
}

GraphWindow::~GraphWindow() {
	clearGraphs(false);
	WRITE_CONFIG(CONFIG_TAGS, ExpressionEstimator::version, m_language);
}

void GraphWindow::changeLanguage(int language) {
	m_language = language;
	updateLanguage();
}

void GraphWindow::clickButton(GtkWidget *widget) {
	clickButton(IBUTTON(INDEX_OF(widget, m_ibutton)));
}

void GraphWindow::clickButton(IBUTTON n) {
	int i;
	bool b;
	double k;
	std::vector<std::pair<int, int>> p;
	GdkWindow *gdk_window;
	GdkWindowState state;
	switch (n) {
	case IBUTTON_PLUS:
		//color which appear minimal times, or minimal index if several colors appear same times
		for (i = 0; i < int(m_vcolor.size()); i++) {
			p.push_back( { i, 0 });
		}
		for (auto &a : m_g) {
			p[a->m_colorIndex].second++;
		}
		i = std::min_element(p.begin(), p.end(),
				[](auto &a, auto &b) {
					return a.second < b.second
							|| (a.first < b.first && a.second == b.second);
				})->first;
		addGraph(GraphType::SIMPLE, i);
		gtk_widget_show_all(m_vb);
		updateEnableClose();
		break;

	case IBUTTON_RESET:
		clearGraphs();
		m_setaxisOnDraw = true;
		addGraph(GraphType::SIMPLE, 0);
		gtk_widget_show_all(m_vb);
		updateEnableClose();
		updateTriangleButton();
		setDefaultPathUpdateTitle();
		m_grid.reset();
		break;

	case IBUTTON_LOAD:
		load();
		break;

	case IBUTTON_SAVE:
		save();
		break;

	case IBUTTON_VIEWMAG_PLUS:
	case IBUTTON_VIEWMAG_MINUS:
		k = n == IBUTTON_VIEWMAG_PLUS ? .5 : 2;
		for (auto &a : m_xy) {
			a.scale(k);
		}
		axisChanged();
		break;

	case IBUTTON_FULLSCREEN:
		gdk_window = gtk_widget_get_window(m_window);
		state = gdk_window_get_state(gdk_window);
		if (state & GDK_WINDOW_STATE_FULLSCREEN) {
			gtk_window_unfullscreen(GTK_WINDOW(m_window));
		} else {
			gtk_window_fullscreen(GTK_WINDOW(m_window));
		}
		break;

	case IBUTTON_TRIANGLE:
		b = isGraphsVisible();
		for (auto &a : m_g) {
			if (b) {
				gtk_widget_hide(a->m_box);
			} else {
				gtk_widget_show(a->m_box);
			}
		}
		updateTriangleButton();
		break;

	case IBUTTON_GRID:
		showGridDialog();
		break;

	case IBUTTON_ON:
	case IBUTTON_OFF:
		for (auto a : m_g) {
			a->setUpdateButton1(n == IBUTTON_ON);
		}
		redraw();
		break;

	case IBUTTON_HELP:
		openURL(URL);
		break;

	default:
		break;
	}
}

//round to half for right line width
double GraphWindow::adjustAxis(double v) {
	int a = round(2 * v);
	return a / 2 + .5;
}

void GraphWindow::draw(cairo_t *cr, int w, int h) {
	double x, y;
	std::string s;

	if (m_setaxisOnDraw) {
		auto k = MAXY * gtk_widget_get_allocated_width(m_area)
				/ gtk_widget_get_allocated_height(m_area);
		m_xy[0].set(-k, k, true);	//scale 1:1 circle is circle
		m_xy[1].set(MINY, MAXY, true);
		m_setaxisOnDraw = false;
		axisChanged();
		return;
	}

	m_xy[0].setSize(w);
	m_xy[1].setSize(h);

	if (!m_xy[0].ok() || !m_xy[1].ok()) {
		return;
	}

	//also uses below
	const Point p = toScreen(0, 0);
	x = adjustAxis(p.x);
	y = adjustAxis(p.y);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, .5);

	cairo_move_to(cr, 0, y);
	cairo_line_to(cr, w, y);

	cairo_move_to(cr, x, 0);
	cairo_line_to(cr, x, h);
	cairo_stroke(cr);

	int i, j;
	double v, x1, y1, step;

	const double dashed[] = { 1, 10 };
	const int fontSize = 13;
	const bool xvisible = p.y >= 0 && p.y < h;
	const bool yvisible = p.x >= 0 && p.x < w;
	const bool bothVisible = xvisible && yvisible;
	const bool hasGrid = m_grid.check[GRID_CHECK_SHOW_X]
			|| m_grid.check[GRID_CHECK_SHOW_Y];

	if (hasGrid) {
		cairo_set_line_width(cr, 1);
		cairo_set_dash(cr, dashed, SIZEI(dashed), 0);
		cairo_set_font_size(cr, fontSize);
	}

	if (m_grid.check[GRID_CHECK_SHOW_X]) {
		const int digits = int(m_grid.value[GRID_ENTRY_DIGITS_X]);
		if (m_grid.check[GRID_CHECK_PIXELS_X]) {
			const int delta = m_grid.value[GRID_ENTRY_STEP_X];
			//v = fmod(v,delta)+trunc(v,delta)*delta
			v = yvisible ? p.x : w / 2;
			j = trunc(v / delta);
			for (x = fmod(v, delta), i = 0; x < w; x += delta, i++) {
				if (i != j || !bothVisible) {
					s = removeEndingZerosMinusZero(
							format("%.*lf", digits, fromScreenX(x)));
					cairo_move_to(cr, x, xvisible ? p.y : fontSize);
					cairo_show_text(cr, s.c_str());
				}

				v = adjustAxis(x);
				cairo_move_to(cr, v, 0);
				cairo_line_to(cr, v, h);
			}
		} else {
			//ceil for round x%100=0 800 900 1000...
			step = m_grid.value[GRID_ENTRY_STEP_X];
			for (x1 = ceil(fromScreenX(0) / step) * step, i = 0;
					i < m_grid.value[GRID_ENTRY_MAXSTEPS]
							&& (x = toScreenX(x1)) < w; x1 += step, i++) {
				s = removeEndingZerosMinusZero(format("%.*lf", digits, x1));
				cairo_move_to(cr, x, xvisible ? p.y : fontSize);
				cairo_show_text(cr, s.c_str());

				v = adjustAxis(x);
				cairo_move_to(cr, v, 0);
				cairo_line_to(cr, v, h);
			}
		}

	}
	if (m_grid.check[GRID_CHECK_SHOW_Y]) {
		const int digits = int(m_grid.value[GRID_ENTRY_DIGITS_Y]);
		if (m_grid.check[GRID_CHECK_PIXELS_Y]) {
			const int delta = m_grid.value[GRID_ENTRY_STEP_Y];
			v = xvisible ? p.y : h / 2;
			j = trunc(v / delta);
			for (y = fmod(v, delta), i = 0; y < h; y += delta, i++) {
				if (i != j || !bothVisible) {
					s = removeEndingZerosMinusZero(
							format("%.*lf", digits, fromScreenY(y)));
					cairo_move_to(cr, yvisible ? p.x : 0, y);
					cairo_show_text(cr, s.c_str());
				}

				v = adjustAxis(y);
				cairo_move_to(cr, 0, v);
				cairo_line_to(cr, w, v);
			}
		} else {
			step = m_grid.value[GRID_ENTRY_STEP_Y];
			for (y1 = ceil(fromScreenY(h) / step) * step, i = 0;
					i < m_grid.value[GRID_ENTRY_MAXSTEPS]
							&& (y = toScreenY(y1)) > 0; y1 += step, i++) {
				s = removeEndingZerosMinusZero(format("%.*lf", digits, y1));
				cairo_move_to(cr, yvisible ? p.x : 0, y);
				cairo_show_text(cr, s.c_str());

				v = adjustAxis(y);
				cairo_move_to(cr, 0, v);
				cairo_line_to(cr, w, v);
			}
		}
	}
	if (hasGrid) {
		cairo_stroke(cr);
	}

	cairo_set_line_width(cr, 1.5);

//	cairo_matrix_t save_matrix;
//	cairo_get_matrix(cr, &save_matrix);
//	cairo_translate(cr, w / 2., h / 2.);
//	cairo_scale(cr, double(w) / h, 1);
//	cairo_arc(cr, 0, 0, h / 2., 0, 2 * M_PI);
//	cairo_set_matrix(cr, &save_matrix);
//	cairo_arc(cr, w / 2., h / 2., w / 2., 0, 2 * M_PI);
//	cairo_arc(cr, w / 2., h / 2., h / 2., 0, 2 * M_PI);

	for (auto &b : m_g) {
		auto &a = *b;
		if (!a.m_show) {
			continue;
		}
		if (a.m_type == GraphType::SIMPLE) {
			a.recount(fromScreenX(0), fromScreenX(w), w);
		}
		gdk_cairo_set_source_rgba(cr,
				&m_vcolor[a.m_colorIndex % m_vcolor.size()]);

		for (auto &p : a.m_v) {
			if (std::isnan(p.y)) {
				continue;
			}
			x = toScreenX(p.x);
			y = toScreenY(p.y);
			if (a.m_points) {
				cairo_arc(cr, x, y, 5, 0, 2 * G_PI);
				cairo_close_path(cr);
				cairo_stroke_preserve(cr);
				cairo_fill(cr);
			} else {
				//fixed 31jan24 without condition need to stroke after every cairo_line_to
				if (x >= 0 && y >= 0 && x <= w && y <= h) {
					cairo_move_to(cr, x, y);
					cairo_line_to(cr, x + 1, y + 1);
				}
			}
		}
		cairo_stroke(cr);
	}

	double ar[][4]={
			{64.9,-0.22879718119572995,0,0.12879718119572142},
			{64.8,0.1575943623914344,0.38639154358716427,-0.2439859059786015},
			{65.1,0.19841973162995835,-0.3455661743486403,0.04714644271868764},
			{65,-0.35127328891125936,-0.20412684619257737,0.2554001351038396},
			{64.7,0.006673424015104734,0.5620735591189413,-0.2687469831340489},
			{65,0.3245795928508409,-0.24416739028320536,0.019587797432358795},
			{65.1,-0.10499179541849343,-0.18540399798612897,-0.009604206595374773},
			{64.8,-0.5046124111768756,-0.2142166177722533,0.31882902894913745},
			{64.4,0.023441440126029972,0.742270469075159,-0.5657119092012004},
			{64.6,-0.1891533493272533,-0.9548652585284421,0.5440186078557011},
			{64,-0.4668280428170344,0.677190565038661,-0.010362522221623852},
			{64.2,0.856465520595416,0.6461029983737895,-0.7025685189692084},
			{65,0.040965960435369264,-1.4616025585338357,0.8206365980984721},
			{64.4,-0.420329362336886,1.0003072357615808,-0.3799778734247061},
			{64.6,0.44035148891215703,-0.13962638451253773,-0.10072510439961642},
			{64.8,-0.14107659331176775,-0.441801697711387,0.18287829102316325},
			{64.4,-0.476045115665052,0.10683317535810273,-0.030788059693056462},
			{64,-0.35474294402800877,0.01446899627893335,-0.059726052250923194},
			{63.6,-0.5049831082229116,-0.1647091604738362,0.3696922686967435},
			{63.3,0.2746753769196463,0.9443676456163944,-0.7190430225360407},
			{63.8,0.006281600544312704,-1.2127614219917278,0.7064798214474152},
			{63.3,-0.29980177909689776,0.9066780423505174,-0.20687626325361388},
			{63.7,0.8929255158432954,0.28604925258967573,-0.3789747684329739},
			{64.5,0.328099715723725,-0.850875052709246,0.32277533698551814},
			{64.3,-0.4053243787382126,0.11745095824730842,0.08787342049090134},
			{64.1,0.09319779922910819,0.3810712197200124,-0.27426901894911776},
			{64.3,0.03253318182177978,-0.4417358371273408,0.10920265530556388},
			{64,-0.5233305265162103,-0.11412787121064921,0.43745839772685663},
			{63.8,0.5607889242430611,1.1982473219699206,-0.6590362462129732},
			{64.9,0.9801748295439824,-0.778861416668999,0.09868658712501366},
			{65.2,-0.28148824241897463,-0.482801655293958,0.7642898977129327},
			{65.2,1.0457781401319068,1.81006803784484,-1.5558461779767496},
			{66.5,-0.0016243181086637026,-2.857470496085409,1.5590948141940755},
			{65.2,-1.0392808676972556,1.8198139464968173,-0.8805330787995702},
			{65.1,-0.041252211102332015,-0.8217852899018934,0.5630375010042282},
			{64.8,0.004289712106565785,0.8673272131107913,-0.6716169252173542},
			{65,-0.27590663732391424,-1.1475235625412714,1.2234301998651829},
			{64.8,1.099336837189092,2.522767037054277,-2.122103874243369},
			{66.3,-0.2214407114324597,-3.8435445856758297,2.0649852971082896},
			{64.3,-1.7135739914592498,2.3514113056490387,-0.9378373141897862},
			{64,0.17573667726946943,-0.4621006369203198,0.2863639596508503},
			{64,0.1106272823813809,0.39699124203223124,-0.3076185244136093},
			{64.2,-0.01824580679498461,-0.5258643312085967,0.04411013800358125},
			{63.7,-0.9376440552014342,-0.3935339171978529,0.1311779723992843},
	};
	//printl(SIZEI(ar),m_g[0]->m_points,m_g[0]->m_v[0].x,m_g[0]->m_v[0].y)
	Graph&g=*m_g[0];
	if(g.m_points){
		cairo_set_dash(cr, 0, 0, 0);
		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_set_line_width(cr, .5);
		j=-1;
		for(auto p:g.m_v){
			j++;
			auto pn=g.m_v[j+1].x;
			int l=toScreenX(pn)-toScreenX(p.x);
			for(i=0;i<l;i++){
				x=i*(pn-p.x)/l;
				auto a=ar[j];
				y=a[3];
				for(int k=2;k>=0;k--){
					y=y*x+a[k];
				}
				x=toScreenX(p.x)+i;
				y=toScreenY(y);
				if (x >= 0 && y >= 0 && x <= w && y <= h) {
					if(i==0){
						cairo_move_to(cr, x, y);
					}
					else{
						cairo_line_to(cr, x, y);
					}
				}
			}
			cairo_stroke(cr);
			if(j==SIZEI(ar)-1){
				break;
			}
		}
	}

	if (m_dragx != NO_DRAG) {
		cairo_set_line_width(cr, .5);
		cairo_set_source_rgb(cr, 0, 0, 0);

		const double dashed[] = { 14.0, 6.0 };
		cairo_set_dash(cr, dashed, SIZEI(dashed), 0);

		cairo_rectangle(cr, m_dragx + .5, m_dragy + .5, m_dragxe - m_dragx + .5,
				m_dragye - m_dragy + .5);
		cairo_stroke(cr);
	}
}

void GraphWindow::mouseButtonDown(GdkEventButton *event) {
	if (event->button == 1) {
		//8sep24 fixed bug with mouse move over window and then go to other window and click on window, need also set m_dragxe, m_dragye
		m_dragx = m_dragxe = event->x;
		m_dragy =m_dragye = event->y;
	} else if (event->button == 3) {
		clickButton(IBUTTON_VIEWMAG_MINUS);
	}
}

void GraphWindow::mouseButtonUp(GdkEventButton *event) {
	double v1, v2;

	if (m_dragx != NO_DRAG) {
//click and release same point skip
		if (event->x == m_dragx || event->y == m_dragy) {
			m_dragx = NO_DRAG;
			return;
		}

		v1 = fromScreenX(m_dragx);
		v2 = fromScreenX(m_dragxe);
		m_xy[0].set(std::min(v1, v2), std::max(v1, v2));

		v1 = fromScreenY(m_dragy);
		v2 = fromScreenY(m_dragye);
		m_xy[1].set(std::min(v1, v2), std::max(v1, v2));

		m_dragx = NO_DRAG;

		axisChanged();
	}
}

void GraphWindow::mouseMove(GdkEventButton *event) {
	if (m_dragx != NO_DRAG) {
		m_dragxe = event->x;
		m_dragye = event->y;
		gtk_widget_queue_draw(m_area);
	}
	auto s = "x=" + forma(fromScreenX(event->x)) + " y="
			+ forma(fromScreenY(event->y));
	gtk_label_set_text(GTK_LABEL(m_coordinates), s.c_str());
}

void GraphWindow::mouseLeave(GdkEventCrossing *event) {
	gtk_label_set_text(GTK_LABEL(m_coordinates), "");
}

double GraphWindow::fromScreenX(double v) {
	return m_xy[0].fromScreen(v);
}

double GraphWindow::fromScreenY(double v) {
	return m_xy[1].fromScreen(v);
}

double GraphWindow::toScreenX(double v) {
	return m_xy[0].toScreen(v);
}

double GraphWindow::toScreenY(double v) {
	return m_xy[1].toScreen(v);
}

Point GraphWindow::toScreen(double x, double y) {
	return {toScreenX(x),toScreenY(y)};
}

void GraphWindow::axisChanged(bool update/*=true*/) {
	if (update) {
		for (auto &a : m_xy) {
			a.updateEntries();
		}
	}
	auto &b = m_xy[0];
	for (auto &a : m_g) {
		if (a->m_type == GraphType::SIMPLE) {
			a->recount(b.m_min, b.m_max, b.m_size);
		}
	}
	gtk_widget_queue_draw(m_area);
}

void GraphWindow::createLanguageCombo() {
	GdkPixbuf *pb;
	GtkTreeIter iter;
	GtkTreeStore *store;
	guint i;

	store = gtk_tree_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	for (i = 0; i < SIZE(LNG); i++) {
		pb = pixbuf((LNG[i] + ".gif").c_str());
		gtk_tree_store_append(store, &iter, NULL);
		gtk_tree_store_set(store, &iter, PIXBUF_COL, pb, TEXT_COL,
				(" " + LNG[i]).c_str(), -1);
		g_object_unref(pb);
	}

	m_combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_combo), m_language);
	auto renderer = gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(m_combo), renderer, FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(m_combo), renderer, "pixbuf",
			PIXBUF_COL, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(m_combo), renderer, FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(m_combo), renderer, "text",
			TEXT_COL, NULL);

	g_signal_connect(m_combo, "changed", G_CALLBACK(combo_changed), 0);
}

void GraphWindow::updateLanguage() {
	updateTitle();
	for (auto &a : m_g) {
		a->updateLanguage();
	}
}

void GraphWindow::loadConfig() {
	m_language = 0;
	int j;
	MapStringString m;
	MapStringString::iterator it;
	if (::loadConfig(m)) {
		if ((it = m.find("language")) != m.end()) {
			if (parseString(it->second, j) && j >= 0 && j < SIZEI(LNG)) {
				m_language = j;
			}
		}
	}
}

void GraphWindow::removeGraph(GtkWidget *w) {
	for (auto it = m_g.begin(); it != m_g.end(); it++) {
		if ((*it)->m_button[BUTTON_REMOVE_INDEX] == w) {
			Graph *a = *it;
			m_g.erase(it);
			gtk_container_remove(GTK_CONTAINER(m_vb), a->m_box);
			gtk_widget_show_all(m_vb);
			delete a;
			updateEnableClose();
			return;
		}
	}
}

void GraphWindow::redraw() {
	gtk_widget_queue_draw(m_area);
}

void GraphWindow::updateEnableClose() {
	for (auto &e : m_g) {
		e->updateEnableClose();
	}
}

gboolean GraphWindow::keyPress(GdkEventKey *event) {
//	from gtk documentation return value
//	TRUE to stop other handlers from being invoked for the event. FALSE to propagate the event further.
//cann't use 'f' key to switch fullscreen if focused in one of the entries because "floor()" function has f char
	GtkWidget *w = gtk_window_get_focus(GTK_WINDOW(m_window));
	bool b = false;
	for (auto &e : m_xy) {
		if (e.inEntry(w)) {
			b = true;
			break;
		}
	}
	if (!b) {
		for (auto e : m_g) {
			if (e->inEntry(w)) {
				b = true;
				break;
			}
		}
	}

	const int k = event->keyval;
	const guint16 hwkey = event->hardware_keycode;
	b = (!b && hwkey == GDK_KEY_F) || oneOf(k, GDK_KEY_F11, GDK_KEY_Escape);
	if (b) {
		clickButton(IBUTTON_FULLSCREEN);
	}
	return b;
}

void GraphWindow::save() {
	std::string s = filechooser(true);
	if (s.empty()) {
		return;
	}
	setPathUpdateTitle(s);
	std::ofstream f(s, std::ios::binary);	//binary no \r
	s = forma(ExpressionEstimator::version);
	int i = 0;
	for (auto a : m_xy) {
		s += format("\nminmax_%c=", "xy"[i]) + a.toString();
		i++;
	}
	s += m_grid.toString();

	for (auto a : m_g) {
		s += "\n" + a->toString();
	}
	f << s;
}

void GraphWindow::load() {
	std::string s = filechooser(false);
	if (s.empty()) {
		return;
	}
	//printl(s)
	load(s);
}

void GraphWindow::load(std::string s) {
	setPathUpdateTitle(s);
	std::ifstream f(s);
	std::stringstream buffer;
	buffer << f.rdbuf();
	s = trim(buffer.str());
	VString v = split(s, "\n"), t;
	size_t i, j, k;
	clearGraphs();
	int error = 0;
	if (v.size() < 3) {
		error = __LINE__;
	} else {
		for (i = 1; i < v.size(); i++) {
			t = split(v[i], SEPARATOR);
			k = 0;
			for (j = 1; j < t.size(); j += 2) {
				t[k++] = t[j];
			}
			t.resize(k);
			if (i > 0 && i < 3) {
				if (t.size() != 2) {
					error = __LINE__;
					break;
				}
				m_xy[i - 1].set(t[0], t[1]);
				m_xy[i - 1].inputChanged();
			} else if (i == 3) {
				s = m_grid.set(t);
				if (!s.empty()) {
					error = 1;
					break;
				}
			} else {
				if (t.size() < 2) {
					error = __LINE__;
					break;
				}
				int p[] = { 0, 0 };
				for (j = 0; j < 2; j++) {
					if (!parseString(t[j], p[j])) {
						error = __LINE__;
						break;
					}
				}
				if (j < 2) {
					break;
				}
				const size_t sz[] = { 4, 7, 8 };
				if (p[0] < 0 || p[0] > 2 || t.size() != sz[p[0]]) {
					error = __LINE__;
					break;
				}
				addGraph(GraphType(p[0]), p[1]);
				Graph *e = m_g.back();
				e->setFormula(t[j++], 0);
				if (e->m_type != GraphType::SIMPLE) {
					if (e->m_type == GraphType::PARAMETRICAL) {
						e->setFormula(t[j++], 1);
					}
					e->setStepsMinMax(t[j], t[j + 1], t[j + 2]);
					j += 3;
				}
				e->setUpdateButton1(t[j] == "1");
				e->recountAnyway();
			}
		}
	}
	if (error) {
		showModalDialog(getLanguageString(ERROR),
				getLanguageString(ERROR_FILE_IS_CORRUPTED) + std::string("\n")
						+ (error == 1 ?
								s :
								getFileInfo(__FILE__, FILEINFO::NAME) + ":"
										+ std::to_string(error)));
		if (m_g.empty()) {
			addGraph(GraphType::SIMPLE, 0);
		}
	}
	gtk_widget_show_all(m_vb);
	updateEnableClose();
	updateTriangleButton();
	redraw();
}

void GraphWindow::clearGraphs(bool removeFromContainer/*=true*/) {
	if (removeFromContainer) {
		for (Graph *a : m_g) {
			gtk_container_remove(GTK_CONTAINER(m_vb), a->m_box);
		}
	}
	for (auto &a : m_g) {
		delete a;
	}
	m_g.clear();
}

void GraphWindow::addGraph(GraphType type, int colorIndex) {
	auto i = m_g.size();
	m_g.push_back(new Graph(type, colorIndex));
	gtk_container_add(GTK_CONTAINER(m_vb), m_g[i]->m_box);
	gtk_box_reorder_child(GTK_BOX(m_vb), m_g[i]->m_box, i + 1);
}

std::string GraphWindow::filechooser(bool save) {
	std::string s;
	auto l = save ? SAVE : OPEN;
	GtkWidget *dialog = gtk_file_chooser_dialog_new(getLanguageString(l),
			GTK_WINDOW(m_window),
			save ? GTK_FILE_CHOOSER_ACTION_SAVE : GTK_FILE_CHOOSER_ACTION_OPEN,
			getLanguageString(CANCEL), GTK_RESPONSE_CANCEL,
			getLanguageString(l), GTK_RESPONSE_ACCEPT,
			NULL);

	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),
			getFileInfo(m_path, FILEINFO::DIRECTORY).c_str());
	if (save) {
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),
				getFileInfo(m_path, FILEINFO::NAME).c_str());
	}
	auto gtkFilter = gtk_file_filter_new();
	gtk_file_filter_set_name(gtkFilter,
			format("%s (*.%s)", getLanguageString(GRAPH_FILES),
					DEFAULT_EXTENSION).c_str());
	gtk_file_filter_add_pattern(gtkFilter,
			format("*.%s", DEFAULT_EXTENSION).c_str());
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), gtkFilter);
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), gtkFilter);

	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	auto v = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	if (response == GTK_RESPONSE_ACCEPT) {
		s = v;
	}

	gtk_widget_destroy(dialog);

	return s;
}

void GraphWindow::updateTriangleButton() {
	gtk_button_set_image(GTK_BUTTON(m_ibutton[IBUTTON_TRIANGLE]),
			image(isGraphsVisible() ? TRIANGLE_UP : TRIANGLE_DOWN));
}

bool GraphWindow::isGraphsVisible() {
	return gtk_widget_get_visible(m_g[0]->m_box);
}

void GraphWindow::updateTitle() {
	std::string s = getFileInfo(m_path, FILEINFO::NAME) + " - "
			+ getLanguageString(PLOTTER) + " (" + getLanguageString(VERSION)
			+ " " + forma(ExpressionEstimator::version) + ")";
	gtk_window_set_title(GTK_WINDOW(m_window), s.c_str());
}

void GraphWindow::setPathUpdateTitle(std::string &s) {
	m_path = s;
	updateTitle();
}

void GraphWindow::setDefaultPathUpdateTitle() {
	std::string s = DEFAULT_NAME + std::string(".") + DEFAULT_EXTENSION;
	setPathUpdateTitle(s);
}

void GraphWindow::showGridDialog() {
	int i;
	GtkWidget *b, *b1, *e, *f, *bh, *c;
	m_gridStart = m_grid;

	bh = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	for (i = 0; i < 2; i++) {
		b = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
		m_gridCheck[2 * i] = c = gtk_check_button_new_with_label(
				getLanguageString(SHOW_LINES));
		gtk_container_add(GTK_CONTAINER(b), c);

		b1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
		m_gridEntry[2 * i] = e = gtk_entry_new();
		gtk_container_add(GTK_CONTAINER(b1),
				gtk_label_new(getLanguageString(STEP)));
		gtk_container_add(GTK_CONTAINER(b1), e);
		m_gridCheck[2 * i + 1] = c = gtk_check_button_new_with_label(
				getLanguageString(PIXELS));
		gtk_container_add(GTK_CONTAINER(b1), c);
		gtk_container_add(GTK_CONTAINER(b), b1);

		b1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
		m_gridEntry[2 * i + 1] = e = gtk_entry_new();
		gtk_container_add(GTK_CONTAINER(b1),
				gtk_label_new(getLanguageString(PRECISION)));
		gtk_container_add(GTK_CONTAINER(b1), e);
		gtk_container_add(GTK_CONTAINER(b), b1);

		f = gtk_frame_new(getLanguageString(i ? Y_AXIS : X_AXIS));
		gtk_frame_set_label_align(GTK_FRAME(f), 0.06, 0.5);
		gtk_container_add(GTK_CONTAINER(f), b);

		gtk_container_add(GTK_CONTAINER(bh), f);
	}

	b1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	m_gridEntry[GRID_ENTRY_SIZE - 1] = e = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(b1),
			gtk_label_new(getLanguageString(MAXIMUM_NUMBER_OF_STEPS)), 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(b1), e, 1, 1, 0);

	b = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add(GTK_CONTAINER(b), bh);
	gtk_container_add(GTK_CONTAINER(b), b1);
	gtk_container_add(GTK_CONTAINER(b), m_modalLabel = gtk_label_new(""));

	m_grid.toDialog();
	//after params are set
	for (auto e : m_gridCheck) {
		g_signal_connect(e, "toggled", G_CALLBACK(check_changed), 0);
	}
	for (auto e : m_gridEntry) {
		g_signal_connect(e, "changed", G_CALLBACK(input_changed), GP(-1));
	}

	auto r = showModalDialog(getLanguageString(GRID), b, false);
	if (r != GTK_RESPONSE_OK) {
		//GTK_RESPONSE_CANCEL or GTK_RESPONSE_DELETE_EVENT
		m_grid = m_gridStart;
	}
}

void GraphWindow::inputChanged(GtkWidget *w) {
	const char *t = gtk_entry_get_text(GTK_ENTRY(w));
	int i = INDEX_OF(w, m_gridEntry);
	auto s = replaceAll(m_grid.setValue(t, i), "\n", " ");
	gtk_label_set_text(GTK_LABEL(m_modalLabel), s.c_str());
	addRemoveClass(w, CERROR, !s.empty()); //red font
	gtk_widget_set_sensitive(m_modalButton[0], m_grid.ok());
}

void GraphWindow::checkChanged(GtkWidget *w) {
	int i = INDEX_OF(w, m_gridCheck);
	if (i == GRID_CHECK_PIXELS_X || i == GRID_CHECK_PIXELS_Y) {
		m_grid.check[i] = gtk_toggle_button_get_active(
				GTK_TOGGLE_BUTTON(pWindow->m_gridCheck[i]));
		inputChanged(
				m_gridEntry[
						i == GRID_CHECK_PIXELS_X ?
								GRID_ENTRY_STEP_X : GRID_ENTRY_STEP_Y]);
	}
}

std::string GraphWindow::toSaveString(const gchar *p) {
	return SEPARATOR + std::string(p) + SEPARATOR;
}

std::string GraphWindow::toSaveString(int i) {
	return SEPARATOR + std::to_string(i) + SEPARATOR;
}

std::string GraphWindow::toSaveString(double i) {
	return SEPARATOR + removeEndingZerosMinusZero(std::to_string(i)) + SEPARATOR;
}

gint GraphWindow::showModalDialog(std::string title, std::string text) {
	return showModalDialog(title, gtk_label_new(text.c_str()));
}

gint GraphWindow::showModalDialog(std::string title, GtkWidget *w,
		bool simple) {
	GtkWidget *b, *b1, *b2;
	int i;
	auto d = m_modal = gtk_dialog_new();
	gtk_window_set_modal(GTK_WINDOW(d), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(d), GTK_WINDOW(m_window));

	gtk_window_set_title(GTK_WINDOW(d), title.c_str());
	gtk_window_set_resizable(GTK_WINDOW(d), 0);

	b = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add(GTK_CONTAINER(b), w);
	b1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	std::vector<STRING_ENUM> v;
	if (simple) {
		v = { STRING_ENUM::OK };
	} else {
		v = { STRING_ENUM::OK, STRING_ENUM::RESET, STRING_ENUM::CANCEL };
	}
	i = 0;
	for (auto e : v) {
		b2 = m_modalButton[i++] = gtk_button_new_with_label(
				getLanguageString(e));
		addClass(b2, "sbutton");
		g_signal_connect(b2, "clicked", G_CALLBACK(grid_gialog_button_clicked),
				GP(e));
		gtk_container_add(GTK_CONTAINER(b1), b2);
	}

	gtk_container_add(GTK_CONTAINER(b), b1);
	auto ca = gtk_dialog_get_content_area(GTK_DIALOG(d));
	gtk_container_add(GTK_CONTAINER(ca), b);

	gtk_widget_show_all(d);
	auto r = gtk_dialog_run(GTK_DIALOG(d));
	gtk_widget_destroy(d);
	return r;
}

void GraphWindow::gridDialogButtonClicked(STRING_ENUM e) {
	//printl(int(e))
	if (e == STRING_ENUM::RESET) {
		m_grid.reset();
		m_grid.toDialog();
		gtk_widget_set_sensitive(m_modalButton[0], 1);
	} else {
		if (e == STRING_ENUM::OK) {
			m_grid.fromDialog();
			redraw();
		}
		gtk_dialog_response(GTK_DIALOG(m_modal),
				e == STRING_ENUM::OK ? GTK_RESPONSE_OK : GTK_RESPONSE_CANCEL);
	}
}
