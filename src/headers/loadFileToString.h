#ifndef _LOADFILETOSTRING_HPP_INCLUDED
#define _LOADFILETOSTRING_HPP_INCLUDED

#include <fstream>
#include <string>

using std::fstream;
using std::string;

/**
 * @brief      Loads a file to string.
 *
 * @param[in]  fileName  The file name
 *
 * @return     a string containing the contents of the file, or an empty string if an error occured.
 */
inline string loadFileToString(const string &fileName) {

	fstream file(fileName);

	if(!file)
		return "";

	string fileAsString{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

	file.close();
	return fileAsString;
}

#endif