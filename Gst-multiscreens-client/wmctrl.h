#ifndef WMCTRL_H
#define WMCTRL_H


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>
#include <glib.h>

#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */
#define VERSION "1.07"

#define HELP "wmctrl " VERSION "\n" \
    "Usage: wmctrl [OPTION]...\n" \
    "Actions:\n" \
    "  -m                   Show information about the window manager and\n" \
    "                       about the environment.\n" \
    "  -l                   List windows managed by the window manager.\n" \
    "  -d                   List desktops. The current desktop is marked\n" \
    "                       with an asterisk.\n" \
    "  -s <DESK>            Switch to the specified desktop.\n" \
    "  -a <WIN>             Activate the window by switching to its desktop and\n" \
    "                       raising it.\n" \
    "  -c <WIN>             Close the window gracefully.\n" \
    "  -R <WIN>             Move the window to the current desktop and\n" \
    "                       activate it.\n" \
    "  -r <WIN> -t <DESK>   Move the window to the specified desktop.\n" \
    "  -r <WIN> -e <MVARG>  Resize and move the window around the desktop.\n" \
    "                       The format of the <MVARG> argument is described below.\n" \
    "  -r <WIN> -b <STARG>  Change the state of the window. Using this option it's\n" \
    "                       possible for example to make the window maximized,\n" \
    "                       minimized or fullscreen. The format of the <STARG>\n" \
    "                       argument and list of possible states is given below.\n" \
    "  -r <WIN> -N <STR>    Set the name (long title) of the window.\n" \
    "  -r <WIN> -I <STR>    Set the icon name (short title) of the window.\n" \
    "  -r <WIN> -T <STR>    Set both the name and the icon name of the window.\n" \
    "  -k (on|off)          Activate or deactivate window manager's\n" \
    "                       \"showing the desktop\" mode. Many window managers\n" \
    "                       do not implement this mode.\n" \
    "  -o <X>,<Y>           Change the viewport for the current desktop.\n" \
    "                       The X and Y values are separated with a comma.\n" \
    "                       They define the top left corner of the viewport.\n" \
    "                       The window manager may ignore the request.\n" \
    "  -n <NUM>             Change number of desktops.\n" \
    "                       The window manager may ignore the request.\n" \
    "  -g <W>,<H>           Change geometry (common size) of all desktops.\n" \
    "                       The window manager may ignore the request.\n" \
    "  -h                   Print help.\n" \
    "\n" \
    "Options:\n" \
    "  -i                   Interpret <WIN> as a numerical window ID.\n" \
    "  -p                   Include PIDs in the window list. Very few\n" \
    "                       X applications support this feature.\n" \
    "  -G                   Include geometry in the window list.\n" \
    "  -x                   Include WM_CLASS in the window list or\n" \
    "                       interpret <WIN> as the WM_CLASS name.\n" \
    "  -u                   Override auto-detection and force UTF-8 mode.\n" \
    "  -F                   Modifies the behavior of the window title matching\n" \
    "                       algorithm. It will match only the full window title\n" \
    "                       instead of a substring, when this option is used.\n" \
    "                       Furthermore it makes the matching case sensitive.\n" \
    "  -v                   Be verbose. Useful for debugging.\n" \
    "  -w <WA>              Use a workaround. The option may appear multiple\n" \
    "                       times. List of available workarounds is given below.\n" \
    "\n" \
    "Arguments:\n" \
    "  <WIN>                This argument specifies the window. By default it's\n" \
    "                       interpreted as a string. The string is matched\n" \
    "                       against the window titles and the first matching\n" \
    "                       window is used. The matching isn't case sensitive\n" \
    "                       and the string may appear in any position\n" \
    "                       of the title.\n" \
    "\n" \
    "                       The -i option may be used to interpret the argument\n" \
    "                       as a numerical window ID represented as a decimal\n" \
    "                       number. If it starts with \"0x\", then\n" \
    "                       it will be interpreted as a hexadecimal number.\n" \
    "\n" \
    "                       The -x option may be used to interpret the argument\n" \
    "                       as a string, which is matched against the window's\n" \
    "                       class name (WM_CLASS property). Th first matching\n" \
    "                       window is used. The matching isn't case sensitive\n" \
    "                       and the string may appear in any position\n" \
    "                       of the class name. So it's recommended to  always use\n" \
    "                       the -F option in conjunction with the -x option.\n" \
    "\n" \
    "                       The special string \":SELECT:\" (without the quotes)\n" \
    "                       may be used to instruct wmctrl to let you select the\n" \
    "                       window by clicking on it.\n" \
    "\n" \
    "                       The special string \":ACTIVE:\" (without the quotes)\n" \
    "                       may be used to instruct wmctrl to use the currently\n" \
    "                       active window for the action.\n" \
    "\n" \
    "  <DESK>               A desktop number. Desktops are counted from zero.\n" \
    "\n" \
    "  <MVARG>              Specifies a change to the position and size\n" \
    "                       of the window. The format of the argument is:\n" \
    "\n" \
    "                       <G>,<X>,<Y>,<W>,<H>\n" \
    "\n" \
    "                       <G>: Gravity specified as a number. The numbers are\n" \
    "                          defined in the EWMH specification. The value of\n" \
    "                          zero is particularly useful, it means \"use the\n" \
    "                          default gravity of the window\".\n" \
    "                       <X>,<Y>: Coordinates of new position of the window.\n" \
    "                       <W>,<H>: New width and height of the window.\n" \
    "\n" \
    "                       The value of -1 may appear in place of\n" \
    "                       any of the <X>, <Y>, <W> and <H> properties\n" \
    "                       to left the property unchanged.\n" \
    "\n" \
    "  <STARG>              Specifies a change to the state of the window\n" \
    "                       by the means of _NET_WM_STATE request.\n" \
    "                       This option allows two properties to be changed\n" \
    "                       simultaneously, specifically to allow both\n" \
    "                       horizontal and vertical maximization to be\n" \
    "                       altered together.\n" \
    "\n" \
    "                       The format of the argument is:\n" \
    "\n" \
    "                       (remove|add|toggle),<PROP1>[,<PROP2>]\n" \
    "\n" \
    "                       The EWMH specification defines the\n" \
    "                       following properties:\n" \
    "\n" \
    "                           modal, sticky, maximized_vert, maximized_horz,\n" \
    "                           shaded, skip_taskbar, skip_pager, hidden,\n" \
    "                           fullscreen, above, below\n" \
    "\n" \
    "Workarounds:\n" \
    "\n" \
    "  DESKTOP_TITLES_INVALID_UTF8      Print non-ASCII desktop titles correctly\n" \
    "                                   when using Window Maker.\n" \
    "\n" \
    "The format of the window list:\n" \
    "\n" \
    "  <window ID> <desktop ID> <client machine> <window title>\n" \
    "\n" \
    "The format of the desktop list:\n" \
    "\n" \
    "  <desktop ID> [-*] <geometry> <viewport> <workarea> <title>\n" \
    "\n" \
    "\n" \
    "Author, current maintainer: Tomas Styblo <tripie@cpan.org>\n" \
    "Released under the GNU General Public License.\n" \
    "Copyright (C) 2003\n"
/* }}} */


#define MAX_PROPERTY_VALUE_LEN 4096
#define SELECT_WINDOW_MAGIC ":SELECT:"
#define ACTIVE_WINDOW_MAGIC ":ACTIVE:"

#define p_verbose(...) if (options.verbose) { \
    fprintf(stderr, __VA_ARGS__); \
    }

char* getWinList();

/* declarations of static functions *//*{{{*/
static gboolean wm_supports (Display *disp, const gchar *prop);
static Window *get_client_list (Display *disp, unsigned long *size);
static int client_msg(Display *disp, Window win, char *msg,
                      unsigned long data0, unsigned long data1,
                      unsigned long data2, unsigned long data3,
                      unsigned long data4);
static int list_windows (Display *disp);
static int list_desktops (Display *disp);
static int showing_desktop (Display *disp);
static int change_viewport (Display *disp);
static int change_geometry (Display *disp);
static int change_number_of_desktops (Display *disp);
static int switch_desktop (Display *disp);
static int wm_info (Display *disp);
static gchar *get_output_str (gchar *str, gboolean is_utf8);
static int action_window (Display *disp, Window win, char mode);
static int action_window_pid (Display *disp, char mode);
static int action_window_str (Display *disp, char mode);
static int activate_window (Display *disp, Window win,
                            gboolean switch_desktop);
static int close_window (Display *disp, Window win);
static int longest_str (gchar **strv);
static int window_to_desktop (Display *disp, Window win, int desktop);
static void window_set_title (Display *disp, Window win, char *str, char mode);
static gchar *get_window_title (Display *disp, Window win);
static gchar *get_window_class (Display *disp, Window win);
static gchar *get_property (Display *disp, Window win,
                            Atom xa_prop_type, gchar *prop_name, unsigned long *size);
static void init_charset(void);
static int window_move_resize (Display *disp, Window win, char *arg);
static int window_state (Display *disp, Window win, char *arg);
static Window Select_Window(Display *dpy);
static Window get_active_window(Display *dpy);

/*}}}*/

static struct {
    int verbose;
    int force_utf8;
    int show_class;
    int show_pid;
    int show_geometry;
    int match_by_id;
    int match_by_cls;
    int full_window_title_match;
    int wa_desktop_titles_invalid_utf8;
    char *param_window;
    char *param;
} options;

static gboolean envir_utf8;

#endif // WMCTRL_H
