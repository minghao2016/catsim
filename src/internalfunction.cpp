#include <Rcpp.h>
using namespace Rcpp;


// [[Rcpp::export]]
double C_gini(NumericVector x){
  R_xlen_t n = x.size();
  std::map<double, double> counts;

  for (R_xlen_t i = 0; i < n; i++) {
    counts[x[i]]++;
  }

  double sqsum = 0.0;
  for (std::map<double, double>::iterator it = counts.begin(); it != counts.end(); ++it)  {
    sqsum += 1.0 * (it->second) *(it->second);
  }
  return (1.0 - sqsum/(1.0*n*n));
}


// [[Rcpp::export]]
double C_ginicorr(NumericVector x, double k){
  double eps = 1e-5;
  if(std::abs(k - 1.0) < eps) return C_gini(x);

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
  R_xlen_t n = x.size();
  if (n != y.size()) Rcpp::stop("X and Y must have the same length.");
  std::map<double, double> countsx;
  std::map<double, double> countsy;

  for (R_xlen_t i = 0; i < n; i++) {
    countsx[x[i]]++;
    countsy[y[i]]++;
  }

  double sqsum = 0.0;
  for (std::map<double,double>::iterator it = countsx.begin(); it != countsx.end(); ++it)  {
    sqsum += 1.0 * (it->second) * (it->second);
  }
  for (std::map<double,double>::iterator it = countsy.begin(); it != countsy.end(); ++it)  {
    sqsum += 1.0 * (it->second) * (it->second);
  }

  double xysum = 0.0;
  std::map<double,double>::iterator il = countsx.begin();
  std::map<double,double>::iterator ir = countsy.begin();
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

// [[Rcpp::export]]
double C_Cohen(NumericVector x, NumericVector y){
  R_xlen_t n = x.size();
  if (n != y.size()) Rcpp::stop("X and Y must have the same length.");
  NumericMatrix xy(n, 2);
  xy.column(0) = x;
  xy.column(1) = y;
  std::map<double, double> countsx;
  std::map<double, double> countsy;
  std::map<double, double> countsxy;

  countsx.clear();
  countsy.clear();

  for (R_xlen_t i = 0; i < n; i++) {
    countsx[x[i]]++;
    countsy[y[i]]++;
    if (x[i] == y[i]){
      countsxy[x[i]]++;
    }

  }

  double xxyysum = 0.0;
  std::map<double,double>::iterator il = countsx.begin();
  std::map<double,double>::iterator ir = countsy.begin();
  while (il != countsx.end() && ir != countsy.end())
  {
    if (il->first < ir->first)
      ++il;
    else if (ir->first < il->first)
      ++ir;
    else
    {
      xxyysum += (il->second) * (ir->second);
      ++il;
      ++ir;
    }
  }
  double xysum = 0.0;

  for (std::map<double, double>::iterator it = countsxy.begin(); it != countsxy.end(); ++it)  {
    xysum  += it->second;
    }



  double pe = xxyysum / (n*n);
  double po = xysum / (n);

  if ((1-pe) < 1e-6 ) {
    return 1.0;
  }
  return (po-pe)/(1-pe);

}



// [[Rcpp::export]]
double C_AdjRand(NumericVector x, NumericVector y){
  double eps = 1e-3;
  R_xlen_t n = x.size();
  if (n != y.size()) Rcpp::stop("X and Y must have the same length.");
  NumericMatrix xy(n, 2);
  xy.column(0) = x;
  xy.column(1) = y;
  std::map<double, double> countsx;
  std::map<double, double> countsy;
  std::map<std::vector<double>, double> count_rows;
  countsx.clear();
  countsy.clear();
  count_rows.clear();
  for (R_xlen_t i = 0; i < n; i++) {
    countsx[x[i]]++;
    countsy[y[i]]++;
    NumericVector a = xy.row(i);
    std::vector<double> b = Rcpp::as< std::vector<double> >(a);

    // Add to map
    count_rows[ b ] += 1.0;
  }

  double ai = 0.0;
  double bi = 0.0;
  double nij = 0.0;

  for (std::map<double,double>::iterator it = countsx.begin(); it != countsx.end(); ++it)  {
    int tmp = it->second;
    ai += (tmp) * (tmp - 1.0)/2.0;
  }
  for (std::map<double,double>::iterator it = countsy.begin(); it != countsy.end(); ++it)  {
    int tmp = it->second;
    bi += (tmp) * (tmp - 1.0)/2.0;
  }
  for (std::map<std::vector<double>, double>::iterator it = count_rows.begin(); it != count_rows.end(); ++it)  {
    int tmp = it->second;
    nij += (tmp) * (tmp - 1.0)/2.0;
  }

  return (nij - ai * bi / (n * (n-1)/2) + eps) / (.5 * (ai + bi) - ai * bi / (n * (n-1)/2) + eps);

}