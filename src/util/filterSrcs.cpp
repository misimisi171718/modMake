
#include "../util.hpp"
#include "../config.hpp"

static std::vector<fs::path> filterVec(const std::regex& re, const std::vector<fs::path>& paths)
{
	std::vector<fs::path> ret;
	ret.reserve(paths.size());
	for (auto &file : paths)
	{
		if(!std::regex_match(file.string(),re))
			ret.push_back(file);
	}
	ret.shrink_to_fit();
	return ret;
}

srcFiles filterSrcs(const srcFiles& allsrcs, const std::vector<std::regex>& excludes)
{
	srcFiles ret;
	for (auto &exclude : excludes)
	{
		ret.modFiles = filterVec(exclude,allsrcs.modFiles);
		ret.configFiles = filterVec(exclude,allsrcs.configFiles);
		ret.configFolders = filterVec(exclude,allsrcs.configFolders);
	}
	return ret;
}