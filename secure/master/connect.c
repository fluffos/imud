#define PLAYER_OB "/secure/user.c"

object connect()
{
  object ob;
  string ret;

  ret = catch(ob = new(PLAYER_OB));

  write("\n");
  if ( ret )
  {
    write(ret + "\n");
    return 0;
  }
  return ob;
}
