#include "RGenericResponse.h"

RGenericResponse::RGenericResponse(QObject *parent, QString name)
	: RResponse(parent, name)
{

}

RGenericResponse::~RGenericResponse()
{

}

bool RGenericResponse::canBeRunParallelly(){
	return true;
}