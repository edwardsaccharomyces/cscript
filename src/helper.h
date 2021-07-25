
#ifndef HELPER_H
#define HELPER_H

/*
 * 9/29/2018: I am incorporating these helper functions into my c++ JIT. This is essentially a supplemental runtime library of functions I frequently use.
 * 11/14/2019: updated with my latest helper functions.
 */


#include <string.h>
#include <cstdlib>
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <map>
#include <vector>
#include <ctime>
#include <fstream>
#include <limits>
#include <math.h>
#include <algorithm>


using namespace std;


// String manipulation items.
vector<string> split(const string &a, const string &tok);
string trim(const string &a, const string &totrim);
string trim(const string &a);
string strip(const string &a);
string lower(const string &a);
string upper(const string &a);
string replace(const string &str, const string& from, const string& to);
string replace(const string &str, const string& from, const string& to, const short ignoreCase);
string replacefirstinstanceof(const string &str, const string& from, const string& to, const short ignoreCase);
string replace(const string &str, const string& from1, const string &from2, const string& to);
void removeContent(const string &input, const string starttag, const string endtag, string &output);
string copy(const string &str, const string &from, const string &to, short useCaseInsensitive);
string concat(const vector<string> &a, const string &tok);
short endswith(const string &a, const string &b);
short startswith(const string &a, const string &b);
int compare(const string &a, const string &b, const short useCaseInsensitive);
string substr(const string &a, const size_t pos);
string substr(const string &a, const size_t pos, const size_t len);


// Variable type conversion items.
string tostring(int a);
string tostring(const long &a);
string tostring(const size_t &a);
string tostring(double a, int numdecimals);
string tostring(const vector<int> &a);
string tostring(const vector<string> &a);
short isint(const string &a);
int toint(const string &a);
void toshort(const string &a, short &wasSuccess, short &i);
double todouble(const string &a);

void inttobytearray(const int a, char *b);
void bytearraytoint(const char *b, int &a);


// File IO items.
vector<char> readFileBinary(const string &fileName);
string readFileText(const string &fileName);
void writeFileBinary(const string &fileName, const vector<char> &data);
void writeFileText(const string &fileName, const string &data);
string getfilenameleaf(const string &path);
string getfolderpath(const string &path);


// Time items.
time_t getCurrentDatetime();
time_t addDays(const time_t &currenttime, const int daysToAdd);
time_t addMinutes(const time_t &currenttime, const int daysToAdd);
void timefunction(const string &label, const clock_t &start);
short parseDatetime(const string &a, const int format, time_t &time);
string formatDatetime(const time_t &time, const int format);

clock_t timefunctionCpuStart();
long timefunctionCpuEnd(const clock_t &start);


// Linear alegbra items.
vector<vector<int> > transpose(const vector<vector<int> > &data);
void transform2d(const double &axmin, const double &axmax, const double &aymin, const double &aymax, const double &bxmin, const double &bxmax, const double &bymin, const double &bymax, const double &ax, const double &ay, double &bx, double &by);


// Basic math items.
double min(double a, double b);
double max(double a, double b);
double abs(double a);
double safelog(const double &a, const short warnOnNegativeInfinity);
int random(const int max);
long randoml(const long max);
string createRandomString(const size_t &length);


// Debugging and process control items.
void printstacktrace();
void print(const string &message);
void print(const string &message, const short includenewline);
void exiterr(const string &err);
void sleep(const long timeMs);


#endif
