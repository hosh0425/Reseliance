#include "SGenericSection.h"
#include "SMaterial.h"
#include "SSection.h"


SGenericSection::SGenericSection(QObject *parent, QString &name)
                : SSection(parent, name)
{
        theMaterial=0;
        //$$ theAParameter = 0;
        theADouble = 0.0;

        //$$ theIyParameter = 0;
        theIyDouble = 0.0;

        //$$ theIzParameter = 0;
        theIzDouble = 0.0;

        //$$ theJParameter = 0;
        theJDouble = 0.0;

        //$$ theCwParameter = 0;
        theCwDouble = 0.0;

        //$$ theAvyParameter = 0;
        theAvyDouble = 0.0;

        //$$ theAvzParameter = 0;
        theAvzDouble = 0.0;
}

SGenericSection::~SGenericSection()
{

}

QObject * SGenericSection::getMaterial() const
{
        return theMaterial;
}
void SGenericSection::setMaterial(QObject *value)
{
        theMaterial = qobject_cast<SMaterial *>(value);
}




QString SGenericSection::getA()
{
        return getStringParameter(&theADouble, theAParameter.data());
}
void SGenericSection::setA(QString value)
{
        setStringParameter(value, &theADouble, &theAParameter);
}
double SGenericSection::getAValue()
{
        return getDoubleParameter(&theADouble, theAParameter.data());
}






QString SGenericSection::getIy()
{
        return getStringParameter(&theIyDouble, theIyParameter.data());
}
void SGenericSection::setIy(QString value)
{
        setStringParameter(value, &theIyDouble, &theIyParameter);
}
double SGenericSection::getIyValue()
{
        return getDoubleParameter(&theIyDouble, theIyParameter.data());
}






QString SGenericSection::getIz()
{
        return getStringParameter(&theIzDouble, theIzParameter.data());
}
void SGenericSection::setIz(QString value)
{
        setStringParameter(value, &theIzDouble, &theIzParameter);
}
double SGenericSection::getIzValue()
{
        return getDoubleParameter(&theIzDouble, theIzParameter.data());
}






QString SGenericSection::getJ()
{
        return getStringParameter(&theJDouble, theJParameter.data());
}
void SGenericSection::setJ(QString value)
{
        setStringParameter(value, &theJDouble, &theJParameter);
}
double SGenericSection::getJValue()
{
        return getDoubleParameter(&theJDouble, theJParameter.data());
}






QString SGenericSection::getCw()
{
        return getStringParameter(&theCwDouble, theCwParameter.data());
}
void SGenericSection::setCw(QString value)
{
        setStringParameter(value, &theCwDouble, &theCwParameter);
}
double SGenericSection::getCwValue()
{
        return getDoubleParameter(&theCwDouble, theCwParameter.data());
}





QString SGenericSection::getAvy()
{
        return getStringParameter(&theAvyDouble, theAvyParameter.data());
}
void SGenericSection::setAvy(QString value)
{
        setStringParameter(value, &theAvyDouble, &theAvyParameter);
}
double SGenericSection::getAvyValue()
{
        return getDoubleParameter(&theAvyDouble, theAvyParameter.data());
}





QString SGenericSection::getAvz()
{
        return getStringParameter(&theAvzDouble, theAvzParameter.data());
}
void SGenericSection::setAvz(QString value)
{
        setStringParameter(value, &theAvzDouble, &theAvzParameter);
}
double SGenericSection::getAvzValue()
{
        return getDoubleParameter(&theAvzDouble, theAvzParameter.data());
}








double SGenericSection::getEA()
{
        double E = theMaterial->getYoungsModulusValue();
        double A = getDoubleParameter(&theADouble, theAParameter.data());
        return (E*A);
}



double SGenericSection::getEIy()
{
        double E = theMaterial->getYoungsModulusValue();
        double Iy = getDoubleParameter(&theIyDouble, theIyParameter.data());
        return (E*Iy);
}


double SGenericSection::getEIz()
{
        double E = theMaterial->getYoungsModulusValue();
        double Iz = getDoubleParameter(&theIzDouble, theIzParameter.data());
        return (E*Iz);
}


double SGenericSection::getGJ()
{
        double E = theMaterial->getYoungsModulusValue();
        double nu = theMaterial->getPoissonsRatioValue();
        double G = E/(2*(1+nu));
        double J = getDoubleParameter(&theJDouble, theJParameter.data());
        return (G*J);
}

double SGenericSection::getECw()
{
        double E = theMaterial->getYoungsModulusValue();
        double Cw = getDoubleParameter(&theCwDouble, theCwParameter.data());
        return (E*Cw);
}


double SGenericSection::getGAvz()
{
        double E = theMaterial->getYoungsModulusValue();
        double nu = theMaterial->getPoissonsRatioValue();
        double G = E/(2*(1+nu));
        double Avz = this->getAvzValue();
        return (G*Avz);
}

double SGenericSection::getGAvy()
{
        double E = theMaterial->getYoungsModulusValue();
        double nu = theMaterial->getPoissonsRatioValue();
        double G = E/(2*(1+nu));
        double Avy = this->getAvyValue();
        return (G*Avy);
}

double SGenericSection::getSectionWeight()
{
        double weightDensity = theMaterial->getWeightDensityValue();
        double A = getDoubleParameter(&theADouble, theAParameter.data());
        return (A*weightDensity);
}

bool SGenericSection::canBeRunParallelly() {
	return false;
}










