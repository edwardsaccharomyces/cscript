#!/home/edward/projects/fun/cscript/cscript


using namespace std;

int main(int argc, char *argv[])
{
	// Read in commandline args.
	if(argc < 2)
	{
		exiterr("Usage: " + string(argv[0]) + " <Filename 1> <Output filename>");
	}

	print("example.cpp... running...");

	string infilename1(argv[1]);
	string otfilename1(argv[2]);

	ifstream infile1(infilename1);
	ofstream otfile1(otfilename1);

	int c = 0;
	string line1;

	while (getline(infile1, line1))
	{
		if(0 == (c % 100))
		{
			print("Processing line " + tostring(c) + "...");
		}
		++c;

		otfile1 << line1 << endl;
	}

	print("Number of lines copied:    " + tostring(c));
	print("example.cpp... done.");

	return 0;
}

