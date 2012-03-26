/*
 * Ratio.h
 *
 *  Created on: Mar 20, 2012
 *      Author: db538
 */

#ifndef RATIO_H_
#define RATIO_H_

class Ratio {
private:
	unsigned int mNumerator;
	unsigned int mDenominator;

public:
	Ratio(unsigned int num, unsigned int den);
	virtual ~Ratio();

	unsigned int getNumerator();
	unsigned int getDenominator();
	double getDouble();
};

#endif /* RATIO_H_ */
