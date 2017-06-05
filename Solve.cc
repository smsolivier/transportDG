#include "Solve.hh"

#include "helper.hh"

#include <iostream>
#include <fstream>
#include <cmath>

#include "printVector.hh"

using namespace std; 

Solve::Solve(Mesh mesh, int Nt, double tb, 
	double a, double b, double c, 
	double q, double f0, double alpha) : mesh(mesh), Nt(Nt), tb(tb), 
		a(a), b(b), c(c), q(q), f0(f0), alpha(alpha) 
{

	dt = tb/Nt; 

	t = linspace(0, tb, Nt); 

	fcount = 0; 

	cout << "a = " << a << endl; 
	cout << "b = " << b << endl; 
	cout << "c = " << c << endl; 
	cout << "f0 = " << f0 << endl; 
	cout << "alpha  = " << alpha << endl; 

}

double Solve::qf(double x, double t) {

	return M_PI*cos(M_PI*x) + sin(M_PI*x);  

}

void Solve::genLocal(Element &el, double upwind, double upwind_prev, 
		double t, double t_prev) {

	for (int i=0; i<el.p; i++) {

		for (int j=0; j<el.p; j++) {

			double x = el.xglob[j]; // current x global location of node 

			// time 
			el.A[i][j] = a/dt*el.M[i][j]; 

			// space part 
			el.A[i][j] += alpha*(-b*el.S[i][j] + c*el.M[i][j]); 

			el.rhs[i] += el.f[j] * a/dt*el.M[i][j]; 

			el.rhs[i] -= el.f[j] * (1 - alpha)*(
				-b*el.S[i][j] + c*el.M[i][j]); 

			el.rhs[i] += qf(x, t)*el.M[i][j]; 
			// el.rhs[i] += q*el.M[i][j]; 

		}
	}

	// apply upwinding 
	el.A.back().back() += alpha*b; 
	el.rhs.back() -= (1 - alpha) * el.f.back() * b; 

	// left boundary 
	el.rhs[0] += b*(alpha*upwind + (1-alpha)*upwind_prev); 


}

void Solve::solveSpace(double t, double t_prev) {

	vector<double> xout(mesh.N);
	vector<double> fout(mesh.N); 

	for (int i=0; i<mesh.N; i++) {

		Element &el = mesh.el[i]; 

		// generate local system 
		if (i > 0) {

			Element &el_down = mesh.el[i-1]; 

			genLocal(el, el_down.evaluate(-1), el_down.f_prev.back(), 
				t, t_prev); 

		}

		else {

			genLocal(el, f0, f0, t, t_prev); 

		}

		double x_i, f_i; 

		el.solve(x_i, f_i);

		xout[i] = x_i; 
		fout[i] = f_i;  

	}

	writeCurve(xout, fout); 

}

void Solve::solveTime() {

	for (int i=1; i<Nt; i++) {

		solveSpace(t[i], t[i-1]); 

	}
}

void Solve::writeCurve(vector<double> &x, vector<double> &f) {

	ofstream file; 

	file.open("data/" + to_string(fcount) + ".curve"); 

	for (int i=0; i<x.size(); i++) {

		file << x[i] << " " << f[i] << endl; 
	}

	file.close(); 

	fcount += 1; 
}

double q(double x, double t) {

	return M_PI*cos(M_PI*x) + sin(M_PI*x); 
}

int main() {

	int Nx = 40; 

	double xb = 1; 

	int p = 2; 

	Mesh mesh(Nx, xb, p); 

	int Nt = 50;  
	double tb = 1; 
	double a = 0; 
	double b = 1; 
	double c = 1; 
	double q = 0; 
	double alpha = 1; 

	double f0 = 0; 

	// mesh.el[0].A[0][0] = 1; 

	// printVector(mesh.el[0].A); 

	Solve sol(mesh, Nt, tb, a, b, c, q, f0, alpha); 

	sol.solveTime(); 

}