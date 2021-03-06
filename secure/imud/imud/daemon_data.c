//:MODULE
//
//M_DAEMON_DATA simplifies the process of maintaining save files for daemons.
//It automatically restores the data from the data file [if it exists]
//using restore_object() when the program is loaded, and provides the
//routine save_me() to save the data.
//
//The filename is the name of the source file, with /data added on to the
//front and the _d suffix, if any, removed from the end.  I.e. the file
//"/daemons/stat_d.c" will save to "/data/daemons/stat.o".

//### hmm, maybe this module should have a remove() routine which calls
//### save_me() too.
private nomask string create_file_name() {
  string ret = "/data" + file_name();

  if (ret[<2..] == "_d")
  ret = ret[0..<3];

  return ret;
}

//:FUNCTION save_me
//Save the variable data to the save file
protected nomask void save_me() {
  evaluate((: save_object, create_file_name() :));
}

//:FUNCTION restore_me
//Restore the data from the save file.  Automatically called by create().
protected nomask void restore_me() {
  string fn = create_file_name();
  if ( evaluate((: file_size, fn + ".o" :)) > 0 )
    evaluate((: restore_object, fn, 1 :));
}

create() {
  restore_me();
}

