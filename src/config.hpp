#pragma once

#include <filesystem>
#include <regex>
#include <vector>
#include <string>

namespace fs = std::filesystem;

struct Target
{
	std::vector<std::regex> excludes;
	std::string name;
	bool makeFolder;
	bool makeArchive;
};


struct Configuration
{
	fs::path srcDir;
	fs::path outDir;
	std::string version;
	std::vector<Target> targets;
	std::vector<std::regex> configPatterns;
};
