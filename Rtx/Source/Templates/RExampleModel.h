// Last modified by Mojtaba on June 30, 2012

#ifndef RExampleModel_H
#define RExampleModel_H

#include "RModel.h"

class RExampleModel : public RModel
{
	Q_OBJECT

	// Declaring meta-properties

	// Meta-property for a single input parameter
	Q_PROPERTY(QObject *ParameterInput READ getParameterInput WRITE setParameterInput)

	// Meta-property for unlimited number of input parameters
	Q_PROPERTY(QString ParameterList READ getParameterList WRITE setParameterList)

public:
	// Constructor method
	RExampleModel(QObject *parent, QString name);

	// Destrcutor method
	~RExampleModel();

	// Method where the model algorithm and DDM sensitivites is implemented
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType);

	// Getter methods for the meta-properties
	QObject *getParameterInput() const;
	QString getParameterList();

	// Setter methods for the meta-properties
	void setParameterList(QString value);
	void setParameterInput(QObject *value);

private:
	// Data members for the meta-properties
	RPointer<RParameter> theParameterInput;
	QList<RPointer<RParameter> > theParameterList;

	// Data member for model response
	RResponse *theResponse;

};

#endif // RExampleModel_H
