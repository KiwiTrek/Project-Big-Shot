#ifndef __MODULE_FILESYSTEM_H__
#define __MODULE_FILESYSTEM_H__

#include "Module.h"
#include <vector>
#include <string>

struct SDL_RWops;
int close_sdl_rwops(SDL_RWops* rw);

struct aiFileIO;

//struct BASS_FILEPROCS;
class Config;
struct PathNode
{
	PathNode() : path("") {};
	//Path in reference to the first PathNode
	std::string path;
	//Folder / file itself
	std::string localPath;

	std::vector<PathNode> children;

	bool isLeaf = true;
	bool isFile = true;

	bool IsLastFolder() const
	{
		for (uint i = 0; i < children.size(); i++)
		{
			if (children[i].isFile == false) return false;
		}
		return true;
	}

	bool operator ==(const PathNode node) const { return path == node.path; }

	void RemoveFoldersMetas()
	{
		for (uint f = 0; f < children.size(); ++f)
		{
			//If the current node is a folder, remove its meta file (if any)
			//and recursively search through the folder content
			if (!children[f].isFile)
			{
				for (uint m = 0; m < children.size(); ++m)
				{
					if (children[m].path == children[f].path + ".meta") children.erase(children.begin() + m);
				}
				children[f].RemoveFoldersMetas();
			}
		}
	}
};

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(Application* app, bool start_enabled = true);// const char* game_path = nullptr);

	// Destructor
	~ModuleFileSystem();

	// Called before render is available
	bool Init() override;

	// Called before quitting
	bool CleanUp() override;

	void CreateLibraryDirectories();

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) const;
	const char* GetWriteDir() const;
	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;
	void GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) const;
	void GetRealDir(std::string path, std::string& output) const;
	std::string GetPathRelativeToAssets(const char* originalPath) const;

	bool Read(const std::string& path, void* data, unsigned size) const; //reads from path and allocates in data. NOTE: The caller should be responsible to clean it
	bool Exists(const std::string& path) const;
	unsigned Size(const std::string& path) const;

	bool HasExtension(const char* path) const;
	bool HasExtension(const char* path, std::string extension) const;
	bool HasExtension(const char* path, std::vector<std::string> extensions) const;

	std::string NormalizePath(const char* path) const;
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;

	bool DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath);
	bool DuplicateFile(const char* srcFile, const char* dstFile);

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;

	std::string GetUniqueName(const char* path, const char* name) const;

	std::string SetNormalName(const char* path);

	std::string systemBasePath;
	std::string texturePath;
	std::string meshPath;
	std::string scenePath;
};

#endif // __MODULE_FILESYSTEM_H__