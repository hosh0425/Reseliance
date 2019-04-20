#ifndef SRectangularSection_H
#define SRectangularSection_H

#include "SMaterial.h"
#include "SSection.h"
#include "RParameter.h"

class SRectangularSection : public SSection
{
        Q_OBJECT;

        Q_PROPERTY(QObject *Material READ getMaterial WRITE setMaterial);
        Q_PROPERTY(QString B READ getB WRITE setB);
        Q_PROPERTY(QString H READ getH WRITE setH);
        Q_PROPERTY(double A READ getAValue);
        Q_PROPERTY(double Iy READ getIyValue);
        Q_PROPERTY(double Iz READ getIzValue);
        Q_PROPERTY(double J READ getJValue);
        Q_PROPERTY(double Cw READ getCwValue);
        Q_PROPERTY(double Avy READ getAvyValue);
        Q_PROPERTY(double Avz READ getAvzValue);

public:
        SRectangularSection(QObject *parent, QString &name);
        ~SRectangularSection();

        QObject *getMaterial() const;
        void setMaterial(QObject *value);

        QString getB();
        void setB(QString value);

        QString getH();
        void setH(QString value);

        double getAValue();
        double getIyValue();
        double getIzValue();
        double getJValue();
        double getCwValue();
        double getAvyValue();
        double getAvzValue();



        double getEA();
        double getEIy();
        double getEIz();
        double getGJ();
        double getECw();
        double getGAvz();
        double getGAvy();
        double getSectionWeight();
		bool canBeRunParallelly();


private:
        SMaterial *theMaterial;

        RPointer<RParameter> theBParameter;
        double theBDouble;

        RPointer<RParameter> theHParameter;
        double theHDouble;
        };

#endif
