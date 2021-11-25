#pragma once

#include <filesystem>
#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>
#include <spdlog/fmt/ostr.h>

#include "config.hpp"

namespace fs = std::filesystem;

struct srcFiles
{
	std::vector<fs::path> modFiles;
	std::vector<fs::path> configFiles;
	std::vector<fs::path> configFolders;
	srcFiles& operator+=(const srcFiles& toAppend);
};

std::ostream& operator<<(std::ostream& os, const srcFiles& f);

Configuration init(int argc,char* argv[]);
srcFiles getSrcFiles(const Configuration& location);
void createTarget(const Target& target, const srcFiles& files, const fs::path& outDir);
void createArchive(const Target& target, const srcFiles& files, const fs::path& outName);
srcFiles filterSrcs(const srcFiles& allsrcs, const std::vector<std::regex>& excludes);