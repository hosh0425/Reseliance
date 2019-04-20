#ifndef RAccumulationThread_H
#define RAccumulationThread_H

#include <QThread>

class RAsynchronousAccumulator;

class RAccumulationThread : public QThread{

public:

	RAccumulationThread(RAsynchronousAccumulator * theAccumulator);

	~RAccumulationThread();

	void run();

private:

	RAsynchronousAccumulator * theAccumulator;

};

#endif