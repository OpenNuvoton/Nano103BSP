// mkrom.cpp : Defines the entry point for the console application.
//


#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const char string1[] = "\n\\htmlonly\r\n<html><table class=\"fixed\" border=\"1\" style=\"border-collapse:collapse;\" borderColor=black ><col width=\"75px\" /><col width=\"125px\" /><col width=\"700px\" /><caption align=\"left\"><font size=\"3\">";
const char string2[] = "</font><br><p> <font size=\"2\">";
const char string3[] = "</font></caption><thread><tr bgcolor=\"#8A0808\" ><td><font color=white><b>Bits</b></font></td><td><font color=white><b>Field</b></font></td><td><font color=white><b>Descriptions</b></font></td></tr></thread><tbody>";
const char string4[] = "<tr><td>";
const char string5[] = "</td><td>";
const char string6[] = "</td><td><div style=\"word-wrap: break-word;\"><b>";
const char string7[] = "</div></td></tr>";
const char string8[] = "</tbody></table></html>\r\n\\endhtmlonly\r\n";


int main(int argc, char* argv[])
{
	ifstream ifile;
	ofstream ofile;
	string line;    // current line we're processing
	int index = 0;
	int cell = 0;   // output cell
	int read = 1;  // no need to read in bit loop
	int end = 0;   // end of reg grop
	int linenum = 0;    // line number for debug only

#if 1
	if( argc < 2 )
	{
		printf("cannot open target file");
		return(-1);
	}


	ifile.open(argv[1]);
#else	
	ifile.open("C:\\work_zone\\mini51\\sandbox\\bsp\\_scripts\\conv\\Mini51Series.h");
#endif
	if (!ifile.is_open()) {
		cout << "cannot open selected file";
		return -1;
	} 

	ofile.open("temp.h", ios::trunc);

    if (!ofile.is_open()) {
		cout << "cannot open output file";
		return -1;
	}


	while (1) {
		if(read) {
			getline (ifile, line);
			if(ifile.rdstate() & std::ifstream::eofbit)
				break;
			linenum++;
		} else
			read = 1;

		//cout << "current line number is " << linenum << endl;
		if(line.compare(0, 12, "     * @var ") == 0) {  // found the first line of register definition
            std::size_t found = line.find("::");
			// output structure and register name
			for (unsigned i = 7; i < line.length(); ++i) {
				ofile << line.at(i);
			}
			// begin of html table...
			ofile << string1;
			// output register name into table, the 2 is to skip "::"
			for (unsigned i = found + 2; i < line.length(); ++i) {
				ofile << line.at(i);
			}
			ofile << string2 << endl;
			getline (ifile, line);  // get offset and description
			linenum++;
            // output offset and description
			for (unsigned i = 7; i < line.length(); ++i) {
				ofile << line.at(i);
			}
			ofile << string3 << endl;
			ofile.flush();
            //* ---------------------------------------------------------------------------------------------------
            //* |Bits    |Field     |Descriptions
            //* | :----: | :----:   | :---- |
			// above three lines we don't need
            for(int i = 0; i < 3; i++) {
                getline (ifile, line);
                linenum++;
                line.clear();
            }

			cell = 0;
			// get bit field
			while(1) {
				getline (ifile, line);
				linenum++;

				if(line.length() > 12) {
					if(line.compare(0, 12, "     * @var ") == 0) {	// end of current register entry
						read = 0;
						break;
					}
				} 
				
				if (line.compare(0, 7, "     */") == 0) {	// end of current register table comment block
					end = 1;
					break;
				} 
				
				if(line.at(8) == '[') { // begin of bit field, contains offset, name, and description
					std::string::iterator it=line.begin() + 8;
					if(cell == 1)
						ofile << string7;
					cell = 1;
					ofile << string4 << endl;
					ofile.flush();
					do{	
						ofile << *it++;		// offset
					} while(*it != ']');

					ofile << ']' << string5;
					while(*it != '|')
							it++;
					it++;
					do{	
						ofile << *it++;		// name
					} while(*it != '|' && *it != ' ');
					ofile << string6;
					while(*it != '|')
							it++;
					it++;
					do{	
						ofile << *it;		// first description line
					} while(++it!=line.end());
					ofile << "</b><br>" << endl;
					ofile.flush();
				} else { // register description
					for (unsigned i = 28; i<line.length(); ++i) {
						ofile << line.at(i);
					}
					ofile << "<br>" << endl;
					ofile.flush();
				}
			}
			ofile << string7;
			ofile << string8 << endl;
			if(end) {
				ofile << "\n */"  << endl;
				end = 0;
			} 

			// now all data line[] are useless, start from entry 0
			//line.clear();
			//getline (ifile, line);
			//linenum++;
		} else {
				ofile << line << endl;
		}
    }

	ifile.close();
	ofile.close();
	return 0;


}


