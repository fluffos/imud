#define LOG_DIR "/log"
#define LOG_FILE_CATCH    (LOG_DIR "/log_catch")
#define LOG_FILE_RUNTIME  (LOG_DIR "/log")
#define LOG_FILE_COMPILE  (LOG_DIR "/compile")

void create() {
  string txt = "\n ---- " + ctime(time()) + "----\n";

  write_file(LOG_FILE_CATCH, txt);
  write_file(LOG_FILE_RUNTIME, txt);
  write_file(LOG_FILE_COMPILE, txt);
}

private int different(string fn, string pr)
{
  sscanf(fn, "%s#%*d", fn);
  fn += ".c";
  return (fn != pr) && (fn != ("/" + pr));
}

private string trace_line(object obj, string prog, string file, int line)
{
  string ret;
  string objfn = obj ? file_name(obj) : "<none>";

  ret = objfn;
  if (different(objfn, prog))
    ret += sprintf(" (%s)", prog);
  if (file != prog)
    ret += sprintf(" at %s:%d\n", file, line);
  else
    ret += sprintf(" at line %d\n", line);
  return ret;
}

private varargs string standard_trace(mapping mp, int flag)
{
string ret;
mapping *trace;
int i,n;

ret = ctime(time());
ret += "\n";
ret += mp["error"] + "Object: " + trace_line(mp["object"], mp["program"],
    mp["file"], mp["line"]);
ret += "\n";
trace = mp["trace"];

n = sizeof(trace);

for (i=0; i<n; i++)
{
if (flag) ret += sprintf("#%d: ", i);

ret += sprintf("'%s' at %s", trace[i]["function"],
trace_line(trace[i]["object"], trace[i]["program"],
    trace[i]["file"], trace[i]["line"]));
}
return ret;
}

string error_handler(mapping mp, int caught)
{
  string logfile = (caught ? LOG_FILE_CATCH : LOG_FILE_RUNTIME);
  string what = mp["error"];
  string userid;
  string ret = "---\n" + standard_trace(mp);
  write_file(logfile, ret);

/* If an object didn't load, they get compile errors.  Don't spam
   or confuse them */
  if (what[0..23] == "*Error in loading object")
    return 0;

  return 0;
}

// Write an error message into a log file. The error occured in the object
// 'file', giving the error message 'message'.
void log_error(string, string message)
{
  write_file(LOG_FILE_COMPILE, message);
}
