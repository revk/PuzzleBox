// Puzzle box maker
// (c) 2018 Adrian Kennard www.me.uk @TheRealRevK
// This includes a distinctive "A" in the design at the final park point, otherwise there are no loops in the maze
// Please leave the "A" in the design as a distinctive feature

#include <stdio.h>
#include <string.h>
#include <popt.h>
#include <err.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "apple-strdupa.h"

// Flags for maze array
#define	FLAGL 0x01		// Left
#define FLAGR 0x02		// Right
#define FLAGU 0x04		// Up
#define FLAGD 0x08		// Down
#define	FLAGA 0x0F		// All directions
#define	FLAGI 0x80		// Invalid

#define	BIASL	2		// Direction bias for random maze choices
#define	BIASR	1
#define	BIASU	1
#define	BIASD	4

#define	SCALE 1000LL		// Scales used for some aspects of output
#define	SCALEI "0.001"
#define	scaled(x)	((long long)round((x)*SCALE))

int
main (int argc, const char *argv[])
{
  double basethickness = 1.6;
  double basegap = 0.4;
  double baseheight = 10;
  double corediameter = 10;
  double coreheight = 50;
  double wallthickness = 1.2;
  double mazethickness = 2;
  double mazestep = 3;
  double clearance = 0.4;	// General X/Y clearance for parts
  double nubrclearance = 0.1;	// Extra radius clearance for nub, should be less than clearance, can be -ve
  double nubzclearance = 0.2;	// Extra Z clearance (per /4 maze step)
  double parkthickness = 0.7;
  double coregap = 0;
  double outerround = 2;
  double mazemargin = 1;
  double textdepth = 0.5;
  double logodepth = 0.6;
  double gripdepth = 2;
  double textsidescale = 1;
  char *textinside = NULL;
  char *textend = NULL;
  char *textsides = NULL;
  char *textfont = NULL;
  char *textfontend = NULL;
  int parts = 4;
  int part = 0;
  int inside = 0;
  int flip = 0;
  int outersides = 7;
  int testmaze = 0;
  int helix = 3;
  int nubs = helix;
  int logo = 0;
  int textslow = 0;
  int textoutset = 0;
  int symmectriccut = 0;
  int coresolid = 0;
  int mime = (getenv ("HTTP_HOST") ? 1 : 0);
  int webform = 0;
  int parkvertical = 0;
  int mazecomplexity = 5;
  int mirrorinside = 0;		// Clockwise lock on inside - may be unwise as more likely to come undone with outer.
  int noa = 0;
  int basewide = 0;

  int f = open ("/dev/urandom", O_RDONLY);
  if (f < 0)
    err (1, "Open /dev/random");

  char pathsep = 0;
  char *path = getenv ("PATH_INFO");
  if (path)
    pathsep = '/';
  else if ((path = getenv ("QUERY_STRING")))
    pathsep = '&';

  const struct poptOption optionsTable[] = {
    {"parts", 'm', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &parts, 0, "Total parts", "N"},
    {"part", 'n', POPT_ARG_INT, &part, 0, "Part to make", "N (0 for all)"},
    {"inside", 'i', POPT_ARG_NONE, &inside, 0, "Maze on inside (hard)"},
    {"flip", 'f', POPT_ARG_NONE, &flip, 0, "Alternating inside/outside maze"},
    {"nubs", 'N', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &nubs, 0, "Nubs", "N"},
    {"helix", 'H', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &helix, 0, "Helix", "N (0 for non helical)"},
    {"base-height", 'b', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &baseheight, 0, "Base height", "mm"},
    {"core-diameter", 'c', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &corediameter, 0, "Core diameter for content", "mm"},
    {"core-height", 'h', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coreheight, 0, "Core height for content", "mm"},
    {"core-gap", 'C', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coregap, 0, "Core gap to allow content to be removed", "mm"},
    {"core-solid", 'q', POPT_ARG_NONE, &coresolid, 0, "Core solid (content is in part 2)"},
    {"base-thickness", 'B', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &basethickness, 0, "Base thickness", "mm"},
    {"base-gap", 'G', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &basegap, 0, "Base gap (Z clearance)", "mm"},
    {"base-wide", 'W', POPT_ARG_NONE, &basewide, 0, "Inside base full width"},
    {"part-thickness", 'w', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &wallthickness, 0, "Wall thickness", "mm"},
    {"maze-thickness", 't', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazethickness, 0, "Maze thickness", "mm"},
    {"maze-step", 'z', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazestep, 0, "Maze spacing", "mm"},
    {"maze-margin", 'M', POPT_ARG_DOUBLE | (mazemargin ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &mazemargin, 0, "Maze top margin", "mm"},
    {"maze-complexity", 'X', POPT_ARG_INT | (mazecomplexity ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &mazecomplexity, 0, "Maze complexity", "-10 to 10"},
    {"park-thickness", 'p', POPT_ARG_DOUBLE | (parkthickness ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &parkthickness, 0, "Thickness of park ridge to click closed", "mm"},
    {"park-vertical", 'v', POPT_ARG_NONE, &parkvertical, 0, "Park vertically"},
    {"clearance", 'g', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &clearance, 0, "General X/Y clearance", "mm"},
    {"outer-sides", 's', POPT_ARG_INT | (outersides ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &outersides, 0, "Number of outer sides", "N (0=round)"},
    {"outer-round", 'r', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &outerround, 0, "Outer rounding on ends", "mm"},
    {"grip-depth", 'R', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &gripdepth, 0, "Grip depth", "mm"},
    {"text-depth", 'D', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &textdepth, 0, "Text depth", "mm"},
    {"text-end", 'E', POPT_ARG_STRING | (textend ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &textend, 0, "Text (initials) on end", "X{\\X...}"},
    {"text-side", 'S', POPT_ARG_STRING | (textsides ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &textsides, 0, "Text on sides", "Text{\\Text...}"},
    {"text-font", 'F', POPT_ARG_STRING | (textfont ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &textfont, 0, "Text font (optional)", "Font"},
    {"text-font-end", 'e', POPT_ARG_STRING | (textfontend ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &textfontend, 0, "Text font for end (optional)", "Font"},
    {"text-slow", 'Z', POPT_ARG_NONE, &textslow, 0, "Text has diagonal edges (very slow)"},
    {"text-side-scale", 'T', POPT_ARG_DOUBLE, &textsidescale, 0, "Scale side text (i.e. if too long)", "N"},
    {"text-outset", 'O', POPT_ARG_NONE, &textoutset, 0, "Text on sides is outset not embossed"},
    {"text-inside", 'I', POPT_ARG_STRING | (textinside ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &textinside, 0, "Text (initials) inside end", "X{\\X...}"},
    {"logo-depth", 'L', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &logodepth, 0, "Logo (and inside text) cut depth", "mm"},
    {"symmetric-cut", 'V', POPT_ARG_NONE, &symmectriccut, 0, "Symmetric maze cut"},
    {"nub-r-clearance", 'y', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &nubrclearance, 0, "Extra clearance on radius for nub", "mm"},
    {"nub-z-clearance", 'Z', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &nubzclearance, 0, "Extra clearance on height of nub", "mm"},
    {"logo", 'A', POPT_ARG_NONE, &logo, 0, "Include A&A logo in last lid"},
    {"test", 'Q', POPT_ARG_NONE, &testmaze, 0, "Test pattern instead of maze"},
    {"mime", 0, POPT_ARG_NONE | (mime ? POPT_ARGFLAG_DOC_HIDDEN : 0), &mime, 0, "MIME Header"},
    {"no-a", 0, POPT_ARG_NONE | (noa ? POPT_ARGFLAG_DOC_HIDDEN : 0), &noa, 0, "No A"},
    {"web-form", 0, POPT_ARG_NONE, &webform, 0, "Web form"},
    POPT_AUTOHELP {}
  };

  {				// POPT
    poptContext optCon;		// context for parsing command-line options

    optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
    //poptSetOtherOptionHelp (optCon, "");

    int c;
    if ((c = poptGetNextOpt (optCon)) < -1)
      errx (1, "%s: %s\n", poptBadOption (optCon, POPT_BADOPTION_NOALIAS), poptStrerror (c));

    if (poptPeekArg (optCon))
      {
	poptPrintUsage (optCon, stderr, 0);
	return -1;
      }
    poptFreeContext (optCon);
  }

  char *error = NULL;
  if (path)
    {				// Settings from PATH_INFO
      path = strdupa (path);
      while (*path)
	{
	  if (*path == pathsep)
	    {
	      *path++ = 0;
	      continue;
	    }
	  if (!isalpha (*path))
	    {
	      if (asprintf (&error, "Path error [%s]\n", path) < 0)
		errx (1, "malloc");
	      break;
	    }
	  char arg = *path++;
	  int o;
	  for (o = 0; optionsTable[o].longName && optionsTable[o].shortName != arg; o++);
	  if (!optionsTable[o].shortName)
	    {
	      if (asprintf (&error, "Unknown arg [%c]", arg) < 0)
		errx (1, "malloc");
	      break;
	    }
	  if (optionsTable[o].arg)
	    switch (optionsTable[o].argInfo & POPT_ARG_MASK)
	      {
	      case POPT_ARG_INT:
		if (*path != '=')
		  {
		    if (asprintf (&error, "Missing value [%c=]", arg) < 0)
		      errx (1, "malloc");
		    break;
		  }
		if (path[1])
		  *(int *) optionsTable[o].arg = strtod (path + 1, &path);
		break;
	      case POPT_ARG_DOUBLE:
		if (*path != '=')
		  {
		    if (asprintf (&error, "Missing value [%c=]", arg) < 0)
		      errx (1, "malloc");
		    break;
		  }
		if (path[1])
		  *(double *) optionsTable[o].arg = strtod (path + 1, &path);
		break;
	      case POPT_ARG_NONE:
		*(int *) optionsTable[o].arg = 1;
		if (*path == '=')
		  {		// Skip =on, etc.
		    path++;
		    while (*path && *path != pathsep)
		      path++;
		  }
		break;
	      case POPT_ARG_STRING:
		if (*path != '=')
		  {
		    if (asprintf (&error, "Missing value [%c=]", arg) < 0)
		      errx (1, "malloc");
		    break;
		  }
		path++;
		*(char **) optionsTable[o].arg = path;
		char *o = path;
		while (*path && *path != pathsep)
		  {
		    if (pathsep == '&' && *path == '+')
		      *o++ = ' ';
		    else if (pathsep == '&' && *path == '%' && isxdigit (path[1]) && isxdigit (path[2]))
		      {
			*o++ = (((isalpha (path[1]) ? 9 : 0) + (path[1] & 0xF)) << 4) + ((isalpha (path[2]) ? 9 : 0) + (path[2] & 0xF));
			path += 2;
		      }
		    else
		      *o++ = *path;
		    path++;
		  }
		if (o < path)
		  *o = 0;
		break;
	      }
	}
    }

  if (webform)
    {
      int o;
      for (o = 0; optionsTable[o].longName; o++)
	if (optionsTable[o].shortName && optionsTable[o].arg)
	  {
	    printf ("<tr>");
	    printf ("<td><label for='%c'>%c%s</label></td>", optionsTable[o].shortName, optionsTable[o].shortName, (optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_NONE ? "" : "=");
	    printf ("<td>");
	    switch (optionsTable[o].argInfo & POPT_ARG_MASK)
	      {
	      case POPT_ARG_NONE:	// Checkbox
		printf ("<input type=checkbox id='%c' name='%c'/>", optionsTable[o].shortName, optionsTable[o].shortName);
		break;
	      case POPT_ARG_INT:	// Select
		{
		  int l = 0, h = 3, v = *(int *) optionsTable[o].arg;
		  if (optionsTable[o].shortName == 'N')
		    l = 1;	// Nubs
		  if (optionsTable[o].shortName == 'm')
		    l = 2;	// Walls
		  if (optionsTable[o].shortName == 'n' || optionsTable[o].shortName == 'm')
		    h = 6;	// Walls or part
		  if (optionsTable[o].shortName == 's')
		    h = 20;	// Sides
		  if (optionsTable[o].shortName == 'X')
		    {		// Complexity
		      l = -10;
		      h = 10;
		    }
		  printf ("<select name='%c' id='%c'>", optionsTable[o].shortName, optionsTable[o].shortName);
		  for (; l <= h; l++)
		    printf ("<option value='%d'%s>%d</option>", l, l == v ? " selected" : "", l);
		  printf ("</select>");
		}
		break;
	      case POPT_ARG_DOUBLE:	// Double
		{
		  double v = *(double *) optionsTable[o].arg;
		  printf ("<input size='5' name='%c' id='%c'", optionsTable[o].shortName, optionsTable[o].shortName);
		  if (v)
		    {
		      char temp[50], *p;
		      sprintf (temp, "%f", v);
		      for (p = temp + strlen (temp); p > temp && p[-1] == '0'; p--);
		      if (p > temp && p[-1] == '.')
			p--;
		      *p = 0;
		      printf (" value='%s'", temp);
		    }
		  printf ("/>");
		}
		break;
	      case POPT_ARG_STRING:	// String
		{
		  char *v = *(char **) optionsTable[o].arg;
		  printf ("<input name='%c' id='%c'", optionsTable[o].shortName, optionsTable[o].shortName);
		  if (optionsTable[o].shortName == 'E')
		    printf (" size='2'");	// Initials
		  if (v)
		    printf (" value='%s'", v);
		  printf ("/>");
		}
		break;
	      }
	    if (optionsTable[o].argDescrip)
	      printf ("%s", optionsTable[o].argDescrip);
	    printf ("</td>");
	    printf ("<td><label for='%c'>%s</label></td>", optionsTable[o].shortName, optionsTable[o].descrip);
	    printf ("</tr>\n");
	  }
      return 0;
    }

  // Sanity checks and adjustments
  char *normalise (char *t)
  {				// Simple text normalise
    if (!t || !*t)
      return NULL;
    char *text = t;
    while (*t)
      {
	if (*t == '"')
	  *t == '\'';
	t++;
      }
    return text;
  }
  textend = normalise (textend);
  textsides = normalise (textsides);
  textinside = normalise (textinside);
  if (!outersides)
    textsides = NULL;
  if (textfont && !*textfont)
    textfont = NULL;
  if (textfont && !textfontend)
    textfontend = textfont;
  if (textend && !*textend)
    textend = NULL;
  if (textinside && !*textinside)
    textinside = NULL;
  if (textsides && !*textsides)
    {
      textsidescale = 0;
      textsides = NULL;
    }
  if (helix && nubs > 1 && nubs < helix)
    {
      if (!(helix % 2) && nubs <= helix / 2)
	nubs = helix / 2;
      else
	nubs = helix;
    }
  if (helix && nubs > helix)
    nubs = helix;
  if (gripdepth > (baseheight - outerround) / 6)
    gripdepth = (baseheight - outerround) / 6;
  if (gripdepth > mazethickness)
    gripdepth = mazethickness;
  if (!logo && !textinside)
    logodepth = 0;
  if (!textsides && !textend && !textinside)
    textdepth = 0;
  if (coresolid && coregap < mazestep * 2)
    coregap = mazestep * 2;

  int markpos0 = (outersides && outersides / nubs * nubs != outersides);	// Mark on position zero for alignment
  double nubskew = (symmectriccut ? 0 : mazestep / 8);	// Skew the shape of the cut

  // MIME header
  if (mime)
    {
      printf ("Content-Type: application/scad\r\nContent-Disposition: Attachment; filename=puzzlebox");
      int o;
      for (o = 0; optionsTable[o].longName; o++)
	if (optionsTable[o].shortName && optionsTable[o].arg)
	  switch (optionsTable[o].argInfo & POPT_ARG_MASK)
	    {
	    case POPT_ARG_NONE:
	      if (!*(int *) optionsTable[o].arg)
		break;
	      printf ("-%c", optionsTable[o].shortName);
	      break;
	    case POPT_ARG_INT:
	      if (!*(int *) optionsTable[o].arg)
		break;
	      printf ("-%d%c", *(int *) optionsTable[o].arg, optionsTable[o].shortName);
	      break;
	    case POPT_ARG_DOUBLE:
	      if (!*(double *) optionsTable[o].arg)
		break;
	      char temp[50], *p;
	      sprintf (temp, "%f", *(double *) optionsTable[o].arg);
	      p = temp + strlen (temp) - 1;
	      while (*p == '0')
		*p-- = 0;
	      p = strchr (temp, '.');
	      if (*p)
		*p++ = 0;
	      printf ("-%s%c%s", temp, optionsTable[o].shortName, p);
	      break;
	    case POPT_ARG_STRING:
	      if (!*(char **) optionsTable[o].arg)
		break;
	      {
		char *p = strdupa (*(char * *) optionsTable[o].arg), *q;
		for (q = p; *q; q++)
		  if (*q <= ' ' || *q == '/' || *q == '\\' || *q == '"' || *q == '\'' || *q == ':')
		    *q = '_';
		printf ("-%c%s", optionsTable[o].shortName, p);
	      }
	      break;
	    }
      printf (".scad\r\n\r\n");	// Used from apache
    }

  printf ("// Puzzlebox by RevK, @TheRealRevK www.me.uk\n");
  printf ("// Thingiverse examples and instructions https://www.thingiverse.com/thing:2410748\n");
  printf ("// GitHub source https://github.com/revk/PuzzleBox\n");
  printf ("// Get new random customer maze gift boxes from https://www.me.uk/puzzlebox\n");
  {				// Document args
    time_t now = time (0);
    struct tm t;
    gmtime_r (&now, &t);
    printf ("// Created %04d-%02d-%02dT%02d:%02d:%02dZ %s\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, getenv ("REMOTE_ADDR") ? : "");
    int o;
    for (o = 0; optionsTable[o].longName; o++)
      if (optionsTable[o].shortName && optionsTable[o].arg)
	switch (optionsTable[o].argInfo & POPT_ARG_MASK)
	  {
	  case POPT_ARG_NONE:
	    if (*(int *) optionsTable[o].arg)
	      printf ("// %s: %c\n", optionsTable[o].descrip, optionsTable[o].shortName);
	    break;
	  case POPT_ARG_INT:
	    {
	      int v = *(int *) optionsTable[o].arg;
	      if (v)
		printf ("// %s: %c=%d\n", optionsTable[o].descrip, optionsTable[o].shortName, v);
	    }
	    break;
	  case POPT_ARG_DOUBLE:
	    {
	      double v = *(double *) optionsTable[o].arg;
	      if (v)
		{
		  char temp[50], *p;
		  sprintf (temp, "%f", v);
		  for (p = temp + strlen (temp); p > temp && p[-1] == '0'; p--);
		  if (p > temp && p[-1] == '.')
		    p--;
		  *p = 0;
		  printf ("// %s: %c=%s\n", optionsTable[o].descrip, optionsTable[o].shortName, temp);
		}
	    }
	    break;
	  case POPT_ARG_STRING:
	    {
	      char *v = *(char * *) optionsTable[o].arg;
	      if (v && *v)
		printf ("// %s: %c=%s\n", optionsTable[o].descrip, optionsTable[o].shortName, v);
	    }
	    break;
	  }
  }
  if (error)
    {				// Problem
      printf ("// ** %s **\n", error);
      return 1;
    }

  // Other adjustments
  basethickness += logodepth;

  {				// Modules
    if (textslow)
      printf ("module cuttext(){translate([0,0,-1])minkowski(){rotate([0,0,22.5])cylinder(h=%lld,d1=%lld,d2=0,$fn=8);linear_extrude(height=1,convexity=10)mirror([1,0,0])children();}}\n", scaled (textdepth), scaled (textdepth));
    else
      printf ("module cuttext(){linear_extrude(height=%lld,convexity=10,center=true)mirror([1,0,0])children();}\n", scaled (textdepth));
    // You can use the A&A logo on your maze print providing it is tasteful and not in any way derogatory to A&A or any staff/officers.
    if (logo)
      printf
	("module aa(w=100,white=0,$fn=100){scale(w/100){if(!white)difference(){circle(d=100.5);circle(d=99.5);}difference(){if(white)circle(d=100);difference(){circle(d=92);for(m=[0,1])mirror([m,0,0]){difference(){translate([24,0,0])circle(r=22.5);translate([24,0,0])circle(r=15);}polygon([[1.5,22],[9,22],[9,-18.5],[1.5,-22]]);}}}}} // A&A Logo is copyright (c) 2013 and trademark Andrews & Arnold Ltd\n");
  }
  void cuttext (double s, char *t, char *f, int outset)
  {
    if (outset)
      printf ("mirror([0,0,1])");
    printf ("cuttext()");
    printf ("scale(%lld)", scaled (1));
    printf ("text(\"%s\"", t);
    printf (",halign=\"center\"");
    printf (",valign=\"center\"");
    printf (",size=%lf", s);
    if (*t & 0x80)
      printf (",font=\"Noto Emoji\"");	// Assume emoji - not clean - TODO needs fontconfig stuff really
    else if (f)
      printf (",font=\"%s\"", f);
    printf (");\n");
  }
  // The base
  printf ("module outer(h,r){e=%lld;minkowski(){cylinder(r1=0,r2=e,h=e,$fn=24);cylinder(h=h-e,r=r,$fn=%d);}}\n", scaled (outerround), outersides ? : 100);
  // Start
  double x = 0, y = 0;
  int sq = sqrt (parts) + 0.5, n = sq * sq - parts;
  int box (int part)
  {				// Make the box - part 1 in inside
    int N, X, Y, Z, S;
    double entrya = 0;		// Entry angle
    int mazeinside = inside;	// This part has maze inside
    int mazeoutside = !inside;	// This part has maze outside
    int nextinside = inside;	// Next part has maze inside
    int nextoutside = !inside;	// Next part has maze outside
    if (flip)
      {
	if (part & 1)
	  {
	    mazeinside = 1 - mazeinside;
	    nextoutside = 1 - nextoutside;
	  }
	else
	  {
	    mazeoutside = 1 - mazeoutside;
	    nextinside = 1 - nextinside;
	  }
      }
    if (part == 1)
      mazeinside = 0;
    if (part == parts)
      mazeoutside = 0;
    if (part + 1 >= parts)
      nextoutside = 0;
    if (part == parts)
      nextinside = 0;
    // Dimensions
    // r0 is inside of part+maze
    // r1 is outside of part+maze
    // r2 is outside of base before "sides" adjust
    // r3 is outside of base with "sides" adjust
    double r1 = corediameter / 2 + wallthickness + (part - 1) * (wallthickness + mazethickness + clearance);	// Outer
    if (coresolid)
      r1 -= wallthickness + mazethickness + clearance - (inside ? mazethickness : 0);	// Adjust to make part 2 the core diameter
    int W = ((int) (r1 * 2 * M_PI / mazestep)) / nubs * nubs;	// Default value
    double r0 = r1 - wallthickness;	// Inner
    if (mazeinside && part > 1)
      r0 -= mazethickness;	// Maze on inside
    if (mazeoutside && part < parts)
      r1 += mazethickness;	// Maze on outside
    double r2 = r1;		// Base outer
    if (part < parts)
      r2 += clearance;
    if (part + 1 >= parts && textsides && !textoutset)
      r2 += textdepth;
    if (nextinside)
      r2 += mazethickness;
    if (nextoutside || part + 1 == parts)
      r2 += wallthickness;
    if (basewide && part + 1 < parts)
      r2 += nextoutside ? mazethickness : wallthickness;
    double r3 = r2;
    if (outersides && part + 1 >= parts)
      r3 /= cos ((double) M_PI / outersides);	// Bigger because of number of sides
    printf ("// Part %d (%.2fmm to %.2fmm and %.2fmm/%.2fmm base)\n", part, r0, r1, r2, r3);
    double height = (coresolid ? coregap + baseheight : 0) + coreheight + basethickness + (basethickness + basegap) * (part - 1);
    if (part == 1)
      height -= (coresolid ? coreheight : coregap);
    if (part > 1)
      height -= baseheight;	// base from previous unit is added to this
    // Output
    void makemaze (double r, int inside)
    {				// Make the maze
      W = ((int) ((r + (inside ? mazethickness : -mazethickness)) * 2 * M_PI / mazestep)) / nubs * nubs;	// Update W for actual maze
      double base = (inside ? basethickness : baseheight);
      if (inside && part > 2)
	base += baseheight;	// Nubs don't go all the way to the end
      if (inside && part == 2)
	base += (coresolid ? coreheight : coregap);	// First one is short...
      if (inside)
	base += basegap;
      double h = height - base - mazemargin - (parkvertical ? mazestep / 4 : 0) - mazestep / 8;
      int H = (int) (h / mazestep);
      printf ("// Maze %s %d/%d\n", inside ? "inside" : "outside", W, H);
      double y0 = base + mazestep / 2 - mazestep * (helix + 1) + mazestep / 8;
      H += 2 + helix;		// Allow one above, one below and helix below
      if (W < 3 || H < 1)
	errx (1, "Too small");
      double a = 0, dy = 0;
      if (helix)
	{
	  a = atan (mazestep * helix / r / 2 / M_PI) * 180 / M_PI;
	  dy = mazestep * helix / W;
	}
      unsigned char maze[W][H];
      memset (maze, 0, sizeof (unsigned char) * W * H);
      int test (int x, int y)
      {				// Test if in use...
	while (x < 0)
	  {
	    x += W;
	    y -= helix;
	  }
	while (x >= W)
	  {
	    x -= W;
	    y += helix;
	  }
	int n = nubs;
	unsigned char v = 0;
	while (n--)
	  {
	    if (y < 0 || y >= H)
	      v |= FLAGI;
	    else
	      v |= maze[x][y];
	    if (!n)
	      break;
	    x += W / nubs;
	    while (x >= W)
	      {
		x -= W;
		y += helix;
	      }
	    if (helix == nubs)
	      y--;
	  }
	return v;
      }
      {				// Maze
	double margin = mazemargin;
	// Make maze
	// Clear too high/low
	for (Y = 0; Y < H; Y++)
	  for (X = 0; X < W; X++)
	    if (mazestep * Y + y0 + dy * X < base + mazestep / 2 + mazestep / 8 || mazestep * Y + y0 + dy * X > height - mazestep / 2 - margin - mazestep / 8)
	      maze[X][Y] |= FLAGI;	// To high or low
	// Final park point
	if (parkvertical)
	  {
	    for (N = 0; N < helix + 2; N++)	// Down to final
	      {
		maze[0][N] |= FLAGU + FLAGD;
		maze[X = 0][Y = N + 1] |= FLAGD;
	      }
	    if (!inside && !noa && W / nubs > 2 && H > helix + 4)
	      {			// An "A" at finish
		maze[X][Y] |= FLAGD | FLAGU | FLAGR;
		maze[X][Y + 1] |= FLAGD | FLAGR;
		maze[X + 1][Y] |= FLAGD | FLAGU | FLAGL;
		maze[X + 1][Y + 1] |= FLAGD | FLAGL;
		maze[X + 1][Y - 1] |= FLAGU;
		X++;
		Y--;
	      }
	  }
	else			// Left to final
	  {
	    maze[0][helix + 1] |= FLAGR;
	    maze[X = 1][Y = helix + 1] |= FLAGL;
	    if (!inside && !noa && W / nubs > 3 && H > helix + 3)
	      {			// An "A" at finish
		maze[X][Y] |= FLAGL | FLAGR | FLAGU;
		maze[X + 1][Y] |= FLAGL | FLAGU;
		maze[X + 1][Y + 1] |= FLAGL | FLAGD;
		maze[X][Y + 1] |= FLAGL | FLAGR | FLAGD;
		maze[X - 1][Y + 1] |= FLAGR;
		X--;
		Y++;
	      }
	  }
	// Make maze
	int maxx = 0;
	if (testmaze)
	  {			// Simple test pattern
	    for (Y = 0; Y < H; Y++)
	      for (X = 0; X < W; X++)
		if (!(test (X, Y) & FLAGI) && !(test (X + 1, Y) & FLAGI))
		  {
		    maze[X][Y] |= FLAGR;
		    int x = X + 1, y = Y;
		    if (x >= W)
		      {
			x -= W;
			y += helix;
		      }
		    maze[x][y] |= FLAGL;
		  }
	    if (!flip || inside)
	      while (maxx + 1 < W && !(test (maxx + 1, H - 2) & FLAGI))
		maxx++;
	  }
	else
	  {			// Actual maze
	    int max = 0;
	    typedef struct pos_s pos_t;
	    struct pos_s
	    {
	      pos_t *next;
	      int x, y, n;
	    };
	    pos_t *pos = malloc (sizeof (*pos)), *last = NULL;
	    pos->x = X;
	    pos->y = Y;
	    pos->n = 0;
	    pos->next = NULL;
	    last = pos;
	    while (pos)
	      {
		pos_t *p = pos;
		pos = p->next;
		p->next = NULL;
		if (!pos)
		  last = NULL;
		// Where we are
		X = p->x;
		Y = p->y;
		int v, n = 0;
		// Which way can we go
		// Some bias for direction
		if (!test (X + 1, Y))
		  n += BIASR;	// Right
		if (!test (X - 1, Y))
		  n += BIASL;	// Left
		if (!test (X, Y - 1))
		  n += BIASD;	// Down
		if (!test (X, Y + 1))
		  n += BIASU;	// Up
		if (!n)
		  {		// No way forward
		    free (p);
		    continue;
		  }
		// Pick one of the ways randomly
		if (read (f, &v, sizeof (v)) != sizeof (v))
		  err (1, "Read /dev/random");
		v %= n;
		// Move forward
		if (!test (X + 1, Y) && (v -= BIASR) < 0)
		  {		// Right
		    maze[X][Y] |= FLAGR;
		    X++;
		    if (X >= W)
		      {
			X -= W;
			Y += helix;
		      }
		    maze[X][Y] |= FLAGL;
		  }
		else if (!test (X - 1, Y) && (v -= BIASL) < 0)
		  {		// Left
		    maze[X][Y] |= FLAGL;
		    X--;
		    if (X < 0)
		      {
			X += W;
			Y -= helix;
		      }
		    maze[X][Y] |= FLAGR;
		  }
		else if (!test (X, Y - 1) && (v -= BIASD) < 0)
		  {		// Down
		    maze[X][Y] |= FLAGD;
		    Y--;
		    maze[X][Y] |= FLAGU;
		  }
		else if (!test (X, Y + 1) && (v -= BIASU) < 0)
		  {		// Up
		    maze[X][Y] |= FLAGU;
		    Y++;
		    maze[X][Y] |= FLAGD;
		  }
		else
		  errx (1, "WTF");	// We should have picked a way we can go
		// Entry
		if (p->n > max && (test (X, Y + 1) & FLAGI)	//
		    && (!flip || inside || !(X % (W / nubs))))
		  {		// Longest path that reaches top
		    max = p->n;
		    maxx = X;
		  }
		// Next point to consider
		pos_t *next = malloc (sizeof (*next));
		next->x = X;
		next->y = Y;
		next->n = p->n + 1;
		next->next = NULL;
		// How to add points to queue... start or end
		if (read (f, &v, sizeof (v)) != sizeof (v))
		  err (1, "Read /dev/random");
		v %= 10;
		if (v < (mazecomplexity < 0 ? -mazecomplexity : mazecomplexity))
		  {		// add next point at start - makes for longer path
		    if (!pos)
		      last = next;
		    next->next = pos;
		    pos = next;
		  }
		else
		  {		// add next point at end - makes for multiple paths, which can mean very simple solution
		    if (last)
		      last->next = next;
		    else
		      pos = next;
		    last = next;
		  }
		if (mazecomplexity <= 0 && v < -mazecomplexity)
		  {		// current point to start
		    if (!pos)
		      last = p;
		    p->next = pos;
		    pos = p;
		  }
		else
		  {
		    if (last)
		      last->next = p;
		    else
		      pos = p;
		    last = p;
		  }
	      }
	    printf ("// Path length %d\n", max);
	  }
	entrya = (double) 360 *maxx / W;
	// Entry point for maze
	for (X = maxx % (W / nubs); X < W; X += W / nubs)
	  {
	    Y = H - 1;
	    while (Y && (maze[X][Y] & FLAGI))
	      maze[X][Y--] |= FLAGU + FLAGD;
	    maze[X][Y] += FLAGU;
	  }

	int MAXY = height / (mazestep / 4) + 10;
	struct
	{			// Data for each slive
	  // Pre calculated x/y for left side 0=back, 1=recess, 2=front - used to create points
	  double x[3], y[3];
	  // The last points as we work up slice (-ve for recess, 0 for not set yet)
	  int l, r;
	  // Points from bottom up on this slice in order - used to ensure manifold buy using points that would be skipped
	  int n;		// Points added to p
	  int p[MAXY];
	} s[W * 4];
	memset (&s, 0, sizeof (*s) * W * 4);
	int p[W][H];		// The point start for each usable maze location (0 for not set) - 16 points
	memset (*p, 0, sizeof (int) * W * H);
	// Work out pre-sets
	for (S = 0; S < W * 4; S++)
	  {
	    double a = M_PI * 2 * (S - 1.5) / W / 4;
	    if (!inside)
	      a = M_PI * 2 - a;
	    double sa = sin (a), ca = cos (a);
	    if (inside)
	      {
		s[S].x[0] = (r + mazethickness + (part < parts ? wallthickness : clearance + 0.01)) * sa;
		s[S].y[0] = (r + mazethickness + (part < parts ? wallthickness : clearance + 0.01)) * ca;
		s[S].x[1] = (r + mazethickness) * sa;
		s[S].y[1] = (r + mazethickness) * ca;
		s[S].x[2] = r * sa;
		s[S].y[2] = r * ca;
	      }
	    else
	      {
		s[S].x[0] = (r - mazethickness - wallthickness) * sa;
		s[S].y[0] = (r - mazethickness - wallthickness) * ca;
		s[S].x[1] = (r - mazethickness) * sa;
		s[S].y[1] = (r - mazethickness) * ca;
		s[S].x[2] = r * sa;
		s[S].y[2] = r * ca;
	      }
	  }
	if (inside && mirrorinside)
	  printf ("mirror([1,0,0])");
	printf ("polyhedron(");
	// Make points
	printf ("points=[");
	int P = 0;
	void addpoint (int S, double x, double y, double z)
	{
	  printf ("[%lld,%lld,%lld],", scaled (x), scaled (y), scaled (z));
	  if (s[S].n >= MAXY)
	    errx (1, "WTF points %d", S);
	  s[S].p[s[S].n++] = P++;
	}
	void addpointr (int S, double x, double y, double z)
	{
	  printf ("[%lld,%lld,%lld],", scaled (x), scaled (y), scaled (z));
	  if (s[S].n >= MAXY)
	    errx (1, "WTF points %d", S);
	  s[S].p[s[S].n++] = -(P++);
	}
	int bottom = P;
	// Base points
	for (S = 0; S < W * 4; S++)
	  addpoint (S, s[S].x[0], s[S].y[0], basethickness - clearance);
	for (S = 0; S < W * 4; S++)
	  addpointr (S, s[S].x[1], s[S].y[1], basethickness - clearance);
	for (S = 0; S < W * 4; S++)
	  addpoint (S, s[S].x[2], s[S].y[2], basethickness - clearance);
	{			// Points for each maze location
	  double dy = mazestep * helix / W / 4;	// Step per S
	  double my = mazestep / 8;	// Vertical steps
	  double y = y0 - dy * 1.5;	// Y vertical centre for S=0
	  for (Y = 0; Y < H; Y++)
	    for (X = 0; X < W; X++)
	      {
		unsigned char v = test (X, Y);
		if (!(v & FLAGA) || (v & FLAGI))
		  continue;
		p[X][Y] = P;
		for (S = X * 4; S < X * 4 + 4; S++)
		  addpoint (S, s[S].x[2], s[S].y[2], y + Y * mazestep + dy * S - my * 3);
		for (S = X * 4; S < X * 4 + 4; S++)
		  addpointr (S, s[S].x[1], s[S].y[1], y + Y * mazestep + dy * S - my - nubskew);
		for (S = X * 4; S < X * 4 + 4; S++)
		  addpointr (S, s[S].x[1], s[S].y[1], y + Y * mazestep + dy * S + my - nubskew);
		for (S = X * 4; S < X * 4 + 4; S++)
		  addpoint (S, s[S].x[2], s[S].y[2], y + Y * mazestep + dy * S + my * 3);
	      }
	}
	int top = P;
	for (S = 0; S < W * 4; S++)
	  addpoint (S, s[S].x[2], s[S].y[2], height - (basewide && !inside && part > 1 ? 0 : margin));	// lower
	for (S = 0; S < W * 4; S++)
	  addpoint (S, s[S].x[1], s[S].y[1], height);
	for (S = 0; S < W * 4; S++)
	  addpoint (S, s[S].x[0], s[S].y[0], height);
	for (S = 0; S < W * 4; S++)
	  {			// Wrap back to start
	    if (s[S].n >= MAXY)
	      errx (1, "WTF points");
	    s[S].p[s[S].n++] = S;
	  }
	printf ("]");
	// Make faces
	void slice (int S, int l, int r)
	{			// Advance slice S to new L and R (-ve for recess)
	  inline int abs (int x)
	  {
	    if (x < 0)
	      return -x;
	    return x;
	  }
	  inline int sgn (int x)
	  {
	    if (x < 0)
	      return -1;
	    if (x > 0)
	      return 1;
	    return 0;
	  }
	  if (S >= W * 4)
	    errx (1, "Bad render %d", S);
	  char start = 0;
	  if (!s[S].l)
	    {			// New - draw to bottom
	      s[S].l = (l < 0 ? -1 : 1) * (bottom + S + W * 4 + (l < 0 ? 0 : W * 4));
	      s[S].r = (r < 0 ? -1 : 1) * (bottom + (S + 1) % (W * 4) + W * 4 + (r < 0 ? 0 : W * 4));
	      printf ("[%d,%d,%d,%d],", abs (s[S].l), abs (s[S].r), (S + 1) % (W * 4), S);
	    }
	  // Advance
	  if (l == s[S].l && r == s[S].r)
	    return;
	  int SR = (S + 1) % (W * 4);
	  printf ("[");
	  int p = 0;
	  int n1, n2;
	  for (n1 = 0; n1 < s[S].n && abs (s[S].p[n1]) != abs (s[S].l); n1++);
	  for (n2 = n1; n2 < s[S].n && abs (s[S].p[n2]) != abs (l); n2++);
	  if (n1 == s[S].n || n2 == s[S].n)
	    errx (1, "Bad render %d->%d", s[S].l, l);
	  while (n1 < n2)
	    {
	      if (sgn (s[S].p[n1]) == sgn (s[S].l))
		{
		  printf ("%d,", abs (s[S].p[n1]));
		  p++;
		}
	      n1++;
	    }
	  printf ("%d,", abs (l));
	  if (p)
	    printf ("%d],", abs (r));	// Triangles
	  for (n1 = 0; n1 < s[SR].n && abs (s[SR].p[n1]) != abs (s[S].r); n1++);
	  for (n2 = n1; n2 < s[SR].n && abs (s[SR].p[n2]) != abs (r); n2++);
	  if (n1 == s[SR].n || n2 == s[SR].n)
	    errx (1, "Bad render %d->%d", r, s[S].r);
	  if (!p || n1 < n2)
	    {
	      n2--;
	      if (p)
		printf ("[");
	      printf ("%d", abs (r));
	      while (n1 <= n2)
		{
		  if (sgn (s[SR].p[n2]) == sgn (s[S].r))
		    printf (",%d", abs (s[SR].p[n2]));
		  n2--;
		}
	      if (p)
		printf (",%d", abs (s[S].l));
	      printf ("],");
	    }
	  s[S].l = l;
	  s[S].r = r;
	}
	printf (",\nfaces=[");
	// Maze
	for (Y = 0; Y < H; Y++)
	  for (X = 0; X < W; X++)
	    {
	      unsigned char v = test (X, Y);
	      if (!(v & FLAGA) || (v & FLAGI))
		continue;
	      S = X * 4;
	      int P = p[X][Y];
	      // Left
	      if (!(v & FLAGD))
		slice (S + 0, P + 0, P + 1);
	      slice (S + 0, P + 0, -(P + 5));
	      if (v & FLAGL)
		{
		  slice (S + 0, -(P + 4), -(P + 5));
		  slice (S + 0, -(P + 8), -(P + 9));
		}
	      slice (S + 0, P + 12, -(P + 9));
	      if (!(v & FLAGU))
		slice (S + 0, P + 12, P + 13);
	      // Middle
	      if (!(v & FLAGD))
		slice (S + 1, P + 1, P + 2);
	      slice (S + 1, -(P + 5), -(P + 6));
	      slice (S + 1, -(P + 9), -(P + 10));
	      if (!(v & FLAGU))
		slice (S + 1, P + 13, P + 14);
	      // Right
	      if (!(v & FLAGD))
		slice (S + 2, P + 2, P + 3);
	      slice (S + 2, -(P + 6), P + 3);
	      if (v & FLAGR)
		{
		  slice (S + 2, -(P + 6), -(P + 7));
		  slice (S + 2, -(P + 10), -(P + 11));
		}
	      slice (S + 2, -(P + 10), P + 15);
	      if (!(v & FLAGU))
		slice (S + 2, P + 14, P + 15);
	      {			// Joining to right
		int x = X + 1, y = Y;
		if (x >= W)
		  {
		    x -= W;
		    y += helix;
		  }
		if (y >= 0 && y < H)
		  {
		    int PR = p[x][y];
		    if (PR)
		      {
			slice (S + 3, P + 3, PR + 0);
			if (v & FLAGR)
			  {
			    slice (S + 3, -(P + 7), -(PR + 4));
			    slice (S + 3, -(P + 11), -(PR + 8));
			  }
			slice (S + 3, P + 15, PR + 12);
		      }
		  }
	      }
	    }
	// Top
	for (S = 0; S < W * 4; S++)
	  {
	    //slice (S, (s[S].l < 0 ? -1 : 1) * (top + S + (s[S].l < 0 ? W * 4 : 0)), (s[S].r < 0 ? -1 : 1) * (top + ((S + 1) % (W * 4)) + (s[S].r < 0 ? W * 4 : 0)));
	    slice (S, top + S + (s[S].l < 0 ? W * 4 : 0), top + ((S + 1) % (W * 4)) + (s[S].r < 0 ? W * 4 : 0));
	    slice (S, top + S + W * 4, top + ((S + 1) % (W * 4)) + W * 4);
	    slice (S, top + S + 2 * W * 4, top + ((S + 1) % (W * 4)) + 2 * W * 4);
	    slice (S, bottom + S, bottom + (S + 1) % (W * 4));
	  }
	printf ("]");
	printf (",convexity=10");
	// Done
	printf (");\n");
	if (parkthickness)
	  {			// Park ridge
	    if (inside && mirrorinside)
	      printf ("mirror([1,0,0])");
	    printf ("polyhedron(points=[");
	    for (N = 0; N < W; N += W / nubs)
	      for (Y = 0; Y < 4; Y++)
		for (X = 0; X < 4; X++)
		  {
		    int S = N * 4 + X + (parkvertical ? 0 : 2);
		    double z = y0 - dy * 1.5 / 4 + (helix + 1) * mazestep + Y * mazestep / 4 + dy * X / 4 + (parkvertical ? mazestep / 8 : dy / 2 - mazestep * 3 / 8);
		    double x = s[S].x[1];
		    double y = s[S].y[1];
		    if (parkvertical ? Y == 1 || Y == 2 : X == 1 || X == 2)
		      {		// ridge height instead or surface
			x = (s[S].x[1] * (mazethickness - parkthickness) + s[S].x[2] * parkthickness) / mazethickness;
			y = (s[S].y[1] * (mazethickness - parkthickness) + s[S].y[2] * parkthickness) / mazethickness;
		      }
		    else if (parkvertical)
		      z -= nubskew;
		    printf ("[%lld,%lld,%lld],", scaled (s[S].x[0]), scaled (s[S].y[0]), scaled (z));
		    printf ("[%lld,%lld,%lld],", scaled (x), scaled (y), scaled (z));
		  }
	    printf ("],faces=[");
	    for (N = 0; N < nubs; N++)
	      {
		int P = N * 32;
		inline void add (int a, int b, int c, int d)
		{
		  printf ("[%d,%d,%d],[%d,%d,%d],", P + a, P + b, P + c, P + a, P + c, P + d);
		}
		for (X = 0; X < 6; X += 2)
		  {
		    add (X + 0, X + 1, X + 3, X + 2);
		    for (Y = 0; Y < 24; Y += 8)
		      {
			add (X + 0 + Y, X + 2 + Y, X + 10 + Y, X + 8 + Y);
			add (X + 1 + Y, X + 9 + Y, X + 11 + Y, X + 3 + Y);
		      }
		    add (X + 25, X + 24, X + 26, X + 27);
		  }
		for (Y = 0; Y < 24; Y += 8)
		  {
		    add (Y + 0, Y + 8, Y + 9, Y + 1);
		    add (Y + 6, Y + 7, Y + 15, Y + 14);
		  }
	      }
	    printf ("],convexity=10);\n");
	  }
      }
    }
    printf ("translate([%lld,%lld,0])\n", scaled (x + (outersides & 1 ? r3 : r2)), scaled (y + (outersides & 1 ? r3 : r2)));
    if (outersides)
      printf ("rotate([0,0,%f])", (double) 180 / outersides + (part + 1 == parts ? 180 : 0));
    printf ("{\n");
    void mark (void)
    {				// Marking position 0
      if (!markpos0 || part + 1 < parts)
	return;
      double a = 0, r = r0 + wallthickness / 2, t = wallthickness * 2;
      if (mazeinside)
	r = r0 + mazethickness + wallthickness / 2;
      else if (mazeoutside)
	r = r1 - mazethickness - wallthickness / 2;
      if (!mazeoutside)
	{			// Try not to cut outside of box
	  r -= wallthickness / 2;
	  t = wallthickness * 3 / 2;
	}
      if (part == parts && mazeinside)
	a = (mirrorinside ? 1 : -1) * entrya;
      if (part + 1 == parts && mazeoutside)
	a = entrya;
      printf ("rotate([0,0,%f])translate([0,%lld,%lld])cylinder(d=%lld,h=%lld,center=true,$fn=4);\n", a, scaled (r), scaled (height), scaled (t), scaled (mazestep / 2));
    }
    // Maze
    printf ("difference(){union(){");
    if (mazeinside)
      makemaze (r0, 1);
    if (mazeoutside)
      makemaze (r1, 0);
    if (!mazeinside && !mazeoutside && part < parts)
      {
	printf ("difference(){\n");
	printf ("translate([0,0,%lld])cylinder(r=%lld,h=%lld,$fn=%d);translate([0,0,%lld])cylinder(r=%lld,h=%lld,$fn=%d);\n", scaled (basethickness / 2 - clearance), scaled (r1), scaled (height - basethickness / 2 + clearance), W * 4, scaled (basethickness), scaled (r0), scaled (height), W * 4);	// Non maze
	printf ("}\n");
      }
    // Base
    printf ("difference(){\n");
    if (part == parts)
      printf ("outer(%lld,%lld);\n", scaled (height), scaled ((r2 - outerround) / cos ((double) M_PI / (outersides ? : 100))));
    else if (part + 1 >= parts)
      printf ("mirror([1,0,0])outer(%lld,%lld);\n", scaled (baseheight), scaled ((r2 - outerround) / cos ((double) M_PI / (outersides ? : 100))));
    else
      printf ("hull(){cylinder(r=%lld,h=%lld,$fn=%d);translate([0,0,%lld])cylinder(r=%lld,h=%lld,$fn=%d);}\n", scaled (r2 - mazethickness), scaled (baseheight), W * 4, scaled (mazemargin), scaled (r2), scaled (baseheight - mazemargin), W * 4);
    printf ("translate([0,0,%lld])cylinder(r=%lld,h=%lld,$fn=%d);\n", scaled (basethickness), scaled (r0 + (part > 1 && mazeinside ? mazethickness + clearance : 0) + (!mazeinside && part < parts ? clearance : 0)), scaled (height), W * 4);	// Hole
    printf ("}\n");
    printf ("}\n");
    // Cut outs
    if (gripdepth && part + 1 < parts)
      printf ("rotate([0,0,%f])translate([0,0,%lld])rotate_extrude(convexity=10,$fn=%d)translate([%lld,0,0])circle(r=%lld,$fn=9);\n", (double) 360 / W / 4 / 2, scaled (mazemargin + (baseheight - mazemargin) / 2), W * 4, scaled (r2 + gripdepth), scaled (gripdepth * 2));
    else if (gripdepth && part + 1 == parts)
      printf ("translate([0,0,%lld])rotate_extrude(convexity=10,$fn=%d)translate([%lld,0,0])circle(r=%lld,$fn=9);\n", scaled (outerround + (baseheight - outerround) / 2), outersides ? : 100, scaled (r3 + gripdepth), scaled (gripdepth * 2));
    if (basewide && nextoutside && part + 1 < parts)	// Connect endpoints over base
      {
	int W = ((int) ((r2 - mazethickness) * 2 * M_PI / mazestep)) / nubs * nubs;
	double wi = 2 * (r2 - mazethickness) * 2 * M_PI / W / 4;
	double wo = 2 * r2 * 2 * M_PI * 3 / W / 4;
	printf ("for(a=[0:%f:359])rotate([0,0,a])translate([0,%lld,0])hull(){cube([%lld,%lld,%lld],center=true);cube([%lld,0.01,%lld],center=true);}\n", (double) 360 / nubs, scaled (r2), scaled (wi), scaled (mazethickness * 2), scaled (baseheight * 2 + clearance), scaled (wo),
		scaled (baseheight * 2 + clearance));
      }
    if (textend)
      {
	int n = 0;
	char *p = strdupa (textend);
	while (p)
	  {
	    char *q = strchr (p, '\\');
	    if (q)
	      *q++ = 0;
	    if (*p && n == (parts - part))
	      {
		printf ("rotate([0,0,%f])", (part == parts ? 1 : -1) * (90 + (double) 180 / (outersides ? : 100)));
		cuttext (r2 - outerround, p, textfontend, 0);
	      }
	    p = q;
	    n++;
	  }
      }
    void textside (int outset)
    {
      double a = 90 + (double) 180 / outersides;
      double h = r3 * sin (M_PI / outersides) * textsidescale;
      char *p = strdupa (textsides);
      while (p)
	{
	  char *q = strchr (p, '\\');
	  if (q)
	    *q++ = 0;
	  if (*p)
	    {
	      printf ("rotate([0,0,%f])translate([0,-%lld,%lld])rotate([-90,-90,0])", a, scaled (r2), scaled (outerround + (height - outerround) / 2));
	      cuttext (h, p, textfont, outset);
	    }
	  a -= 360 / outersides;
	  p = q;
	}
    }
    if (textsides && part == parts && outersides && !textoutset)
      textside (0);
    if (logo && part == parts)
      printf ("translate([0,0,%lld])linear_extrude(height=%lld,convexity=10)aa(%lld,white=true);\n", scaled (basethickness - logodepth), scaled (logodepth * 2), scaled (r0 * 1.8));
    else if (textinside)
      printf ("translate([0,0,%lld])linear_extrude(height=%lld,convexity=10)text(\"%s\",font=\"%s\",size=%lld,halign=\"center\",valign=\"center\");\n", scaled (basethickness - logodepth), scaled (logodepth * 2), textinside, textfontend, scaled (r0));
    if (markpos0 && part + 1 >= parts)
      mark ();
    printf ("}\n");
    if (textsides && part == parts && outersides && textoutset)
      textside (1);
    if (coresolid && part == 1)
      printf ("translate([0,0,%lld])cylinder(r=%lld,h=%lld,$fn=%d);\n", scaled (basethickness), scaled (r0 + clearance + (!mazeinside && part < parts ? clearance : 0)), scaled (height - basethickness), W * 4);	// Solid core
    if ((mazeoutside && !flip && part == parts) || (!mazeoutside && part + 1 == parts))
      entrya = 0;		// Align for lid alignment
    else if (part < parts && !basewide)
      {				// We can position randomly
	int v;
	if (read (f, &v, sizeof (v)) != sizeof (v))
	  err (1, "Read /dev/random");
	entrya = v % 360;
      }
    // Nubs
    void addnub (double r, int inside)
    {
      double ri = r + (inside ? -mazethickness : mazethickness);
      int W = ((int) ((ri + (inside ? -clearance : clearance)) * 2 * M_PI / mazestep)) / nubs * nubs;
      double da = (double) 2 * M_PI / W / 4;	// x angle per 1/4 maze step
      double dz = mazestep / 4 - nubzclearance;
      double my = mazestep * da * 4 * helix / (r * 2 * M_PI);
      if (inside)
	da = -da;
      else if (mirrorinside)
	my = -my;		// This is nub outside which is for inside maze
      double a = -da * 1.5;	// Centre A
      double z = height - mazestep / 2 - (parkvertical ? 0 : mazestep / 8) - dz * 1.5 - my * 1.5;	// Centre Z
      printf ("rotate([0,0,%f])for(a=[0:%f:359])rotate([0,0,a])polyhedron(points=[", entrya, (double) 360 / nubs);
      r += (inside ? nubrclearance : -nubrclearance);	// Extra gap
      ri += (inside ? nubrclearance : -nubrclearance);	// Extra gap
      for (Z = 0; Z < 4; Z++)
	for (X = 0; X < 4; X++)
	  printf ("[%lld,%lld,%lld],", scaled (((X == 1 || X == 2) && (Z == 1 || Z == 2) ? ri : r) * sin (a + da * X)), scaled (((X == 1 || X == 2) && (Z == 1 || Z == 2) ? ri : r) * cos (a + da * X)), scaled (z + Z * dz + X * my + (Z == 1 || Z == 2 ? nubskew : 0)));
      r += (inside ? clearance - nubrclearance : -clearance + nubrclearance);	// Back in to wall
      for (Z = 0; Z < 4; Z++)
	for (X = 0; X < 4; X++)
	  printf ("[%lld,%lld,%lld],", scaled (r * sin (a + da * X)), scaled (r * cos (a + da * X)), scaled (z + Z * dz + X * my + (Z == 1 || Z == 2 ? nubskew : 0)));
      printf ("],faces=[");
      for (Z = 0; Z < 3; Z++)
	for (X = 0; X < 3; X++)
	  printf ("[%d,%d,%d],[%d,%d,%d],", Z * 4 + X + 20, Z * 4 + X + 21, Z * 4 + X + 17, Z * 4 + X + 20, Z * 4 + X + 17, Z * 4 + X + 16);
      for (Z = 0; Z < 3; Z++)
	printf ("[%d,%d,%d],[%d,%d,%d],[%d,%d,%d],[%d,%d,%d],", Z * 4 + 4, Z * 4 + 20, Z * 4 + 16, Z * 4 + 4, Z * 4 + 16, Z * 4 + 0, Z * 4 + 23, Z * 4 + 7, Z * 4 + 3, Z * 4 + 23, Z * 4 + 3, Z * 4 + 19);
      for (X = 0; X < 3; X++)
	printf ("[%d,%d,%d],[%d,%d,%d],[%d,%d,%d],[%d,%d,%d],", X + 28, X + 12, X + 13, X + 28, X + 13, X + 29, X + 0, X + 16, X + 17, X + 0, X + 17, X + 1);
      printf ("[0,1,5],[0,5,4],[4,5,9],[4,9,8],[8,9,12],[9,13,12],");
      printf ("[1,2,6],[1,6,5],[5,6,10],[5,10,9],[9,10,14],[9,14,13],");
      printf ("[2,3,6],[3,7,6],[6,7,11],[6,11,10],[10,11,15],[10,15,14],");
      printf ("]);\n");
    }
    if (!mazeinside && part > 1)
      addnub (r0, 1);
    if (!mazeoutside && part < parts)
      addnub (r1, 0);
    printf ("}\n");
    x += (outersides & 1 ? r3 : r2) + r2 + 5;
    if (++n >= sq)
      {
	n = 0;
	x = 0;
	y += (outersides & 1 ? r3 : r2) * 2 + 5;
      }
  }

  printf ("scale(" SCALEI "){\n");
  if (part)
    box (part);
  else
    for (part = 1; part <= parts; part++)
      box (part);
  printf ("}\n");
  close (f);
  return 0;
}
