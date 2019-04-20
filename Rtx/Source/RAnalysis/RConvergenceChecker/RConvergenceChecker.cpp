#include "RConvergenceChecker.h"

#include "RDomain.h"

RConvergenceChecker::RConvergenceChecker(QObject *parent, QString name)
	: RObject(parent, name)
{
	theDomain = qobject_cast<RDomain *>(parent);
}

RConvergenceChecker::~RConvergenceChecker()
{

}
