#include "config.h"

// Include the correct filesystem library
#if defined(_WIN32)
	#include <Windows.h>
#else
	#include <dirent.h>
#endif

using namespace std;

#ifdef _WIN32 // If Windows OS (Uses Windows.h)
vector<string> getSASMfiles(string folder) {
	// code found here:
	// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c?rq=1
	vector<string> names;
	string search_path = folder + "/*.sasm";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do { // read all (real) files in current folder
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	if (names.size() == 0)
		throw std::exception("No Simcomp assembler files found in given directory");
	return names;
}

#else // Assumes UNIX-like OS otherwise (Uses dirent.h)
vector<string> getSASMfiles(string folder) {
	vector<string> names;
	string search_path = folder + "/";
	string filetype = ".sasm"; // Can be changed to accommodate diffrent filetypes

	DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(search_path.c_str())) == NULL) {
      return names; // Error opening directory
  }
  string name;
  while ((dirp = readdir(dp)) != NULL) {
      name = string(dirp->d_name);
      if (name.length() > filetype.length() && name.substr(name.length()-filetype.length(), name.length()) == filetype)
      	names.push_back(name);
  }
	return names;
}
#endif
