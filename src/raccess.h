/*
 MIT License

 Copyright (c) 2016 Tsukasa Fukunaga
 Copyright (c) 2021 Iñaki Amatria-Barral, Jorge González-Domínguez, Juan Touriño

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/

#ifndef RACCESS_H
#define RACCESS_H

#include <vector>
#include <fstream>
#include <iostream>

#include <math.h>
#include <stdlib.h>

#include "intloops.h"
#include "energy_par.h"

using namespace std;

class Raccess {
 public:
  Raccess(string db_name, int w, int delta, string path) {
    _maximal_span = w;
    _min_accessible_length = delta;
    _path = path;

    if(db_name.size() == 0){
      cerr << "Error: -o option is required." << endl;
      exit(1);
    }

    if(delta <= 1){
      cerr << "Error: -d option must be greater than 1." << endl;
      exit(1);
    }
    _seq_length = 0;

    set_energy_parameters();
  }
  Raccess(int w, int delta) {
    _maximal_span = w;
    _min_accessible_length = delta;
    set_energy_parameters();
  }

  void Run(string &sequence, int idx);
  void Run(string &sequence, vector<float> &accessibility, vector<float> &conditional_accessibility);
 private:
  double hairpin[31];
  double mismatchH[7][5][5];
  double mismatchI[7][5][5];
  double stack[7][7];
  double bulge[31];
  double TermAU;
  double int11[8][8][5][5];
  double int21[8][8][5][5][5];
  double int22[8][8][5][5][5][5];
  double internal[31];
  double MLclosing;
  double MLintern;
  double MLbase;
  double dangle5[8][5];
  double dangle3[8][5];
  double ninio[MAXLOOP+1];

  vector<int> _int_sequence;
  int _seq_length;
  int _maximal_span;
  int _min_accessible_length;
  string _db_name;
  string _path;

  vector<double> _Alpha_outer;
  vector<vector<double> > _Alpha_stem;
  vector<vector<double> > _Alpha_stemend;
  vector<vector<double> > _Alpha_multi;
  vector<vector<double> > _Alpha_multibif;
  vector<vector<double> > _Alpha_multi1;
  vector<vector<double> > _Alpha_multi2;

  vector<double> _Beta_outer;
  vector<vector<double> > _Beta_stem;
  vector<vector<double> > _Beta_stemend;
  vector<vector<double> > _Beta_multi;
  vector<vector<double> > _Beta_multibif;
  vector<vector<double> > _Beta_multi1;
  vector<vector<double> > _Beta_multi2;

  void set_energy_parameters(){
    MLclosing = -ML_closing37*10/kT;
    MLintern = -ML_intern37*10./kT;
    MLbase = -ML_BASE37*10./kT;
    TermAU= -TerminalAU*10/kT;

    for (int i=0; i<=30; i++) {
      hairpin[i] = -hairpin37[i]*10./kT;
      bulge[i] = - bulge37[i]*10./kT;
      internal[i] = -internal_loop37[i]*10./kT;
    }

    for (int i=0; i< 7; i++){
      for (int j=0; j<5; j++){
	for (int k=0; k<5; k++) {
	  mismatchI[i][j][k] = -mismatchI37[i][j][k]*10.0/kT;
	  mismatchH[i][j][k] = -mismatchH37[i][j][k]*10.0/kT;
	}
      }

      for (int j=0; j<7; j++) {
	stack[i][j] = -stack37[i][j]*10./kT;
      }

      for (int j=0; j<=4; j++) {
	dangle5[i][j] = -dangle5_37[i][j]*10./kT;
	dangle3[i][j] = -dangle3_37[i][j]*10./kT;
	if (i>2){
	  dangle3[i][j] += TermAU;
	}
      }
    }

    for (int i=0; i<=7; i++){
      for (int j=0; j<=7; j++){
	for (int k=0; k<5; k++){
	  for (int l=0; l<5; l++){
	    int11[i][j][k][l] = -int11_37[i][j][k][l]*10./kT;
	    for (int m=0; m<5; m++){
	      int21[i][j][k][l][m] = -int21_37[i][j][k][l][m]*10./kT;
	      for (int n=0; n<5; n++){
		int22[i][j][k][l][m][n] = -int22_37[i][j][k][l][m][n]*10./kT;
	      }
	    }
	  }
	}
      }
    }

    for (int i=0; i<=MAXLOOP; i++){
      ninio[i]= -min(MAX_NINIO, i*F_ninio37)*10/kT ;
    }
  }

  void Initiallize(string &sequence);
  void CalcInsideVariable();
  void CalcOutsideVariable();
  void CalcAccessibility(string &sequence, int idx);
  void CalcAccessibility(string &sequence, vector<float> &accessibility, vector<float> &conditional_accessibility);
  double CalcExteriorProbability(int x, int w);
  void CalcHairpinProbability(vector<double> &hairpin_probability, vector<double> &conditional_hairpin_probability);
  double CalcMultiProbability(int x, int w);
  void CalcBulgeAndInternalProbability(vector<double> &biloop_probability, vector<double> &conditional_biloop_probability);
  void CalcLogSumBulgeAndInternalProbability(vector<double> &biloop_probability, vector<double> &conditional_biloop_probability);
  void Clear();

  double CalcDangleEnergy(int type,int a, int b);
  double logsumexp(double x,double y);
  double LoopEnergy(int type, int type2,int i,int j,int p,int q);
  double HairpinEnergy(int type, int i, int j);
};

#endif
