#include "RStepSizeSearcher.h"

#include "RDomain.h"
#include "RFunction.h"
#include "RRandomVariable.h"

RStepSizeSearcher::RStepSizeSearcher(QObject *parent, QString name)
	: RObject(parent, name)
{
	theDomain = qobject_cast<RDomain *>(parent);
}

RStepSizeSearcher::~RStepSizeSearcher()
{

}
