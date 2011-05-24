/*
 * IDrive.h
 *
 *  Created on: 17 de Mai de 2011
 *      Author: Sorcha
 */


#pragma once

class IDrive {

public:
	virtual void Init()=0;
	virtual void ShutDown()=0;
	void AcquireLock(){}
	void ReleaseLock(){}
};


