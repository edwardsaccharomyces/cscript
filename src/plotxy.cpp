
#include "plotxy.h"

using namespace std;


// See http://zetcode.com/gfx/cairo/cairobackends/

struct PlotXYData
{
	GtkWidget *window;
	GtkWidget *darea;

	string title;
	double dataminX;
	double datamaxX;
	double dataminY;
	double datamaxY;
	int xborder;
	int yborder;
	int ticklength;

	vector<pair<double, double> > data;
	vector<double> xticks;
	vector<double> yticks;

	PlotXYData() : window(),darea(),title(),dataminX(),datamaxX(),dataminY(),datamaxY(),xborder(),yborder(),ticklength()
	{}
};

// Cairo callbacks start.



void drawXYAxes(cairo_t *cr, int height, int width, const PlotXYData &data)
{
	double windowmaxX = width - data.xborder;
	double windowminX = data.xborder;
	double windowmaxY = data.yborder;
	double windowminY = height - data.yborder;

	double xmin, xmax, ymin, ymax;
	transform2d(data.dataminX,data.datamaxX,data.dataminY,data.datamaxY, windowminX,windowmaxX,windowminY,windowmaxY, data.dataminX, data.dataminY, xmin, ymin);
	transform2d(data.dataminX,data.datamaxX,data.dataminY,data.datamaxY, windowminX,windowmaxX,windowminY,windowmaxY, data.datamaxX, data.datamaxY, xmax, ymax);

	/*printf("axmin: %f axmax: %f aymin: %f aymax: %f bxmin: %f bxmax: %f bymin: %f bymax: %f\n", dataminX,datamaxX,dataminY,datamaxY, windowminX,windowmaxX,windowminY,windowmaxY);
	printf("ax: %f ay: %f bx: %f by: %f\n", dataminX, dataminY, xmin, ymin);
	printf("ax: %f ay: %f bx: %f by: %f\n", datamaxX, datamaxY, xmax, ymax);*/

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 0.5);
	// X
	cairo_move_to(cr, xmin, ymin);
	cairo_line_to(cr, xmax, ymin);
	cairo_stroke(cr);
	// Y
	cairo_move_to(cr, xmin, ymin);
	cairo_line_to(cr, xmin, ymax);
	cairo_stroke(cr);

	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 12.0);

	for(vector<double>::const_iterator iter=data.xticks.begin(); iter!=data.xticks.end(); iter++)
	{
		double x,y;
		transform2d(data.dataminX,data.datamaxX,data.dataminY,data.datamaxY, windowminX,windowmaxX,windowminY,windowmaxY, *iter, data.dataminY, x, y);
		cairo_move_to(cr, x, y);
		cairo_line_to(cr, x, y + 5);
		cairo_stroke(cr);

		cairo_move_to(cr, x + 5, y + 30);
		cairo_save(cr);
		cairo_rotate(cr, 3.0 * M_PI / 2.0);
		cairo_show_text(cr, tostring(*iter, 1).c_str());
		cairo_restore(cr);
	}

	for(vector<double>::const_iterator iter=data.yticks.begin(); iter!=data.yticks.end(); iter++)
	{
		double x,y;
		transform2d(data.dataminX,data.datamaxX,data.dataminY,data.datamaxY, windowminX,windowmaxX,windowminY,windowmaxY, data.dataminX, *iter, x, y);
		cairo_move_to(cr, x, y);
		cairo_line_to(cr, x + 5, y);
		cairo_stroke(cr);

		cairo_move_to(cr, x - 30, y + 5);
		cairo_show_text(cr, tostring(*iter, 1).c_str());
	}

	cairo_stroke(cr);
}

void drawXYData(cairo_t *cr, int height, int width, const PlotXYData &data)
{
	double windowmaxX = width - data.xborder;
	double windowminX = data.xborder;
	double windowmaxY = data.yborder;
	double windowminY = height - data.yborder;

	for(vector<pair<double, double> >::const_iterator iter=data.data.begin(); iter!=data.data.end(); iter++)
	{
		double x, y;
		transform2d(data.dataminX,data.datamaxX,data.dataminY,data.datamaxY, windowminX,windowmaxX,windowminY,windowmaxY, (*iter).first, (*iter).second, x, y);

		// http://zetcode.com/gfx/cairo/transformations/
		cairo_save(cr);
		cairo_set_source_rgb(cr, 0.69, 0.19, 0);
		cairo_set_line_width(cr, 4);
		cairo_translate(cr, x, y);
		cairo_arc(cr, 0, 0, 2, 0, 2 * M_PI);
		cairo_stroke_preserve(cr);

		cairo_set_source_rgb(cr, 0.3, 0.4, 0.6); 
		cairo_fill(cr);
		cairo_restore(cr);
	}
}

gboolean xyOnDrawEvent(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	PlotXYData *data = (PlotXYData*)user_data;

	int height, width;
	GtkWidget *win = gtk_widget_get_toplevel((*data).darea);
	gtk_window_get_size(GTK_WINDOW(win), &width, &height);

	drawXYAxes(cr, height, width, *data);
	drawXYData(cr, height, width, *data);

	return FALSE;
}

gboolean xyClicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	PlotXYData *data = (PlotXYData*)user_data;

	int height, width;
	GtkWidget *win = gtk_widget_get_toplevel((*data).darea);
	gtk_window_get_size(GTK_WINDOW(win), &width, &height);

	double windowmaxX = width - data->xborder;
	double windowminX = data->xborder;
	double windowmaxY = data->yborder;
	double windowminY = height - data->yborder;

	double x, y;
	transform2d(windowminX,windowmaxX,windowminY,windowmaxY, data->dataminX,data->datamaxX,data->dataminY,data->datamaxY, event->x, event->y, x, y);

	printf("Clicked (x,y): (%f, %f)\n", x, y);

    return TRUE;
}
// Cairo callbacks end.

void plotXYSetupdata(const vector<pair<double, double> > &data, const vector<double> &xticks, const vector<double> &yticks, const string &title, PlotXYData &pxydata)
{
	pxydata.title = title;
	pxydata.xborder = 50;
	pxydata.yborder = 40;
	pxydata.ticklength = 5;

	for(vector<pair<double, double> >::const_iterator iter=data.begin(); iter!=data.end(); iter++)
	{
		pxydata.data.push_back(*iter);
		//printf("(%f, %f)\n", (*iter).first, (*iter).second);
	}
	for(vector<double>::const_iterator iter=xticks.begin(); iter!=xticks.end(); iter++)
	{
		pxydata.xticks.push_back(*iter);
	}
	for(vector<double>::const_iterator iter=yticks.begin(); iter!=yticks.end(); iter++)
	{
		pxydata.yticks.push_back(*iter);
	}

	pxydata.dataminX = numeric_limits<double>::max();
	pxydata.datamaxX = numeric_limits<double>::min();
	pxydata.dataminY = numeric_limits<double>::max();
	pxydata.datamaxY = numeric_limits<double>::min();

	// Calculate min and max.
	for(vector<pair<double, double> >::const_iterator iter=pxydata.data.begin(); iter!=pxydata.data.end(); iter++)
	{
		if(pxydata.dataminX > (*iter).first)
			pxydata.dataminX = (*iter).first;
		if(pxydata.datamaxX < (*iter).first)
			pxydata.datamaxX = (*iter).first;
		if(pxydata.dataminY > (*iter).second)
			pxydata.dataminY = (*iter).second;
		if(pxydata.datamaxY < (*iter).second)
			pxydata.datamaxY = (*iter).second;
	}
}


int plotXYToWindow(int argc, char *argv[], const vector<pair<double, double> > &data, const vector<double> &xticks, const vector<double> &yticks, const string &title)
{
	// TODO: somewhere i ought to free this memory...
	PlotXYData *pxydata = new PlotXYData();
	plotXYSetupdata(data, xticks, yticks, title, *pxydata);

	gtk_init(&argc, &argv);
	pxydata->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	pxydata->darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(pxydata->window), pxydata->darea);
	
	g_signal_connect(G_OBJECT(pxydata->darea), "draw", G_CALLBACK(xyOnDrawEvent), pxydata);
	g_signal_connect(pxydata->window, "destroy", G_CALLBACK(gtk_main_quit), pxydata);
	g_signal_connect(pxydata->window, "button-press-event", G_CALLBACK(xyClicked), pxydata);

	gtk_window_set_position(GTK_WINDOW(pxydata->window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(pxydata->window), 400, 300); 
	gtk_window_set_title(GTK_WINDOW(pxydata->window), title.c_str());

	gtk_widget_show_all(pxydata->window);

	gtk_main();

	return 0;
}

int plotXYToPng(const vector<pair<double, double> > &data, const vector<double> &xticks, const vector<double> &yticks, const string &title, const string &filename)
{
	PlotXYData *pxydata = new PlotXYData();
	plotXYSetupdata(data, xticks, yticks, title, *pxydata);

	cairo_surface_t *surface;
	cairo_t *cr;

	int height = 300;
	int width = 400;
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	cr = cairo_create(surface);

	// Background color.
	/*cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, width, height);    
	cairo_fill(cr);*/

	// Draw plot.
	drawXYAxes(cr, height, width, *pxydata);
	drawXYData(cr, height, width, *pxydata);

	cairo_surface_write_to_png(surface, filename.c_str());

	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	delete pxydata;
	return 0;
}

int drawToPdf() 
{
	cairo_surface_t *surface;
	cairo_t *cr;
	
	surface = cairo_pdf_surface_create("pdffile.pdf", 504, 648);
	cr = cairo_create(surface);
	
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 40.0);
	
	cairo_move_to(cr, 10.0, 50.0);
	cairo_show_text(cr, "Disziplin ist Macht.");
	
	cairo_show_page(cr);
	
	cairo_surface_destroy(surface);
	cairo_destroy(cr);
	
	return 0;
}


