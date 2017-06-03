#ifndef Polynomial_hh
#define Polynomial_hh

#include <vector> 
#include <iostream>
#include <cmath> 

using namespace std; 

class Polynomial {

public: 
	// Polynomial(); // empty constructor 
	Polynomial(vector<double> coef); //constructor 

	double evaluate(double x); // evaluate polynomial at x 
	vector<double> evaluate(vector<double> x); // evaluate vector of x's 
	Polynomial derivative(); // compute derivative 

	void print(); 

	vector<double> coef; 
	int p; // polynomial order 
}; 

#endif