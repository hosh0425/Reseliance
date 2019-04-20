#include "SRectangularSection.h"
#include "SMaterial.h"
#include "SSection.h"


SRectangularSection::SRectangularSection(QObject *parent, QString &name)
                : SSection(parent, name)
{
        theMaterial=0;

        //$$ theBParameter = 0;
        theBDouble = 0.0;

        //$$ theHParameter = 0;
        theHDouble = 0.0;
}

SRectangularSection::~SRectangularSection()
{

}

QObject * SRectangularSection::getMaterial() const
{
        return theMaterial;
}
void SRectangularSection::setMaterial(QObject *value)
{
        theMaterial = qobject_cast<SMaterial *>(value);
}






QString SRectangularSection::getB()
{
        return getStringParameter(&theBDouble, theBParameter.data());
}
void SRectangularSection::setB(QString value)
{
        setStringParameter(value, &theBDouble, &theBParameter);
}






QString SRectangularSection::getH()
{
        return getStringParameter(&theHDouble, theHParameter.data());
}
void SRectangularSection::setH(QString value)
{
        setStringParameter(value, &theHDouble, &theHParameter);
}









double SRectangularSection::getAValue()
{
        double H = getDoubleParameter(&theHDouble, theHParameter.data());
        double B = getDoubleParameter(&theBDouble, theBParameter.data());
        return B*H;
}



double SRectangularSection::getIyValue()
{
        double H = getDoubleParameter(&theHDouble, theHParameter.data());
        double B = getDoubleParameter(&theBDouble, theBParameter.data());
        return B*H*H*H/12.0;
}


double SRectangularSection::getIzValue()
{
        double H = getDoubleParameter(&theHDouble, theHParameter.data());
        double B = getDoubleParameter(&theBDouble, theBParameter.data());
        return H*B*B*B/12.0;
}


double SRectangularSection::getJValue()
{
        double H = getDoubleParameter(&theHDouble, theHParameter.data());
        double B = getDoubleParameter(&theBDouble, theBParameter.data());
        return H*B*B*B*(0.333-0.21*B/H*(1.0-(B*B*B*B/(12.0*H*H*H*H))));
}

double SRectangularSection::getCwValue()
{
        // To be added
        return 0.0;
}


double SRectangularSection::getAvzValue()
{
        double A = this->getAValue();
        return (5.0*A/6.0);
}

double SRectangularSection::getAvyValue()
{
        double A = this->getAValue();
        return (5.0*A/6.0);
}







double SRectangularSection::getEA()
{
        double E = theMaterial->getYoungsModulusValue();
        double A = this->getAValue();
        return (E*A);
}



double SRectangularSection::getEIy()
{
        double E = theMaterial->getYoungsModulusValue();
        double Iy = this->getIyValue();
        return (E*Iy);
}


double SRectangularSection::getEIz()
{
        double E = theMaterial->getYoungsModulusValue();
        double Iz = this->getIzValue();
        return (E*Iz);
}


double SRectangularSection::getGJ()
{
        double E = theMaterial->getYoungsModulusValue();
        double nu = theMaterial->getPoissonsRatioValue();
        double G = E/(2*(1+nu));
        double J = this->getJValue();
        return (G*J);
}

double SRectangularSection::getECw()
{
        double E = theMaterial->getYoungsModulusValue();
        double Cw = this->getCwValue();
        return (E*Cw);
}


double SRectangularSection::getGAvz()
{
        double E = theMaterial->getYoungsModulusValue();
        double nu = theMaterial->getPoissonsRatioValue();
        double G = E/(2*(1+nu));
        double Avz = this->getAvzValue();
        return (G*Avz);
}

double SRectangularSection::getGAvy()
{
        double E = theMaterial->getYoungsModulusValue();
        double nu = theMaterial->getPoissonsRatioValue();
        double G = E/(2*(1+nu));
        double Avy = this->getAvyValue();
        return (G*Avy);
}


double SRectangularSection::getSectionWeight()
{
        double weightDensity = theMaterial->getWeightDensityValue();
        double H = getDoubleParameter(&theHDouble, theHParameter.data());
        double B = getDoubleParameter(&theBDouble, theBParameter.data());
        return (H*B*weightDensity);
}


bool SRectangularSection::canBeRunParallelly() {
	return false;
}










