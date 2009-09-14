/*
 * Copyright (C) 2009, Gostai S.A.S.
 *
 * This software is provided "as is" without warranty of any kind,
 * either expressed or implied, including but not limited to the
 * implied warranties of fitness for a particular purpose.
 *
 * See the LICENSE file for more information.
 */
#include <libport/cassert>

#include <libport/program-name.hh>

namespace libport
{
  static std::string program_name_;
  static cli_args_type program_arguments_;
  static bool program_initialized_;

  void program_initialize(int argc, char** argv)
  {
    assert(argc >= 1);
    program_initialize(cli_args_type(argv, argv + argc));
  }

  void program_initialize(const cli_args_type& args)
  {
    assert(!program_initialized_);
    assert(args.size() >= 1);
    program_name_ = args[0];
    program_arguments_.insert(program_arguments_.end(),
                              ++args.begin(), args.end());
    program_initialized_ = true;
  }

  void program_initialize(const std::string& program_name)
  {
    cli_args_type args;
    args.push_back(program_name);
    program_initialize(args);
  }

  const std::string& program_name()
  {
    assert(program_initialized_);
    return program_name_;
  }

  const cli_args_type& program_arguments()
  {
    assert(program_initialized_);
    return program_arguments_;
  }

}
