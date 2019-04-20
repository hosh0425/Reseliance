#include "RConstant.h"

RConstant::RConstant(QObject *parent, QString name)
	: RParameter(parent, name)
{

}

RConstant::~RConstant()
{

}

void RConstant::setUpstreamParameterList(QList<RParameter *> parameterList)
{
	theUpstreamParameterList = parameterList;
}

QList<RParameter *> RConstant::getUpstreamParameterList()
{
	return theUpstreamParameterList;
}

bool RConstant::canBeRunParallelly(){
	return true;
}