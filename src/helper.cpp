
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
#include <stdexcept>
#include <iomanip>
#include <chrono>
#include <pthread.h>
#include <execinfo.h>
#include <signal.h>

#include "helper.h"

using namespace std;



////////////////////////////////////////////////////////////
/////////////// debugging + process ctrl ///////////////////
////////////////////////////////////////////////////////////

// "inline" is important here, so that we don't print the stack trace of calling this function!
inline void printstacktrace()
{
	// https://stackoverflow.com/questions/77005/how-to-automatically-generate-a-stacktrace-when-my-program-crashes
	void *array[10];
	size_t size;
	// get void*'s for all entries on the stack
	size = backtrace(array, 10);
	// print out all the frames to stderr
	backtrace_symbols_fd(array, size, STDERR_FILENO);
}

// See https://stackoverflow.com/questions/77005/how-to-automatically-generate-a-stacktrace-when-my-program-crashes
void printstacktrace3(int sig)
{
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}

void print(const string &message, const short includenewline)
{
	//ostream os = cout;

	// Special characters can mess up the output.
	string message2 = message;
	message2 = replace(message2, "\r", "");

	cout << message2;
	if(includenewline)
		cout << endl;
}

void print(const string &message)
{
	print(message, 1);
}

void exiterr(const string &err)
{
	print(err);
	exit(1);
}



////////////////////////////////////////////////////////////
///////////////////// string funcs /////////////////////////
////////////////////////////////////////////////////////////

vector<string> split(const string &a, const string &tok)
{
	vector<string> out;

	//11/3/2018: added this check, as otherwise empty input would return garbled and non-empty output.
	if(a.empty())
	{
		return out;
	}

	size_t current = 0;
	size_t i = 0;
	
	while((i = a.find(tok, current)) != string::npos)
	{
		out.push_back(substr(a, current, i - current));
		current = i + tok.size();
	}

	// Add the last item
	out.push_back(substr(a, current, string::npos));
	return out;
}

size_t splitHelper(const string &a, const vector<char> &toks, const size_t currenti)
{
	size_t mini = a.length() + 1;
	for(vector<char>::const_iterator iter=toks.begin(); iter!=toks.end(); iter++)
	{
		size_t tmp = a.find(*iter, currenti);
		if(tmp < mini)
		{
			mini = tmp;
		}
	}

	if(mini == a.length() + 1)
	{
		return string::npos;
	}

	return mini;
}

vector<string> split(const string &a, const vector<char> &toks)
{
	vector<string> out;

	//11/3/2018: added this check, as otherwise empty input would return garbled and non-empty output.
	if(a.empty())
	{
		return out;
	}

	size_t current = 0;
	size_t i = 0;
	
	//while((i = a.find(tok, current)) != string::npos)
	while((i = splitHelper(a, toks, current)) != string::npos)
	{
		out.push_back(substr(a, current, i - current));
		// Tokens are always a char, so length = 1.
		current = i + 1;
	}

	// Add the last item
	out.push_back(substr(a, current, string::npos));
	return out;
}

// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
string trim(const string &a, const string &totrim)
{
	string b = a;
	size_t start = b.find_first_not_of(totrim);
	size_t end = b.find_last_not_of(totrim);
	
	// Only complete the trim if both indexes are valid.
	if(start != string::npos && end != string::npos)
	{
		++end;

		// Trying to find a bug with this if statement. 11/9/2019.
		if(b.size() < end-start || start < 0)
			print("substr error in helper::trim");

		b = substr(b, start, end - start);
	}
	// Otherwise, the string must have been only whitespace, and so return empty string.
	else
	{
		b = "";
	}
	return b;
}

string trim(const string &a)
{
	return trim(a, " \t\n\r");
}

string strip(const string &a)
{
	return trim(a);
}

string lower(const string &a)
{
	string b = a;
	transform(b.begin(), b.end(), b.begin(), ::tolower);
	return b;
}

string upper(const string &a)
{
	string b = a;
	transform(b.begin(), b.end(), b.begin(), ::toupper);
	return b;
}

string limitsize(const string &a, const size_t &maxlength, short *didresize)
{
	if(a.size() > maxlength)
	{
		if(NULL != didresize)
			(*didresize) = 1;

		return substr(a, 0, maxlength);
	}
	if(NULL != didresize)
		(*didresize) = 0;
	return a;
}

string replace(const string &str, const string& from, const string& to)
{
	return replace(str, from, to, 0);
}

// http://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
string replace(const string &str, const string& from, const string& to, const short ignoreCase)
{
	string str2 = str;

	// Strategy for support case insensitive here is to have a duplicate of str and from that is made lower case but which still matches the indexes of the non-lower string.
	string strFind = str;
	string fromFind = from;
	if(ignoreCase)
	{
		strFind = lower(strFind);
		fromFind = lower(fromFind);
	}
	size_t start_pos = 0;
	while((start_pos = strFind.find(fromFind, start_pos)) != string::npos)
	{
		// Update both the string destined to be output and the strFind string, so that indexes will continue to match.
		str2.replace(start_pos, from.length(), to);
		strFind.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
	return str2;
}

// Used to replace the first instance of <blockquote> in a reply in order to overcome a javascript editor bug. Same code as replace2(...), but with "if" instead of "while"
string replacefirstinstanceof(const string &str, const string& from, const string& to, const short ignoreCase)
{
	string str2 = str;

	// Strategy for support case insensitive here is to have a duplicate of str and from that is made lower case but which still matches the indexes of the non-lower string.
	string strFind = str;
	string fromFind = from;
	if(ignoreCase)
	{
		strFind = lower(strFind);
		fromFind = lower(fromFind);
	}
	size_t start_pos = 0;
	if((start_pos = strFind.find(fromFind, start_pos)) != string::npos)
	{
		// Update both the string destined to be output and the strFind string, so that indexes will continue to match.
		str2.replace(start_pos, from.length(), to);
		strFind.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
	return str2;
}

// Replaces all the text between the first match of from1 and the first match of from2 with to. This is useful when processing page html when i need to cut out a region in order to print it back out in a loop.
string replace(const string &str, const string& from1, const string &from2, const string& to)
{
	string str2 = str;
	size_t start_pos1 = 0, start_pos2 = 0;
	while((start_pos1 = str2.find(from1, start_pos1)) != string::npos)
	{
		start_pos2 = str2.find(from2, start_pos1);
		if(start_pos2 != string::npos)
		{
			str2.replace(start_pos1, start_pos2 - start_pos1 + from2.length(), to);
		}
		else
		{
			str2.replace(start_pos1, str2.size() - start_pos1, to);
		}
		start_pos1 += to.length(); // Handles case where 'to' is a substring of 'from'
    }
	return str2;
}

// Mostly used to remove "<style>" tags in html.
void removeContent(const string &input, const string starttag, const string endtag, string &output)
{
	//string starttag = "<style";
	//string endtag = "</style>";

	string tmp = input;
	string tmp2Lower = lower(tmp);
	size_t start = tmp2Lower.find(starttag);
	size_t end;
	
	while(start != string::npos)
	{
		// Get the end of this element.
		end = tmp2Lower.find(endtag, start);

		if(end == string::npos)
		{
			// Badly formatted input. Finish processing and break.
			output = tmp;
			return;
		}

		// Update the text and lowercase version of text.
		string tmp1 = substr(tmp, 0, start);
		string tmp2 = substr(tmp, end + endtag.length());
		tmp = tmp1 + tmp2;
		tmp2Lower = lower(tmp);

		// More style elements?
		start = tmp2Lower.find(starttag);
	}

	// Copy to output.
	output = tmp;
}

// Copies a substring.
string copy(const string &str, const string &from, const string &to, short useCaseInsensitive)
{
	if(str.empty())
	{
		return str;
	}

	size_t start_pos = 0;
	size_t end_pos = str.length();
	if(useCaseInsensitive)
	{
		string fromlower = lower(from);
		string tolower = lower(to);
		string strlower = lower(str);

		if(!from.empty())
			start_pos = strlower.find(fromlower);
		if(!to.empty())
			end_pos = strlower.find(tolower, start_pos);
	}
	else
	{
		if(!from.empty())
			start_pos = str.find(from);
		if(!to.empty())
			end_pos = str.find(to, start_pos);
	}

	if(start_pos != string::npos && end_pos != string::npos)
	{
		start_pos += from.size();
		char buf[str.size()];
		bzero(buf, str.size());
		str.copy(buf, end_pos - start_pos, start_pos);

		return string(buf);
	}
	else
	{
		print("COULD NOT PERFORM COPY PROCEDURE!!!");
		print(from);
		print(to);
			
		print(tostring(start_pos));
		print(tostring(end_pos));
		print(tostring(str.size()));


		return string();
	}
}

string concat(const vector<string> &a, const string &tok, const short concatemptyvals)
{
	string b;
	for(vector<string>::const_iterator iter = a.begin(); iter != a.end(); iter++)
	{
		if(concatemptyvals || !(*iter).empty())
		{
			b += *iter + tok;
		}
	}
	return b;
}

string concat(const vector<string> &a, const string &tok)
{
	return concat(a, tok, 1);
}

short endswith(const string &a, const string &b)
{
	if(a.length() < b.length())
	{
		return 0;
	}
	return (a.compare(a.length() - b.length(), b.length(), b) == 0);
}

short startswith(const string &a, const string &b)
{
	if(a.length() < b.length())
	{
		return 0;
	}
	return (a.compare(0, b.length(), b) == 0);
}

int compare(const string &a, const string &b, const short useCaseInsensitive)
{
	string aa = a;
	string bb = b;
	if(useCaseInsensitive)
	{
		aa = lower(a);
		bb = lower(b);
	}
	return strcmp(aa.c_str(), bb.c_str());
}

string substr(const string &a, const size_t pos, const size_t len)
{
	// This function wraps the native substr function, and provides string bounds checking. i currently have a bug somewhere that is due to an out of bounds substring, so this function will hopefully help me solve it... 11/8/2019.

	if(pos < 0 || a.size() < pos || (len != string::npos && len < 0) || (len != string::npos && a.size() < pos + len))
	{
		print("Error: out of bounds substr requested.");
		printstacktrace();
	}

	size_t pos2 = pos;

	if(pos2 < 0)
		pos2 = 0;
	if(a.size() < pos2)
		pos2 = a.size();

	return a.substr(pos2, len);
}

string substr(const string &a, const size_t pos)
{
	return substr(a, pos, string::npos);
}

////////////////////////////////////////////////////////////
/////////////////// type conversions ///////////////////////
////////////////////////////////////////////////////////////

string tostring(int a)
{
	return to_string(a);
}

string tostring(const long &a)
{
	return to_string(a);
}

string tostring(const size_t &a)
{
	return to_string(a);
}

string tostring(double a, int numdecimals)
{
	stringstream ss;
	ss << fixed << setprecision(numdecimals) << a;
	return ss.str();
}

string tostring(const vector<int> &a)
{
	string s;
	for(vector<int>::const_iterator iter=a.begin();iter!=a.end();iter++)
	{
		s += tostring(*iter) + string(", ");
	}
	if(!s.empty())
	{
		s.pop_back();
		s.pop_back();
	}
	return s;
}

string tostring(const vector<string> &a)
{
	string s;
	for(vector<string>::const_iterator iter=a.begin();iter!=a.end();iter++)
	{
		s += *iter + string(", ");
	}
	if(!s.empty())
	{
		s.pop_back();
		s.pop_back();
	}
	return s;
}

short isint(const string &a)
{
	// http://www.cplusplus.com/reference/stdexcept/invalid_argument/
	try
	{
		// Need to return "1" if yes, but don't want the stoi() call to be optimized out if we don't use the value returned from it, so try and trick the compiler.
		int b = stoi(a);
		b -= stoi(a);
		b += 1;
		return b;
	}
	catch (const std::invalid_argument& ia)
	{
		return 0;
	}
}

int toint(const string &a)
{
	// http://www.cplusplus.com/reference/stdexcept/invalid_argument/
	try
	{
		return stoi(a);
	}
	catch (const std::invalid_argument& ia)
	{
		print(string("Error. Invalid argument in ") + string(ia.what()) + string(". Input: \"") + a + string("\""));
		printstacktrace();
		exit(1);
	}
}

double todouble(const string &a)
{
	// http://www.cplusplus.com/reference/stdexcept/invalid_argument/
	try
	{
		return stod(a);
	}
	catch (const std::invalid_argument& ia)
	{
		print(string("Error. Invalid argument in ") + string(ia.what()) + string(". Input: \"") + a + string("\""));
		printstacktrace();
		exit(1);
	}
}

short toshort(const string &a)
{
	// http://www.cplusplus.com/reference/stdexcept/invalid_argument/
	try
	{
		int ii = stoi(a);
		if(ii < numeric_limits<short>::max() && ii > numeric_limits<short>::min())
		{
			return (short)ii;
		}
		else
		{
			print(string("Error. Conversion to short causes out of bounds. ") + string("Input: \"") + a + string("\""));
			printstacktrace();
			exit(1);
		}
	}
	catch (const std::invalid_argument& ia)
	{
		print(string("Error. Invalid argument in ") + string(ia.what()) + string(". Input: \"") + a + string("\""));
		printstacktrace();
		exit(1);
	}
}

void toshort(const string &a, short &wasSuccess, short &i)
{
	char *endCheck = NULL;
	int ii = strtol(a.c_str(), &endCheck, 10);
	if(endCheck == NULL)
	{
		// Failed to convert string completely into integer.
		wasSuccess = 0;
	}
	else
	{
		if(ii < numeric_limits<short>::max() && ii > numeric_limits<short>::min())
		{
			i = (short)ii;
			wasSuccess = 1;
		}
		else
		{
			wasSuccess = 0;
		}
	}
}

void inttobytearray(const int a, char *b)
{
	// Caller must allocate b to be char b[sizeof(int)]
	//char bytes[sizeof x];
	std::copy(static_cast<const char*>(static_cast<const void*>(&a)),
		static_cast<const char*>(static_cast<const void*>(&a)) + sizeof a,
		b);
}

void bytearraytoint(const char *b, int &a)
{
	// Caller must allocate b to be char b[sizeof(int)]
	std::copy(b, b + sizeof a,
		static_cast<char*>(static_cast<void*>(&a)));
}
////////////////////////////////////////////////////////////
//////////////////////// time funcs ////////////////////////
////////////////////////////////////////////////////////////

time_t getCurrentDatetime()
{
	time_t rawTime;
	time(&rawTime);
	return rawTime;
}

time_t addDays(const time_t &currenttime, const int daysToAdd)
{
	struct tm* tm = localtime(&currenttime);
	tm->tm_mday += daysToAdd;
	time_t next = mktime(tm);
	return next;
}

time_t addMinutes(const time_t &currenttime, const int minsToAdd)
{
	struct tm* tm = localtime(&currenttime);
	tm->tm_min += minsToAdd;
	time_t next = mktime(tm);
	return next;
}

clock_t timefunctionCpuStart()
{
	// Measures CPU time spent.
	return clock();
}

long timefunctionCpuEnd(const clock_t &start)
{
	long clocksperms = CLOCKS_PER_SEC / 1000;
	long functimeMs = (long) ((double)(clock() - start) / ((double)clocksperms) );
	return functimeMs;
}

short parseDatetime(const string &a, const int format, time_t &time)
{
	struct tm tm;
	char *ret = NULL;
	switch(format)
	{
		case 0:
		{
			ret = strptime(a.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
			break;
		}
		case 1:
		{
			// Used in SiteHelper::convertDocToEmailFull()
			// Sat, 18 Jun 2016 16:23:45 +0000
			ret = strptime(a.c_str(), "%a, %d %b %Y %H:%M:%S %Z", &tm);
			break;
		}
		case 2:
		{
			// Used in SiteHelper::convertDocToEmailFull()
			// 29 Sep 2016 08:58:14 -0400
			ret = strptime(a.c_str(), "%d %b %Y %H:%M:%S %Z", &tm);
			break;
		}
		default:
		{
			print("Unknown format passed to Helper::parseDatetime()");
			return 2;
		}
	}
	if(NULL != ret)
	{
		time = mktime(&tm);
		return 0;
	}
	// Failed to parse the string.
	return 1;
}


// http://stackoverflow.com/questions/16357999/current-date-and-time-as-string
string formatDatetime(const time_t &time, const int format)
{
	struct tm * timeInfo;
	char buf[80];
	bzero(buf, sizeof(buf));

	timeInfo = localtime(&time);
	switch(format)
	{
		case 0:
		{
			strftime(buf, sizeof(buf), "%H:%M %m/%d", timeInfo);
			break;
		}
		case 1:
		{
			strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", timeInfo);
			break;
		}
		case 2:
		{
			strftime(buf, sizeof(buf), "%m/%d %H:%M", timeInfo);
			break;
		}
		case 3:
		{
			strftime(buf, sizeof(buf), "%m/%d/%Y %H:%M", timeInfo);
			break;
		}
		case 4:
		{
			// Used for mysql datetimes.
			strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", timeInfo);
			break;
		}
		case 5:
		{
			// Used for email header.
			// Mon, 29 Nov 2010 21:54:29 +1100
			strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", timeInfo);
			break;
		}
		case 6:
		{
			// Used for generating unique strings (e.g. email message-id).
			strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", timeInfo);
			break;
		}
		case 7:
		{
			// Used for curt timestamps in logger.cpp.
			strftime(buf, sizeof(buf), "%H:%M:%S", timeInfo);
			break;
		}
		default:
		{
			sprintf(buf, "no time format specified");
		}
	}
	return string(buf);
}

////////////////////////////////////////////////////////////
///////////////////// linear algebra ///////////////////////
////////////////////////////////////////////////////////////

// https://stackoverflow.com/questions/6009782/how-to-pivot-a-vector-of-vectors
vector<vector<int> > transpose(const vector<vector<int> > &data)
{
	// This assumes that all inner vectors have the same size and allocates space for the complete result in advance.
	vector<vector<int> > result(data[0].size(), vector<int>(data.size()));

	for (vector<int>::size_type i = 0; i < data[0].size(); i++)
	{
		for (vector<int>::size_type j = 0; j < data.size(); j++)
		{
			result[i][j] = data[j][i];
		}
	}
	return result;
}

void transform2d(const double &axmin, const double &axmax, const double &aymin, const double &aymax, const double &bxmin, const double &bxmax, const double &bymin, const double &bymax, const double &ax, const double &ay, double &bx, double &by)
{
	double xx = (bxmax - bxmin) / (axmax - axmin);
	double yy =  (bymax - bymin) / (aymax - aymin);

	bx = (xx * (ax - axmin)) + bxmin;
	by = (yy * (ay - aymin)) + bymin;
}



////////////////////////////////////////////////////////////
//////////////////////// basic math ////////////////////////
////////////////////////////////////////////////////////////

string createRandomString(const size_t &length)
{
	// see https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
	string a;

	static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < length; ++i)
	{
        a += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

	return a;
}

short ghelperisrandomseeded = 0;
int random(const int max)
{
	// http://www.cplusplus.com/reference/cstdlib/rand/
	if(!ghelperisrandomseeded)
	{
		srand(time(NULL));
		ghelperisrandomseeded = 1;
	}

	if(RAND_MAX < max)
	{
		exiterr("Requested random number larger than RAND_MAX!");
	}

	return rand() % max;
}

long randoml(const long max)
{
	// http://www.cplusplus.com/reference/cstdlib/rand/
	if(!ghelperisrandomseeded)
	{
		srand(time(NULL));
		ghelperisrandomseeded = 1;
	}

	long r = 0;
	int *i = reinterpret_cast<int *>(&r);
	i[0] = rand();
	i[1] = rand();

	r = r % max;
	return r;
}

double min(double a, double b)
{
	if(a > b)
		return b;
	return a;
}

double max(double a, double b)
{
	if(a > b)
		return a;
	return b;
}

double abs(double a)
{
	return fabs(a);
}

double safelog(const double &a, const short warnOnNegativeInfinity)
{
	double small = 0.00000000000001;
	if(a > 0)
	{
		return log(a);
	}
	else
	{
		if(warnOnNegativeInfinity)
		{
			print("Requested log(0), which is negative infinity!!");
		}
		return log(small);
	}
}


