#include <Rcpp.h>
using namespace Rcpp;

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//


// [[Rcpp::export]]
double C_gini(NumericVector x){
  int n = x.size();
  std::map<double, int> counts;

  for (int i = 0; i < n; i++) {
    counts[x[i]]++;
  }

  double sqsum = 0.0;
  for (std::map<double,int>::iterator it = counts.begin(); it != counts.end(); ++it)  {
    sqsum += 1.0 * (it->second) *(it->second);
  }
  return (1.0 - sqsum/(1.0*n*n));
}


// [[Rcpp::export]]
double C_ginicorr(NumericVector x, double k){
  double eps = 1e-5;
  if(abs(k - 1.0) < eps) return C_gini(x);

  return C_gini(x) / (1-1/k);
}

// [[Rcpp::export]]
double C_cfunc(NumericVector x, NumericVector y, double c, double k){
  double varx = C_ginicorr(x, k);
  double vary = C_ginicorr(y, k);

  return(2*sqrt(varx * vary) + c)/(varx + vary + c);

}

// [[Rcpp::export]]
double C_meansfunc(NumericVector x, NumericVector y, double c){
  int n = x.size();
  if (n != y.size()) Rcpp::stop("X and Y must have the same length.");
  std::map<double, int> countsx;
  std::map<double, int> countsy;

  for (int i = 0; i < n; i++) {
    countsx[x[i]]++;
    countsy[y[i]]++;
  }

  double sqsum = 0.0;
  for (std::map<double,int>::iterator it = countsx.begin(); it != countsx.end(); ++it)  {
    sqsum += 1.0 * (it->second) *(it->second);
  }
  for (std::map<double,int>::iterator it = countsy.begin(); it != countsy.end(); ++it)  {
    sqsum += 1.0 * (it->second) *(it->second);
  }

  double xysum = 0.0;
  std::map<double,int>::iterator il = countsx.begin();
  std::map<double,int>::iterator ir = countsy.begin();
  while (il != countsx.end() && ir != countsy.end())
  {
    if (il->first < ir->first)
      ++il;
    else if (ir->first < il->first)
      ++ir;
    else
    {
      xysum += (il->second) * (ir->second);
      ++il;
      ++ir;
    }
  }


  return (2*(xysum) + c)/(sqsum + c);
}
