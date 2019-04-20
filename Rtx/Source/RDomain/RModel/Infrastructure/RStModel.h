#ifndef RStModel_H
#define RStModel_H

#include "RModel.h"

class RParameter;
class RResponse;
class RGLWidget;
class RGLWidgetEngine;

class RStModel : public RModel
{
	Q_OBJECT

	Q_PROPERTY(QString StInputFile READ getStInputFile WRITE setStInputFile)
	Q_PROPERTY(QString Responses READ getResponses WRITE setResponses)

public:
	RStModel(QObject *parent, QString name);
	~RStModel();

	// Define the mandatory read/write methods for all QProperties
	QString getStInputFile() const;
	void setStInputFile(QString value);
	QString getResponses() const;
	void setResponses(QString value);

	QList<RParameter *> getAllParametersList(bool *ok);

	// Define the method where the actual model is coded
	int evaluateModel(bool evaluateDDM, RObject::RParameterType parameterType, int threadID);

	QStringList getActionList();

	bool canBeRunParallelly();

private slots:
	void display();

private:
	RDomain *theStModelDomain;
	// Declare input data members
	QString theStInputFile;
	QList<RPointer<RObject> > theStructuralObjects;
	QStringList theResponses;
	
	// Declare responses
	QList<RResponse *> theResponseList;

	// OpenGL
	QPointer<RGLWidget> theGLWidget;
	QPointer<RGLWidgetEngine> theGLWidgetEngine;

};

#endif // RStModel_H
