#include <iostream>
#include <libport/dirent.h>

#ifdef WIN32

struct DIR
{
  intptr_t handle;
  dirent entry;
  char* dir;
};

DIR* opendir(const char* name)
{
  DIR* res = (DIR*)malloc(sizeof (DIR));
  res->handle = NULL;
  res->dir = strdup(name);
  return res;
}

int closedir(DIR* dir)
{
  if (dir->handle)
    _findclose(dir->handle);
  free(dir->dir);
  return 0;
}

dirent* readdir(DIR *dir)
{
  int ret;
  _finddata_t fileinfo;

  if (dir->handle == NULL)
  {
    std::string pattern = dir->dir;
    pattern += "\\*";
    ret = dir->handle = _findfirst(pattern.c_str(), &fileinfo);
  }
  else
    ret = _findnext(dir->handle, &fileinfo);

  if (ret == -1)
    return NULL;
  strcpy(dir->entry.d_name, fileinfo.name);
  return &dir->entry;
}

#endif