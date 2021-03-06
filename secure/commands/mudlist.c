#define IMUD_D "/secure/imud/imud.c"

#define COLUMN_LIMIT 120
/*
** Pairs of info for each column.  ( header-index, field-width )
** The header indices can be seen from the headers[] *below.
**
** NOTE: at the moment, the first two elements must specify the "Up"
**       state and the mud name
*/
#define INFO_DRIVERTYPE ({ \
    ({ 0, 2 }), ({ 11, 18 }), \
    ({8,8}),({ 7, 20 }), ({ 5, 18 })})

#define INFO_MUDTYPE	({ \
    ({ 0, 2 }), ({ 11, 18 }), \
    ({8,8}),({ 1, 15 }), ({ 2, 5 }), ({ 5, 18 })})

#define INFO_DEFAULT	({ \
    ({ 0, 2 }), ({ 11, 25 }), \
    ({1, 15}), ({2,5}), ({5, 20}), ({ 7, 25 }), ({ 9, 100 }) })

/* Do not remove the headers from this file! see /USAGE for more info. */

#define CARET_AS_NOT

nosave private mapping cache = ([]);


int has_magic( string s ){
    return sizeof( regexp( ({ s }), "[\\[\\*\\]\\?]"));
}

// The flag toggles whether or not ^ and $ are valid.
// 1 means valid.
varargs string translate( string pat, int flag )
{
    int 	i, j, n;
    string	res, stuff;

    n = strlen(pat);
    res = "";

    for( i = 0; i < n; i++ )
    {
	if( pat[i] == '\\' && i+1 != n ){
	    res += pat[i..++i];
	    continue;
	}
	switch( pat[i] ){
	case '^':
	case '$':
	    if(flag)
	    {
		res += pat[i..i];
		continue;
	    }
	case '.':
	    res += "\\" + pat[i..i];
	    continue;
	case	'*':
	    res += ".*";
	    continue;
	case '?':
	    res += ".";
	    continue;
	case '[':
	    j=i;
	    if( j<n && pat[j] == '!' ) j++;
#ifdef  CARET_AS_NOT
	    if( j<n && pat[j] == '^' ) j++;
#endif
	    if( j<n && pat[j] == ']' ) j++;
	    while( j < n && pat[j] != ']' ) j++;
	    if( j >= n )
		res += "\\[";
	    else{
		stuff = pat[i+1..j];
#ifndef CARET_AS_NOT
		if( member_array('^', stuff) != -1 )
		    stuff = replace_string(stuff,"^","\\^");
#endif
		i = j;
		if( strlen(stuff) > 2 && stuff[0] == '!' ) stuff = "[^"+stuff[1..];
		else stuff = "[" + stuff;
		res += stuff;
	    }
	    continue;
	default:
	    res+= pat[i..i];
	    continue;
	}
    }
    return res;
}

int fnmatch( string name, string pattern )
{
    string 	result;

    if( undefinedp(cache[pattern]) ){
	result = "^"+translate(pattern)+"$";
	cache[pattern] = result;
    }
    return sizeof( regexp( ({ name }), cache[pattern] ) );
}

varargs string insensitive_pattern(string pat)
{
  int i;
  int bracket;
  int patlen;
  string real_pat;

  patlen = strlen(pat);
  real_pat = "";
  for(i=0;i<patlen;i++)
    {
      if(pat[i] == '[') {
	bracket++;
	real_pat += "[";
      }
      else if (pat[i] == ']') {
	bracket--;
	real_pat += "]";
      }

      else if (pat[i] == '\\' && i+1 != patlen)
	{
	  real_pat += pat[i..i+1];
	  i++;
	  continue;
	}
      else if (pat[i] >= 'a' && pat[i] <= 'z' )
           if(!bracket)
		real_pat += sprintf("[%c%c]",pat[i], pat[i] - 32);
           else real_pat += sprintf("%c%c",pat[i], pat[i] - 32);
      else if (pat[i] >= 'A' && pat[i] <= 'Z' )
	  if(!bracket)
		real_pat += sprintf("[%c%c]",pat[i]+32,pat[i]);
	  else
		real_pat += sprintf("%c%c", pat[i]+32,pat[i]);
      else real_pat += pat[i..i];
    }
  return real_pat;
}

varargs string* insensitive_regexp(string* arr, string pat, int flag)
{
  return regexp(arr, insensitive_pattern(pat), flag);
}


private nosave string * headers = ({
      "Up",                                 // 0
      "Address",                            // 1
      "",		/* port */                      // 2
      "",		/* TCP OOB port */              // 3
      "",		/* UDP OOB port */              // 4
      "Lib",                                // 5
      "Base Lib",                           // 6
      "Driver",                             // 7
      "Type",                               // 8
      "Open Status",                        // 9
      "Admin Email",                        // 10

      "Mud",	/* special... mudname. not part of the mud info */
});


int main(mixed *arg, mapping flags)
{
  mapping	mudlist = IMUD_D->query_mudlist();
  string*	muds = keys(mudlist);
  string*	matches;
  int		matched;
  int		upcount;
  string	match;
  mixed*	mudinfo;
  string	output;
  mixed *	info;
  string	format;
  string	search_str;

  if ( !arg || stringp(arg) )
  {
    arg = ({ 0 });
    flags = ([ ]);
  }

  if ( !arg[0] )
    matches = muds;
  else
  {
    matches = insensitive_regexp(muds, "^" + translate(arg[0]));
    if ( !sizeof(matches) )
    {
      printf("No muds out of %d match that pattern.\n", sizeof(mudlist));
      return;
    }
  }

  if ( flags["t"] )
    info = INFO_MUDTYPE;
  else if ( flags["d"] )
    info = INFO_DRIVERTYPE;
  else
    info = INFO_DEFAULT;

  search_str = flags["s"];
  if ( search_str )
    search_str = lower_case(search_str);

  format = implode(info, (: $1 + sprintf("%%-%ds  ", $2[1]) :), "");
  format[<1] = '\n';

  matched = sizeof( matches );

  if ( flags["a"] )
  {
    upcount = implode(matches, (: $1 + ($(mudlist)[$2][0] == -1) :), 0);
  }
  else
  {
    matches = filter_array(matches, (: $(mudlist)[$1][0] == -1 :));
    upcount = sizeof( matches );
  }

  output = "";
  if ( wizardp(this_user()) )
    output += "Type mudinfo <mudname> for more info on a mud.\n";
  output += sprintf(format + repeat_string("-", COLUMN_LIMIT) + "\n",
                    map_array(info, (: $(headers)[$1[0]] :))...);

  foreach ( match in sort_array(matches, 1) )
  {
    string	line;

    mudinfo = mudlist[match];
    line = sprintf(format,
                   mudinfo[0] == -1 ? "U" : "D",
                   match[0..info[1][1]-1],
                   map_array(info[2..],
                       (: ($(mudinfo)[$1[0]]+"")[0..$1[1]-1] :)
    )...);
    if ( search_str )
    {
      if ( regexp(lower_case(line), search_str) )
      {
        output += line;
      }
      else
      {
        --matched;
        if ( mudinfo[0] == -1 )
          --upcount;
      }
    }
    else
    {
      output += line;
    }

  }

  output = sprintf("%d matches out of %d muds. %d %s UP.\n",
                   matched, sizeof(mudlist), upcount, (upcount == 1) ? "is" : "are") + output;

  write(output);

  return 1;
}
