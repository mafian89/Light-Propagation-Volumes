/*
* File:   CTimeQuery.h
* Desc:		Trida pro mereni casu
*
*/
#ifndef CTIMEQUERY_H
#define CTIMEQUERY_H

#pragma once

#include "common.h"

class TimeQuery {
public:
	TimeQuery();
	TimeQuery(std::string name);
	~TimeQuery();
	void genBuffers();
	static int timerCount;
	void start();
	void stop();
	float getElapsedTime();
	std::string getQueryName() {
		return name;
	}
private:
	std::string name;
	GLuint query;
	GLuint64 elapsedTime;
	GLint done;
};
#endif