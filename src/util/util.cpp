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

std::ostream& operator<<(std::ostream& os, const srcFiles& f)
{
	os << "mod files [ \n";
	for (size_t i = 0; i < f.modFiles.size(); i++)
	{
		os <<  fmt::format("    {},\n",f.modFiles[i]);
	}
	os << " ]\n";
	os << "config files [ \n";
	for (size_t i = 0; i < f.configFiles.size(); i++)
	{
		os <<  fmt::format("    {},\n",f.configFiles[i]);
	}
	os << " ]\n";
	os << "config folders [ \n";
	for (size_t i = 0; i < f.configFolders.size(); i++)
	{
		os <<  fmt::format("    {},\n",f.configFolders[i]);
	}
	os << " ]\n";
	return os;
}
