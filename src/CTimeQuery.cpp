#include "CTimeQuery.h"

int TimeQuery::timerCount = 0;

TimeQuery::TimeQuery() {
	TimeQuery::timerCount++;
	this->name = "timeQuery" + std::to_string(TimeQuery::timerCount);
	this->done = 0;
	//genBuffers();
}

TimeQuery::TimeQuery(std::string name) {
	TimeQuery::timerCount++;
	this->name = name;
	this->done = 0;
	//genBuffers();
}

void TimeQuery::start() {
	glGenQueries(1, &query);
	glBeginQuery(GL_TIME_ELAPSED, query);
}

void TimeQuery::stop() {
	glEndQuery(GL_TIME_ELAPSED);
}

/*
Returns elapsed time in ms
*/
float TimeQuery::getElapsedTime() {
	while (!done) {
		glGetQueryObjectiv(query,GL_QUERY_RESULT_AVAILABLE,&done);
	}
	glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);

	return	elapsedTime / 1000000.0;
}

TimeQuery::~TimeQuery() {

}