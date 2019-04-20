#include "SAssembler.h"
#include "RDomain.h"


SAssembler::SAssembler(QObject *parent, QString &name)
                : RObject(parent, name)
{
        theDomain = qobject_cast<RDomain *>(parent);
}


SAssembler::~SAssembler()
{
}

void SAssembler::setDomain(RDomain *passedDomain)
{
	theDomain = passedDomain;
}
