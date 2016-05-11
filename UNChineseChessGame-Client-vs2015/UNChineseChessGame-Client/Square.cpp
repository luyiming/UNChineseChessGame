//#include "stdafx.h"
#include "Square.h"

void Square::moveTo(Square &des) {
	des.empty = false;
	des.valid = true;
	des.color = color;
	des.piece = piece;
	clear();
}

void Square::clear() {
	empty = true;
	valid = true;
	color = -1;
	piece = -1;
}

void Square::reset() {
	empty = false;
	valid = false;
	color = -1;
	piece = -1;
}