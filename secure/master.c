inherit "/secure/master/error.c";
inherit "/secure/master/connect.c";

private nomask void preload( string file )
{
  object o;

  write("Preloading: "+file+"...  ");
  catch(o = load_object(file));
}

string* epilog( int eflag )
{
  return ({ "/secure/imud/imud.c" });
}

string get_root_uid() {
  return "ROOT";
}

string get_bb_uid() {
  return "BACKBONE";
}

string creator_file() {
  return "";
}

int valid_socket() {
  return 1;
}

int valid_read() {
  return 1;
}

int valid_write() {
  return 1;
}
