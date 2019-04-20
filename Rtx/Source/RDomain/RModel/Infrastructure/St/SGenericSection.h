#ifndef SGenericSection_H
#define SGenericSection_H

#include "SMaterial.h"
#include "SSection.h"
#include "RParameter.h"

class SGenericSection : public SSection
{
        Q_OBJECT;

        Q_PROPERTY(QObject *Material READ getMaterial WRITE setMaterial);
        Q_PROPERTY(QString A READ getA WRITE setA);
        Q_PROPERTY(QString Iy READ getIy WRITE setIy);
        Q_PROPERTY(QString Iz READ getIz WRITE setIz);
        Q_PROPERTY(QString J READ getJ WRITE setJ);
        Q_PROPERTY(QString Cw READ getCw WRITE setCw);
        Q_PROPERTY(QString Avy READ getAvy WRITE setAvy);
        Q_PROPERTY(QString Avz READ getAvz WRITE setAvz);

public:
        SGenericSection(QObject *parent, QString &name);
        ~SGenericSection();

        QObject *getMaterial() const;
        void setMaterial(QObject *value);

        QString getA();
        void setA(QString value);
        double getAValue();

        QString getIy();
        void setIy(QString value);
        double getIyValue();

        QString getIz();
        void setIz(QString value);
        double getIzValue();

        QString getJ();
        void setJ(QString value);
        double getJValue();

        QString getCw();
        void setCw(QString value);
        double getCwValue();

        QString getAvy();
        void setAvy(QString value);
        double getAvyValue();

        QString getAvz();
        void setAvz(QString value);
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

        RPointer<RParameter> theAParameter;
        double theADouble;

        RPointer<RParameter> theIyParameter;
        double theIyDouble;

        RPointer<RParameter> theIzParameter;
        double theIzDouble;

        RPointer<RParameter> theJParameter;
        double theJDouble;

        RPointer<RParameter> theCwParameter;
        double theCwDouble;

        RPointer<RParameter> theAvyParameter;
        double theAvyDouble;

        RPointer<RParameter> theAvzParameter;
        double theAvzDouble;
        };

#endif
