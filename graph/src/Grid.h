/*
 * Grid.h
 *
 *  Created on: 20.08.2024
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef GRID_H_
#define GRID_H_

#include "aslov.h"

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

class Grid {
public:
	bool check[GRID_CHECK_SIZE];
	double value[GRID_ENTRY_SIZE];
	std::string checkError[GRID_CHECK_SIZE], valueError[GRID_ENTRY_SIZE],
			commonError;
	std::string set(VString const &t);
	bool ok();
	void reset();
	std::string setValue(std::string s, int n);
	std::string toString();
	void toDialog();
	void fromDialog();
	void operator=(Grid const &g);
};

#endif /* GRID_H_ */
