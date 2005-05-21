
/******************************************************************************
* MODULE     : tm_scheme.cpp
* DESCRIPTION: The TeXmacs-lisp motor
* COPYRIGHT  : (C) 1999  Joris van der Hoeven
*******************************************************************************
* This software falls under the GNU general public license and comes WITHOUT
* ANY WARRANTY WHATSOEVER. See the file $TEXMACS_PATH/LICENSE for more details.
* If you don't have this file, write to the Free Software Foundation, Inc.,
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
******************************************************************************/

#include "tm_scheme.hpp"
#include "convert.hpp"
#include "file.hpp"

/******************************************************************************
* Constructor and destructor
******************************************************************************/

tm_scheme_rep::tm_scheme_rep () { dialogue_win= NULL; }
tm_scheme_rep::~tm_scheme_rep () {}

/******************************************************************************
* Execution of commands
******************************************************************************/

bool
tm_scheme_rep::exec_file (url u) {
  object ret= eval_file (materialize (u));
  return ret != object ("#<unspecified>");
}

string
tm_scheme_rep::preference (string var) {
  return as_string (call ("get-preference", var));
}

/******************************************************************************
* Delayed execution of commands
******************************************************************************/

void
tm_scheme_rep::exec_delayed (object cmd) {
  cmds << cmd;
}

void
tm_scheme_rep::exec_pending_commands () {
  array<object> a= cmds;
  cmds= array<object> (0);
  int i, n= N(a);
  for (i=0; i<n; i++) {
    object obj= call (a[i]);
    if (is_bool (obj) && !as_bool (obj))
      cmds << a[i];
  }
}

/******************************************************************************
* Dialogues
******************************************************************************/

class dialogue_command_rep: public command_rep {
  server_rep* sv;
  object      fun;
public:
  dialogue_command_rep (server_rep* sv2, object fun2):
    sv (sv2), fun (fun2) {}
  void apply () {
    string s_arg;
    sv->dialogue_inquire (s_arg);
    if (s_arg != "cancel") {
      object arg= string_to_object (s_arg);
      object cmd= scheme_cmd (cons (fun, cons (arg, null_object ())));
      sv->exec_delayed (cmd);
    }
    sv->exec_delayed (scheme_cmd ("(dialogue-end)")); }
  ostream& print (ostream& out) {
    return out << "Dialogue"; }
};

command
dialogue_command (server_rep* sv, object fun) {
  return new dialogue_command_rep (sv, fun);
}

void
tm_scheme_rep::dialogue_start (string name, widget wid) {
  if (dialogue_win == NULL) {
    string lan= get_display()->out_lan;
    if (lan == "russian") lan= "english";
    name= get_display()->translate (name, "english", lan);
    char* _name= as_charp (name);
    dialogue_wid= wid;
    dialogue_win= plain_window (dialogue_wid, _name);
    dialogue_win->map ();
    delete[] _name;
  }
}

void
tm_scheme_rep::dialogue_inquire (string& arg) {
  dialogue_wid << get_string ("input", arg);
}

void
tm_scheme_rep::dialogue_end () {
  if (dialogue_win != NULL) {
    dialogue_win->unmap ();
    delete dialogue_win;
    dialogue_win= NULL;
    dialogue_wid= widget ();
  }
}

static int
gcd (int i, int j) {
  if (i<j)  return gcd (j, i);
  if (j==0) return i;
  return gcd (j, i%j);
}

void
tm_scheme_rep::choose_file (object fun, string title, string type) {
  string magn;
  if (type == "image") {
    tm_widget meta = get_meta ();
    editor ed      = get_editor ();
    int dpi        = as_int (ed->get_env_string (DPI));
    int sfactor    = meta->get_shrinking_factor ();
    int num        = 75*sfactor;
    int den        = dpi;
    int g          = gcd (num, den);
    num /= g; den /= g;
    if (num != 1) magn << "*" << as_string (num);
    if (den != 1) magn << "/" << as_string (den);
  }

  url     name= get_name_buffer ();
  command cb  = dialogue_command (get_server(), fun);
  widget  wid = file_chooser_widget (cb, type, magn);
  if (!is_without_name (name)) {
    wid << set_string ("directory", as_string (head (name)));
    if ((type != "image") && (type != "")) {
      url u= tail (name);
      string old_suf= suffix (u);
      string new_suf= format_to_suffix (type);
      if ((suffix_to_format (suffix (u)) != type) &&
	  (old_suf != "") && (new_suf != ""))
	{
	  u= unglue (u, N(old_suf) + 1);
	  u= glue (u, "." * new_suf);
	}
      wid << set_string ("file", as_string (u));
    }
  }
  else wid << set_string ("directory", ".");
  dialogue_start (title, wid);
  if (type == "directory")
    dialogue_win->set_keyboard_focus (dialogue_wid[0]["directory"]["input"]);
  else dialogue_win->set_keyboard_focus (dialogue_wid[0]["file"]["input"]);
}

/******************************************************************************
* Interactive commands
******************************************************************************/

class interactive_command_rep: public command_rep {
  server_rep*   sv;   // the underlying server
  tm_widget     wid;  // the underlying TeXmacs window
  object        fun;  // the function which is applied to the arguments
  scheme_tree   p;    // the interactive arguments
  int           i;    // counter where we are
  array<string> s;    // feedback from interaction with user

public:
  interactive_command_rep (
    server_rep* sv2, tm_widget wid2, object fun2, scheme_tree p2):
      sv (sv2), wid (wid2), fun (fun2), p (p2), i (0), s (N(p)) {}
  void apply ();
  ostream& print (ostream& out) {
    return out << "interactive command " << p; }
};

void
interactive_command_rep::apply () {
  if ((i>0) && (s[i-1] == "cancel")) return;
  if (i == arity (p)) {
    array<object> params(N(p));
    for (i=0; i<N(p); i++) {
      params[i]= object (unquote (s[i]));
      call ("learn-interactive-arg", fun, object (i), params[i]);
    }
    string ret= object_to_string (call (fun, params));
    if (ret != "" && ret != "<unspecified>" && ret != "#<unspecified>")
      sv->set_message (ret, "interactive command");
  }
  else {
    string prompt, type;
    array<string> defs;
    if (is_atomic (p[i])) {
      if ((!is_atomic (p[i])) || (!is_quoted (p[i]->label))) return;
      prompt= unquote (p[i]->label);
      type  = "string";
    }
    else {
      int j;
      array<string> a (N(p[i]));
      if (N(p[i]) < 2) return;
      for (j=0; j<N(p[i]); j++) {
	if ((!is_atomic (p[i][j])) || (!is_quoted (p[i][j]->label))) return;
	if (j == 0) prompt= unquote (p[i][j]->label);
	else if (j == 1) type= unquote (p[i][j]->label);
	else defs << unquote (p[i][j]->label);
      }
    }
    s[i]= string ("");
    wid->interactive (prompt, type, defs, s[i], this);
    i++;
  }
}

void
tm_scheme_rep::interactive (object fun, scheme_tree p) {
  if (!is_tuple (p))
    fatal_error ("tuple expected", "edit_interface_rep::interactive");
  if (get_meta () -> get_footer_mode () == 1) beep ();
  else {
    command interactive_cmd=
      new interactive_command_rep (this, get_meta (), fun, p);
    interactive_cmd ();
  }
}
