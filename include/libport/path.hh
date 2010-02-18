/*
 * Copyright (C) 2009, 2010, Gostai S.A.S.
 *
 * This software is provided "as is" without warranty of any kind,
 * either expressed or implied, including but not limited to the
 * implied warranties of fitness for a particular purpose.
 *
 * See the LICENSE file for more information.
 */

/**
 ** \file libport/path.hh
 ** \brief Declaration of libport::path.
 */

#ifndef LIBPORT_PATH_HH
# define LIBPORT_PATH_HH

# include <string>
# include <list>
# include <libport/export.hh>
# include <libport/detect-win32.h>
# include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace libport
{
  /** \brief Paths in filesystems, i.e., file names.
   **/
  class LIBPORT_API path
  {
  public:
    /// Exception thrown on invalid path
    class LIBPORT_API invalid_path : public std::exception
    {
    public:
      invalid_path(const std::string& msg);
      const char* what() const throw ();
      virtual ~invalid_path() throw ();
    private:
      std::string msg_;
    };

    /// \name Constructors.
    /// \{

    /// Init object with \a path.
    /** @throw invalid_path if \a p isn't a valid path
     */
    path(const std::string& p);

    /** @throw invalid_path if \a p isn't a valid path
     */
    path(const char* p);
    /// \}

    /// \name Operations on path.
    /// \{
#ifdef SWIG
    %rename (assign) operator=(const path& rhs);
#endif
    path& operator=(const path& rhs);
    /** @throw invalid_path if \a rhs is absolute.
     */
    path& operator/=(const path& rhs);
    /** @throw invalid_path if \a rhs is absolute.
     */
    path operator/(const path& rhs) const;
    bool operator==(const path& rhs) const;

    std::string basename() const;
    /// The extension of the file, or "" if not applicable.
    std::string extension() const;
    /// Return the volume.  Empty, unless on windows, in which
    /// case it looks like "c:".
    std::string volume_get() const;
    void volume_set(const std::string& v);
    path dirname() const;
    bool exists() const;
    /// Remove the file.
    /// \return true iff no error.
    void remove() const;
    bool create() const;
    void rename(const std::string& dst);
    /// \}

#ifndef WIN32
    static path temporary_file();
#endif

    /// \name Printing and converting.
    /// \{
#ifdef SWIG
    %rename (__str__) operator std::string() const;
#endif
    /// path is represented with a list of directories.
    typedef std::list<std::string> path_type;
    std::string to_string() const;
    operator std::string() const;
    const path_type& components() const;
    std::ostream& dump(std::ostream& ostr) const;
    /// \}

    /// Whether is absolute.
    bool absolute_get() const;

    /// Get boost::filesystem::path object.
    fs::path& get_boostpath();

  private:

    /// Append a single directory \a dir.
    void append_dir(const std::string& dir);

    /// Init object with path \a p.
    void init();

    /// Reset object to empty, and then init
    /// to path \a p
    void reset(const std::string& p);

    /// Boost Object.
    fs::path boost_path_;

    /// Represented path.
    path_type path_;

#ifdef WIN32
    /// Path separator
    static const char separator_ = '\\';

    /// Name of the volume (C:) for absolute paths under windows.
    std::string volume_;
#else
    /// Path separator
    static const char separator_ = '/';
#endif
  };

  /// Dump \a p on \a o.
  std::ostream& operator<<(std::ostream& o, const path& p);
}

# include <libport/path.hxx>

#endif // !LIBPORT_PATH_HH
