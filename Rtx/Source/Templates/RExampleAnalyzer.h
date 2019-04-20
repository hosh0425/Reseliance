// Last modified by Mojtaba on June 30, 2012

#ifndef RExampleAnalyzer_H
#define RExampleAnalyzer_H

#include "RObject.h"
#include "RPointer.h"//salarifard


class RTransformer;
class RFunction;


class RExampleAnalyzer : public RObject
{
	Q_OBJECT

	// Declaring meta-properties

	// Meta-property for a double input
	Q_PROPERTY(double DoubleInput READ getDoubleInput WRITE setDoubleInput)

	// Meta-property for an integer input
	Q_PROPERTY(int IntegerInput READ getIntegerInput WRITE setIntegerInput)

	// Meta-property for a string input
	Q_PROPERTY(QString StringInput READ getStringInput WRITE setStringInput)

	// Meta-property for a Boolean input
	Q_PROPERTY(bool BooleanInput READ getBooleanInput WRITE setBooleanInput)

	// Meta-property for a pointer to another analysis tool, here an RTransfomer object
	Q_PROPERTY(QObject *Transformer READ getTransformer WRITE setTransformer)

public:
	// Constrcutor
	RExampleAnalyzer(QObject *parent, QString name);

	// Destrcutor
	~RExampleAnalyzer();

	// Getter methods for the meta-properties
	double getDoubleInput() const;
	int getIntegerInput() const; 
	QString getStringInput() const; 
	bool getBooleanInput() const;
	QObject *getTransformer() const;	

	// Setter methods for the meta-properties
	void setDoubleInput(double value);
	void setIntegerInput(int value);
	void setStringInput(QString value);
	void setBooleanInput(bool value);
	void setTransformer(QObject *value);

	// Method where the algorithm of the analysis is implemented
	int analyze(RFunction *theFunction);


private:
	// Data members for the meta-properties
	double theDoubleInput;
	int theIntegerInput;
	QString theStringInput;
	bool theBooleanInput;
	RPointer<RTransformer> theTransformer;

};

#endif // RExampleAnalyzer_H
