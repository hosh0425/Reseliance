#include "RMeritChecker.h"

#include "RDomain.h"

RMeritChecker::RMeritChecker(QObject *parent, QString name)
	: RObject(parent, name)
{
	theDomain = qobject_cast<RDomain *>(parent);
}

RMeritChecker::~RMeritChecker()
{

}
