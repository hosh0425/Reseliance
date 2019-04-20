#ifndef SSection_H
#define SSection_H

#include "SMaterial.h"
#include "RObject.h"

class SSection : public RObject
{
        Q_OBJECT

public:
        SSection(QObject *parent, QString &name);
        ~SSection();

        virtual double getAValue() = 0;
        virtual double getIyValue() = 0;
        virtual double getIzValue() = 0;
        virtual double getJValue() = 0;
        virtual double getCwValue() = 0;
        virtual double getAvyValue() = 0;
        virtual double getAvzValue() = 0;

        virtual double getEA() = 0;
        virtual double getEIy() = 0;
        virtual double getEIz() = 0;
        virtual double getGJ() = 0;
        virtual double getECw() = 0;

        virtual double getGAvz() = 0;
        virtual double getGAvy() = 0;

        virtual double getSectionWeight() = 0;

private:
};

#endif
