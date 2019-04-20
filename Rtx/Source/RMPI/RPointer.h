#ifndef RPOINTER_H
#define RPOINTER_H

#include <QObject>
#include <QWeakPointer>
#include <QSharedPointer>
#include "RObject.h" //salarifard

template <class T> class RPointer : public QWeakPointer<T>
{

	//	Q_OBJECT -> can't mix Q_OBJECT macro with template!

public:

	RPointer() :QWeakPointer<T>() {}

	RPointer(const RPointer<T> &other) : QWeakPointer<T>(other) {}

	// Checks if the object cannot be cast to the type T, creates an empty pointer
    RPointer(QObject * object) : QWeakPointer<T>(((!object) ? QWeakPointer<T>() : QWeakPointer<T>(qSharedPointerCast<T>(((RObject *)object)->getObjectSharedPointer())))) {
        } //salarifard

	RPointer(QSharedPointer<T> &other) : QWeakPointer<T>(other) {}

	~RPointer() {}

	/*
	// defined to be able to assign QObject * to the pointer
	void operator=(QObject * givenObject);

	void operator=(QSharedPointer<T> &other);
	*/

	template <class T_base> RPointer<T_base> cast_to_super() {
		return RPointer<T_base>(this->data());
	}

	operator bool() {
		//return bool(QWeakPointer::operator bool());
		return !operator !();
	}

	bool operator !() {
        return QWeakPointer<T>::operator !(); //salarifard
	}

	bool operator ==(RPointer<T> other) {
		return (this->data() == other.data());
	}


private:


};

#endif

