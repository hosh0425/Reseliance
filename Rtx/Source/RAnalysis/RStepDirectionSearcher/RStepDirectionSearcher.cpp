#include "RStepDirectionSearcher.h"

#include "RDomain.h"

RStepDirectionSearcher::RStepDirectionSearcher(QObject *parent, QString name)
	: RObject(parent, name)
{
	theDomain = qobject_cast<RDomain *>(parent);
}

RStepDirectionSearcher::~RStepDirectionSearcher()
{

}
