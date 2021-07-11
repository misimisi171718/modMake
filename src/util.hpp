#pragma once

#include <filesystem>
#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

struct srcFiles
{
	std::vector<fs::path> modFiles;
	std::vector<fs::path> configFiles;
	std::vector<fs::path> configFolders;
	srcFiles& operator+=(const srcFiles& toAppend);
};

YAML::Node init(int argc,char* argv[]);
srcFiles getSrcFiles(const fs::path& location);
void createTarget(const std::string& name, const srcFiles& files, const YAML::Node& config);