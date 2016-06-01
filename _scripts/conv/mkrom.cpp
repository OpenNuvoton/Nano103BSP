// mkrom.cpp : Defines the entry point for the console application.
//


#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const char string0[] = "/**\r\n\\htmlonly\r\n<html><table class=\"fixed\" border=\"1\" style=\"border-collapse:collapse;\" borderColor=black ><col width=\"75px\" /><col width=\"125px\" /><col width=\"700px\" /><caption align=\"left\"><font size=\"3\">";
const char string1[] = "</font><br><p> <font size=\"2\">";
const char string2[] = "</font></caption><thread><tr bgcolor=\"#8A0808\" ><td><font color=white><b>Bits</b></font></td><td><font color=white><b>Field</b></font></td><td><font color=white><b>Descriptions</b></font></td></tr></thread><tbody>";
const char string3[] = "<tr><td>";
const char string4[] = "</td><td>";
const char string5[] = "</td><td><div style=\"word-wrap: break-word;\"><b>";
const char string6[] = "</div></td></tr>";
const char string7[] = "</tbody></table></html>\r\n\\endhtmlonly\r\n*/";


int main(int argc, char* argv[])
{
	ifstream ifile;
	ofstream ofile;
	string line[4];
	int index = 0;
	int cell = 0; // output cell

	if( argc < 2 )
	{
		printf("cannot open target file");
		return(-1);
	}


	ifile.open(argv[1]);

	if (!ifile.is_open()) {
		cout << "cannot open selected file";
		return -1;
	}
	ofile.open("temp.h", ios::trunc);

    if (!ofile.is_open()) {
		cout << "cannot open output file";
		return -1;
	}

	// we bet "====================" won't start at the beging of input file
	getline (ifile, line[0]);
	index = 1;

	while (getline (ifile, line[index])) {
		
		if(line[index].compare("     * ===================================================================================================") == 0) {
			// find register definition. let's get to work!!

			// begin of html table...
			ofile << string0;
			// get register name, store in line[index - 1]
			for (unsigned i=7; i<line[(index + 4 - 1) & 0x3].length(); ++i) {
				ofile << line[(index + 4 - 1) & 0x3].at(i);
			}
			ofile << string1 << endl;
			getline (ifile, line[0]);  // get offset and description
			for (unsigned i=7; i<line[0].length(); ++i) {
				ofile << line[0].at(i);
			}
			ofile << string2 << endl;
			ofile.flush();
			// three more lines we don't need
			getline (ifile, line[0]);
			getline (ifile, line[0]);
			getline (ifile, line[0]);
			line[0].clear();
			line[1].clear();
			line[2].clear();
			line[3].clear();
			cell = 0;
			// get bit field
			while(1) {
				getline (ifile, line[0]);
				if(line[0].length() < 8) {	// if length is smaller than 8, cannot call at(8), will cause abort
					if(line[0].compare("    */") == 0) {	// end of current register table
						break;
					}
				} else if(line[0].at(8) == '[') { // begine of bit field, contains offset, name, and description
					std::string::iterator it=line[0].begin() + 8;
					if(cell == 1)
						ofile << string6;
					cell = 1;
					ofile << string3 << endl;
					ofile.flush();
					do{	
						ofile << *it++;		// offset
					} while(*it != ']');

					ofile << ']' << string4;
					while(*it != '|')
							it++;
					it++;
					do{	
						ofile << *it++;		// name
					} while(*it != '|' && *it != ' ');
					ofile << string5;
					while(*it != '|')
							it++;
					it++;
					do{	
						ofile << *it;		// first description line
					} while(++it!=line[0].end());
					ofile << "</b><br>" << endl;
					ofile.flush();
				} else { // register description
					for (unsigned i=28; i<line[0].length(); ++i) {
						ofile << line[0].at(i);
					}
					ofile << "<br>" << endl;
					ofile.flush();
				}
			}
			ofile << string6;
			ofile << string7 << endl;
			// now all data line[] are useless, start from entry 0
			line[0].clear();
			line[1].clear();
			line[2].clear();
			line[3].clear();
			getline (ifile, line[0]);
			index = 0;
		} else {
			//if(!line[(index + 4 - 2) & 0x3].empty())
				ofile << line[(index + 4 - 2) & 0x3] << endl;
		}
		index = (index + 1) & 0x3;	// goto next entry
    }

	// output remaining lines
	ofile << line[index + 4 - 2] << endl;
	ofile << line[index + 4 - 1] << endl;

	ifile.close();
	ofile.close();
	return 0;


}


