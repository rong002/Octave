/*

Copyright (C) 1993-2012 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

// Use the GNU readline library for command line editing and hisory.

#if !defined (octave_input_h)
#define octave_input_h 1

#include <cstdio>

#include <string>

#include "oct-time.h"
#include "oct-obj.h"
#include "pager.h"

class octave_value;

extern OCTINTERP_API FILE *get_input_from_stdin (void);

// TRUE means that input is coming from a file that was named on
// the command line.
extern bool input_from_command_line_file;

// TRUE means that stdin is a terminal, not a pipe or redirected file.
extern bool stdin_is_tty;

// Simple name of function file we are reading.
extern std::string curr_fcn_file_name;

// Full name of file we are reading.
extern std::string curr_fcn_file_full_name;

// Fix things up so that input can come from file 'name', printing a
// warning if the file doesn't exist.

// TRUE means this is an interactive shell.
extern bool interactive;

// TRUE means the user forced this shell to be interactive (-i).
extern bool forced_interactive;

// Should we issue a prompt?
extern int promptflag;

// A line of input.
extern std::string current_input_line;

// TRUE after a call to completion_matches.
extern bool octave_completion_matches_called;

// TRUE if the plotting system has requested a call to drawnow at
// the next user prompt.
extern OCTINTERP_API bool Vdrawnow_requested;

// TRUE if we are in debugging mode.
extern OCTINTERP_API bool Vdebugging;

extern void initialize_command_input (void);

extern bool octave_yes_or_no (const std::string& prompt);

extern octave_value do_keyboard (const octave_value_list& args = octave_value_list ());

extern std::string VPS4;

extern char Vfilemarker;

enum echo_state
{
  ECHO_OFF = 0,
  ECHO_SCRIPTS = 1,
  ECHO_FUNCTIONS = 2,
  ECHO_CMD_LINE = 4
};

extern int Vecho_executing_commands;

extern octave_time Vlast_prompt_time;

class
octave_base_reader
{
public:

  friend class octave_input_reader;

  octave_base_reader (void) : count (1) { }

  octave_base_reader (const octave_base_reader&) : count (1) { }

  virtual ~octave_base_reader (void) { }

  virtual std::string get_input (bool& eof) = 0;

  virtual std::string input_source (void) const { return in_src; }

  std::string octave_gets (bool& eof);

private:

  int count;

  static const std::string in_src;
};

class
octave_terminal_reader : public octave_base_reader
{
public:

  octave_terminal_reader (void) : octave_base_reader () { }

  std::string get_input (bool& eof);

  std::string input_source (void) const { return in_src; }

private:

  static const std::string in_src;
};

class
octave_file_reader : public octave_base_reader
{
public:

  octave_file_reader (FILE *f_arg)
    : octave_base_reader (), file (f_arg) { }

  std::string get_input (bool& eof);

  std::string input_source (void) const { return in_src; }

private:

  FILE *file;

  static const std::string in_src;
};

class
octave_eval_string_reader : public octave_base_reader
{
public:

  octave_eval_string_reader (const std::string& str)
    : octave_base_reader (), eval_string (str)
  { }

  std::string get_input (bool& eof);

  std::string input_source (void) const { return in_src; }

private:

  std::string eval_string;

  static const std::string in_src;
};

class
octave_input_reader
{
public:
  octave_input_reader (void)
    : rep (new octave_terminal_reader ())
  { }

  octave_input_reader (FILE *file)
    : rep (new octave_file_reader (file))
  { }

  octave_input_reader (const std::string& str)
    : rep (new octave_eval_string_reader (str))
  { }

  octave_input_reader (const octave_input_reader& ir)
  {
    rep = ir.rep;
    rep->count++;
  }

  octave_input_reader& operator = (const octave_input_reader& ir)
  {
    if (&ir != this)
      {
        rep = ir.rep;
        rep->count++;
      }

    return *this;
  }

  ~octave_input_reader (void)
  {
    if (--rep->count == 0)
      delete rep;
  }

  std::string get_input (bool& eof)
  {
    return rep->get_input (eof);
  }

  std::string input_source (void) const
  {
    return rep->input_source ();
  }

private:

  octave_base_reader *rep;
};

#endif
