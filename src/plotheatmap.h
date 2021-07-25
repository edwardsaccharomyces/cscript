
#ifndef PLOTHEATMAP_H
#define PLOTHEATMAP_H

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

#include <cairo.h>
#include <cairo-pdf.h>
#include <gtk/gtk.h>

#include "helper.h"

using namespace std;

int plotHeatmapToWindow(int argc, char *argv[], const vector<pair<double, double> > &data, const vector<double> &xticks, const vector<double> &yticks, const string &title);

int plotHeatmapToPng(const vector<pair<double, double> > &data, const vector<double> &xticks, const vector<double> &yticks, const string &title, const string &filename);


#endif
