#include "RNonlinSingleConstrSolver.h"

#include "RDomain.h"
#include "RFunction.h"

RNonlinSingleConstrSolver::RNonlinSingleConstrSolver(QObject *parent, QString name)
	: RObject(parent, name)
{
	theDomain = qobject_cast<RDomain *>(parent);
	theStartPointType = RObject::Mean;
}

RNonlinSingleConstrSolver::~RNonlinSingleConstrSolver()
{

}

RObject::RStartPointType RNonlinSingleConstrSolver::getStartPoint() const
{
	return theStartPointType;
}

void RNonlinSingleConstrSolver::setStartPoint(RObject::RStartPointType value)
{
	theStartPointType = value;
	/*if (theStartPointType == RObject::Origin) {
		theStartPointType = RObject::Mean;
	}*/
}
