#pragma once
#include <iostream>
#include <cstdlib>
#include "Field.h"
using namespace std;

class Param
{
protected:
	int srow{0};
	int scol{0};
	int sant{0};
	int trow{0};
	int tcol{0};
	int tant{0};

public:
	Param(int srow, int scol, int sant, int trow, int tcol, int tant)
		:srow{ 0 }, scol{ 0 }, sant{ 0 }, trow{ 0 }, tcol{ 0 }, tant{ 0 } {}

	//setters
	void setSource(int r, int c) {
		srow = r; scol = c;
	}
	void setSant(int ant) {
		sant = ant;
	}
	void setTarget(int r, int c) {
		trow = r; tcol = c;
	}
	void setTant(int ant) {
		tant = ant;
	}
	void Reset() {
		srow = 0; scol = 0; sant = 0; trow = 0; tcol = 0; tant = 0;
	}
	//getters
	int getSrow() const {
		return srow;
	}
	int getScol() const {
		return scol;
	}
	int getSant() const {
		return sant;
	}
	int getTrow() const {
		return trow;
	}
	int getTcol() const {
		return tcol;
	}
	int getTant() const {
		return tant;
	}
};

