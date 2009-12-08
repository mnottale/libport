/*
 * Copyright (C) 2009, Gostai S.A.S.
 *
 * This software is provided "as is" without warranty of any kind,
 * either expressed or implied, including but not limited to the
 * implied warranties of fitness for a particular purpose.
 *
 * See the LICENSE file for more information.
 */
/**
 ** \file libport/path.cc
 ** \brief Implement libport::path.
 */

#include <iostream>
#include <libport/cstdio>
#include <libport/exception.hh>
#include <libport/fcntl.h>
#include <libport/unistd.h>
#include <libport/sys/stat.h>
#include <libport/sys/types.h>
#include <cctype>

#include <libport/contract.hh>
#include <libport/debug.hh>
#include <libport/detect-win32.h>
#include <libport/finally.hh>
#include <libport/foreach.hh>
#include <libport/path.hh>
#include <libport/tokenizer.hh>

GD_ADD_CATEGORY(path);


// Implementation detail: if path_ is empty and absolute_ is false,
// then the path is '.'.

namespace libport
{
  path::path(const std::string& p)
  {
    init(p);
  }

  path::path(const char* p)
  {
    init(p);
  }

  void
  path::init(std::string p)
  {
    if (p.empty())
      throw invalid_path("Path can't be empty.");

    // Compute volume_ and absolute_.
#if defined WIN32
    // Under Win32, absolute paths start with a letter followed by
    // ":\". If the trailing slash is missing, this is a relative
    // path.
    if (2 <= p.length()
	&& isalpha(p[0]) && p[1] == ':')
    {
      volume_ = p.substr(0, 2);
      absolute_ = p.length() >= 3 && (p[2] == '\\' || p[2] == '/');
      p.erase(0, absolute_ ? 3 : 2);
    }
    // Network share, such as "\\shared volume\foo\bar"
    else if (3 <= p.length()
	     && p[0] == '\\' && p[1] == '\\')
    {
      absolute_ = true;
      std::string::size_type pos = p.find('\\', 3);
      if (pos == std::string::npos)
      {
	volume_ = p;
	p.clear();
      }
      else
      {
	volume_ = p.substr(0, pos);
	p = p.erase(0, pos);
      }
    }
    // Fallback to Unix cases for subsystems such as cygwin
    else
#endif // WIN32
    if (!p.empty() && (p[0] == '/' || p[0] == separator_))
    {
      absolute_ = true;
      p = p.erase(0, 0);
    }
    else
      absolute_ = false;


    // Cut directories on / and \.
    foreach (const std::string& component,
             make_tokenizer(p, WIN32_IF("/\\", "/")))
      append_dir(component);
  }

  path&
  path::operator=(const path& rhs)
  {
    absolute_ = rhs.absolute_;
    path_ = rhs.path_;
#if defined WIN32
    volume_ = rhs.volume_;
#endif
    return *this;
  }

  path&
  path::operator/=(const path& rhs)
  {
    if (rhs.absolute_get())
      throw invalid_path(
        "Rhs of concatenation is absolute: " + rhs.to_string());
#ifdef WIN32
    if (!rhs.volume_.empty())
      throw invalid_path("concatenation of path with volume: " +
			 rhs.to_string());
#endif
    for (path_type::const_iterator dir = rhs.path_.begin();
	 dir != rhs.path_.end();
	 ++dir)
      append_dir(*dir);

    return *this;
  }

  path
  path::operator/(const path& rhs) const
  {
    path res = *this;
    return res /= rhs;
  }

  path::operator std::string() const
  {
    return to_string();
  }

  std::string
  path::to_string() const
  {
    if (!absolute_ && path_.empty())
      return WIN32_IF(volume_.empty() ? "." : volume_, ".");

    std::string res = WIN32_IF(volume_, "");
    if (absolute_)
      res += separator_;
    bool tail = false;
    foreach (const std::string& dir, path_)
    {
      if (tail++)
	res += separator_;
      res += dir;
    }
    return res;
  }

  bool
  path::operator==(const path& rhs) const
  {
    return (volume_get() == rhs.volume_get()
            && absolute_get() == rhs.absolute_get()
            && components() == rhs.components());
  }

  std::ostream&
  path::dump(std::ostream& o) const
  {
    return o << operator std::string();
  }

  void
  path::append_dir(const std::string& dir)
  {
    precondition(dir.find(separator_) == std::string::npos);

    if (!dir.empty() && dir != ".")
    {
      // The following does not work properly with symlinks.  One
      // cannot expect that removing "foo/../" inside a valid path
      // results in the same path.  We should check whether "foo" is
      // *not* a symlink before playing this kind of trick.
#if 0
      if (dir == ".."
          && !path_.empty() && *path_.rbegin() != "..")
        path_.pop_back();
      else
#endif
	path_.push_back(dir);
    }
  }


  std::string
  path::basename() const
  {
    // basename of / is /, basename of . is .
    if (path_.empty())
      return *this;

    return path_.back();
  }

  std::string
  path::extension() const
  {
    std::string name = basename();
    size_t pos = name.rfind(".");

    if (pos == std::string::npos || pos == name.size() - 1)
      return "";

    return name.substr(pos + 1);
  }

  path
  path::dirname() const
  {
    // dirname of / is /, dirname of . is .
    if (path_.empty())
      return *this;

    std::string last = path_.back();
    // The const cast here is justified since the modification is
    // systematically reverted
    const_cast<path*>(this)->path_.pop_back();
    libport::Finally finally(
      boost::bind(&path_type::push_back,
		  boost::ref(const_cast<path*>(this)->path_), last));
    path res = path_.empty() ? "." : *this;
    return res;
  }

  bool path::exists() const
  {
    struct stat buf;
    bool res = 0 == stat(to_string().c_str(), &buf);
    GD_CATEGORY(path);
    GD_FINFO_DUMP("exists: %-5s: %s",
                  ((res ? "true" : "false"))
                  ((to_string())));
    return res;
  }

  bool path::create() const
  {
    int fd = open(to_string().c_str(),
                  O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1)
      return false;
    close(fd);
    return true;
  }

#ifndef WIN32
  path
  path::temporary_file()
  {
    char tmpfile[] = "/tmp/tmp.XXXXXX";
    int fd = mkstemp(tmpfile);
    close (fd);
    return path(tmpfile);
  }
#else
  // Implementation pieces for windows

  // // Get the path to the temporary folder
  // char tmp_path[MAX_PATH];
  // memset (tmp_path, 0, MAX_PATH);
  // GetTempPath(MAX_PATH, tmp_path);
  // char tmpfile[MAX_PATH];
  // memset (tmpfile, 0, MAX_PATH);
  // GetTempFileName (tmp_path, "", 0, tmpfile);
#endif

  void
  path::remove() const
  {
    if (unlink(to_string().c_str()))
      throw Exception(libport::format("unable to unlink file: %s", *this));
  }

  void
  path::rename(const std::string& dst)
  {
    if (::rename(to_string().c_str(), dst.c_str()))
      throw Exception(libport::format("unable to rename file: %s", *this));
    else
      *this = dst;
  }

  const path::path_type& path::components() const
  {
    return path_;
  }
}
