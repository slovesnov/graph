/*
 * Grid.cpp
 *
 *  Created on: 20.08.2024
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#include "Grid.h"
#include "GraphWindow.h"

extern GraphWindow *pWindow;

#define E1(id,s,v) return s=pWindow->getLanguageString(id) +v+"\n"+getFileInfo(__FILE__, FILEINFO::NAME)+":"+std::to_string(__LINE__);
#define E(id,s) E1(id,s,std::string(""));

std::string Grid::set(VString const &t) {
	size_t j;
	int l, m, n;
	std::string s;

	if (t.size() != GRID_CHECK_SIZE + GRID_ENTRY_SIZE) {
		E(INVALID_NUMBER_OF_PARAMETERS, commonError)
	}
	//all checks at first, because parsing of steps depends on checks
	for (m = 0; m < SIZEI(GRID_CHECK_INDEX); m++) {
		j = GRID_CHECK_INDEX[m];
		if (!parseString(t[j], l) || (l != 0 && l != 1)) {
			E(INVALID_CHECK_SHOULD_BE_0_OR_1, checkError[m])
		}
		check[m] = l;
	}
	//all entries
	n = 0;
	for (j = 0; j < t.size(); j++) {
		if ( INDEX_OF(j, GRID_CHECK_INDEX) != -1) {
			continue;
		}
		s = setValue(t[j], n);
		if (!s.empty()) {
			return s;
		}
		n++;
	}
	return "";
}

bool Grid::ok() {
	int i;
	for (i = 0; i < GRID_CHECK_SIZE; i++) {
		if (!checkError[i].empty()) {
//			printl(i)
			return false;
		}
	}
	for (i = 0; i < GRID_ENTRY_SIZE; i++) {
		if (!valueError[i].empty()) {
//			printl(i)
			return false;
		}
	}
//	printl(commonError)
	return commonError.empty();
}

void Grid::reset() {
	int i;
	bool c[] = { 1, 1, 1, 1 };
	commonError = "";
	for (i = 0; i < GRID_CHECK_SIZE; i++) {
		check[i] = c[i];
		checkError[i] = "";
	}
	double v[] = { 150, 2, 150, 2, 30 };
	for (i = 0; i < GRID_ENTRY_SIZE; i++) {
		value[i] = v[i];
		valueError[i] = "";
	}
}

//Note check[] should be set
std::string Grid::setValue(std::string s, int n) {
	double d;
	int l;
	if (oneOf(n, GRID_ENTRY_DIGITS_X, GRID_ENTRY_DIGITS_Y,
			GRID_ENTRY_MAXSTEPS)) {
		if (!parseString(s, l) || l < 0) {
			E(CANNT_PARSE_INTEGER_OR_PARSE_NEGATIVE_VALUE, valueError[n])
		}
		if (n == GRID_ENTRY_MAXSTEPS && l == 0) {
			E(MAXIMUM_STEPS_SHOULD_BE_GREATER_THAN_ZERO, valueError[n])
		}
		d = l;
	} else { //GRID_ENTRY_STEP_X GRID_ENTRY_STEP_Y
		if (check[
				n == GRID_ENTRY_STEP_X ?
						GRID_CHECK_PIXELS_X : GRID_CHECK_PIXELS_Y]) {
			const int q = 100;
			if (!parseString(s, l) || l < q) {
				E1(CANNT_PARSE_INTEGER_OR_PARSE_VALUE, valueError[n],
						(" " + std::to_string(q)))
			}
			d = l;
		} else {
			if (!parseString(s, d) || d <= 0) {
				E(CANNT_PARSE_DOUBLE_OR_PARSE_NEGATIVE_VALUE, valueError[n])
			}
		}
	}
	value[n] = d;
	valueError[n] = "";
	return "";
}

std::string Grid::toString() {
	const char *p[] = { "show_x", "step_x", "pixels_x", "digits_x", "show_y",
			"step_y", "pixels_y", "digits_y", "maxsteps" };
	const double v[] = { double(check[GRID_CHECK_SHOW_X]),
			value[GRID_ENTRY_STEP_X], double(check[GRID_CHECK_PIXELS_X]),
			value[GRID_ENTRY_DIGITS_X], double(check[GRID_CHECK_SHOW_Y]),
			value[GRID_ENTRY_STEP_Y], double(check[GRID_CHECK_PIXELS_Y]),
			value[GRID_ENTRY_DIGITS_Y], value[GRID_ENTRY_MAXSTEPS] };
	std::string s = "\ngrid";
	for (int i = 0; i < SIZEI(p); i++) {
		s += formatz(' ', p[i], "=", SEPARATOR, v[i], SEPARATOR);
	}
	return s;
}

void Grid::toDialog() {
	int i;
	for (i = 0; i < GRID_CHECK_SIZE; i++) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pWindow->m_gridCheck[i]),
				check[i]);
	}
	for (i = 0; i < GRID_ENTRY_SIZE; i++) {
		std::string s = forma(value[i]);
		gtk_entry_set_text(GTK_ENTRY(pWindow->m_gridEntry[i]), s.c_str());
	}
}

void Grid::fromDialog() {
	int i;
	for (i = 0; i < GRID_CHECK_SIZE; i++) {
		check[i] = gtk_toggle_button_get_active(
				GTK_TOGGLE_BUTTON(pWindow->m_gridCheck[i]));
	}
	for (i = 0; i < GRID_ENTRY_SIZE; i++) {
		parseString(gtk_entry_get_text(GTK_ENTRY(pWindow->m_gridEntry[i])),
				value[i]);
	}
}

void Grid::operator =(const Grid &g) {
	int i;
	for (i = 0; i < GRID_CHECK_SIZE; i++) {
		check[i] = g.check[i];
		checkError[i] = g.checkError[i];
	}
	for (i = 0; i < GRID_ENTRY_SIZE; i++) {
		value[i] = g.value[i];
		valueError[i] = g.valueError[i];
	}
	commonError = g.commonError;
}
