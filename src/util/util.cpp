#include "../util.hpp"

srcFiles& srcFiles::operator+=(const srcFiles& toAppend)
{
	modFiles.insert(
		modFiles.end(),
		toAppend.modFiles.begin(),
		toAppend.modFiles.end()
	);
	configFiles.insert(
		configFiles.end(),
		toAppend.configFiles.begin(),
		toAppend.configFiles.end()
	);
	configFolders.insert(
		configFolders.end(),
		toAppend.configFolders.begin(),
		toAppend.configFolders.end()
	);
	return *this;
}