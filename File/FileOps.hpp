#pragma once

#include <fstream>
#include <iostream>
#include <filesystem>

#include "Filepath.hpp"

// stream string operations derived from:
// Optimized C++ by Kurt Guntheroth (O’Reilly).
// Copyright 2016 Kurt Guntheroth, 978-1-491-92206-4
// Updated to c++17 by Benjamin Edwards, 2020

namespace Avokii::FileOps
{
	bool StreamReadString( std::istream& file, String& out_file_contents );

	bool ReadFile( const Filepath& filename, String& out_file_contents );
	inline bool ReadFile( StringView filename, String& out_file_contents ) { return ReadFile( Filepath{ filename }, out_file_contents ); }

	std::vector<String> GetFilesInFolder( const Filepath& path );
	inline std::vector<String> GetFilesInFolder( StringView path ) { return GetFilesInFolder( Filepath{ path } ); }

	Filepath GetFileDirectory( const Filepath& filepath, bool relative_to_working_directory = true );
	inline Filepath GetFileDirectory( StringView filepath, bool relative_to_working_directory = true ) { return GetFileDirectory( Filepath{ filepath }, relative_to_working_directory ); }
}
