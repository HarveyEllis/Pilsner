// (C) P.J. Dodd (2018): Distributed under CC BY 4.0 license https://creativecommons.org/licenses/by/4.0/
#ifndef EVENTDEFN               /* guard */
#define EVENTDEFN
// [[Rcpp::plugins(cpp11)]]

#include <iostream>
#include <string>
#include <stdlib.h>

#include <queue>
#include <fstream>
#include <ctime>
#include <math.h>

#include <vector>
#include <map>
#include <functional>
#include <algorithm>

#include <Rcpp.h>

/* /\* below to get GSL working *\/ */
/* #include <RcppGSL.h> */
/* #include <gsl/gsl_rng.h> */
/* #include <gsl/gsl_randist.h> */

using namespace std;
using namespace Rcpp;

/* person class */
class person{
 public:
  unsigned int who;             // population id
  double tle;                   // time of last event
  map< string, double> A;       // ages: age, other times-since-x (that advance with time)
  map< string, double> C;       // continuous states
  map< string, int> D;          // integer states
  // needs constructor
  person(unsigned int id,
         vector< string > AL, vector< double > AV,
         vector< string > CL, vector< double > CV,
         vector< string > DL, vector< int > DV);
};

// event structure
struct event{
  double t;
  unsigned int who;
  string what;
};

// comparator to make the priority queue work
struct LTbyTime{
  bool operator()(const event& lhs, const event& rhs) const {
    return lhs.t > rhs.t;       //care with direction!
  }
};

// for event queue
typedef priority_queue< event, vector<event>, LTbyTime> eventq;

//struct for recording events that happen
typedef struct {
  event evnt;
  map<string, double> Az;
  map<string, double> Cz;
  map<string, int> Dz;
} eventRecord;

// results class here
class results{
public:
  bool recording;
  
  // --- data ------
  vector<eventRecord> records;

  // ---- functions ----
  // function to capture event from event and person
  void record(event& todo, person& P){
    records.push_back(eventRecord{.evnt = todo, .Az = P.A, .Cz = P.C, .Dz = P.D});
  }

  // function to reshape 
  template<typename T, typename F> // v.important - T and F are not the same type! Lambda type != other type
  void extractFromRecords(std::vector<F>& inputVector,T extractor) {
    std::transform(records.begin(), records.end(), inputVector.begin(), extractor);
  }
  
  DataFrame getresult(){
    // depends on res.recording
    vector<double> tz(records.size());
    extractFromRecords(tz, [](eventRecord const& er) {return er.evnt.t; });
    
    vector<string> whatz(records.size());
    extractFromRecords(whatz, [](eventRecord const& er) {return er.evnt.what; });
    
    vector<unsigned int> whoz(records.size());
    extractFromRecords(whoz, [](eventRecord const& er) {return er.evnt.who; });
    
    DataFrame ans;
    if(recording){
      /* need to onvert tz to Numeric vector */
      NumericVector tzn; tzn.assign(tz.begin(),tz.end());
      StringVector wtzn; wtzn.assign(whatz.begin(),whatz.end());
      IntegerVector wozn; wozn.assign(whoz.begin(),whoz.end());
      ans = DataFrame::create(Named("time")=tzn,Named("event")=wtzn,Named("who")=wozn);
      /* TODO think about adding on state data for debugging, reshaping bit of pain */
    } else
      ans = DataFrame::create(Named("recording")=0,Named("turned")=0,Named("off")=0);
    return ans;
  }

}; /* will need function defns in pilsner */

      
// =========== function prototypes ============
int eventdoer( eventq& EQ, vector< person >& cohort, results& res,
               double& now, List& parmzl );
int updateages( map< string, double >& mymap, double dt );


/* actual event defn declaration */
int eventdefn( person& P,
               double& now,
               event & ev,
               eventq& EQ,
               NumericVector& Z);/* , */
               /* gsl_rng *r); */
#endif
