#pragma once
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

class FileScanner {
public:
	void run();
	void join();
	void addTVShowsSourcePath(const bfs::path& sourcePath);
	void addMoviesSourcePath(const bfs::path& sourcePath);
};
