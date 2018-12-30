// Puzzle box maker
// (c) 2018 Adrian Kennard www.me.uk @TheRealRevK

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

#define	L 1
#define R 2
#define U 4
#define D 8
#define	A 15

int
main (int argc, const char *argv[])
{
  double basethickness = 1.6;
  double basegap = 0.4;
  double baseheight = 10;
  double corediameter = 10;
  double coreheight = 50;
  double wallthickness = 1.75;
  double mazethickness = 1.75;
  double mazestep = 3;
  double clearance = 0.25;
  double coregap = 0;
  double outerround = 2;
  double mazemargin = 1;
  double parkheight = 0.6;
  double textdepth = 0.5;
  double logodepth = 0.6;
  double gripdepth = 2;
  char *textend = NULL;
  char *textsides = NULL;
  char *textfont = NULL;
  int walls = 4;
  int wall = 0;
  int inside = 0;
  int outersides = 7;
  int testmaze = 0;
  int helix = 2;
  int nubs = 2;
  int logo = 0;
  int textslow = 0;
  int symmectriccut = 0;
  int mime = (getenv ("HTTP_HOST") ? 1 : 0);

  char pathsep = 0;
  char *path = getenv ("PATH_INFO");
  if (path)
    pathsep = '/';
  else if ((path = getenv ("QUERY_STRING")))
    pathsep = '&';

  const struct poptOption optionsTable[] = {
    {"inside", 'i', POPT_ARG_NONE, &inside, 0, "Maze on inside (hard)"},
    {"wall", 'n', POPT_ARG_INT, &wall, 0, "Wall", "N"},
    {"walls", 'm', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &walls, 0, "Walls", "N"},
    {"nubs", 'N', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &nubs, 0, "Nubs", "N"},
    {"helix", 'H', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &helix, 0, "Helix", "N"},
    {"base-height", 'b', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &baseheight, 0, "Base height", "mm"},
    {"core-diameter", 'c', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &corediameter, 0, "Core diameter", "mm"},
    {"core-height", 'h', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coreheight, 0, "Core height", "mm"},
    {"core-gap", 'C', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coregap, 0, "Core gap", "mm"},
    {"base-thickness", 'B', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &basethickness, 0, "Base thickness", "mm"},
    {"base-gap", 'G', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &basegap, 0, "Base gap", "mm"},
    {"wall-thickness", 'w', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &wallthickness, 0, "Wall thickness", "mm"},
    {"maze-thickness", 't', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazethickness, 0, "Maze thickness", "mm"},
    {"maze-step", 'z', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazestep, 0, "Maze step", "mm"},
    {"maze-margin", 'M', POPT_ARG_DOUBLE | (mazemargin ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &mazemargin, 0, "Maze top margin", "mm"},
    {"park-height", 'p', POPT_ARG_DOUBLE | (parkheight ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &parkheight, 0, "Height of park ridge", "mm"},
    {"clearance", 'g', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &clearance, 0, "Clearance", "mm"},
    {"outer-sides", 's', POPT_ARG_INT | (outersides ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &outersides, 0, "Outer sides", "N"},
    {"outer-round", 'r', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &outerround, 0, "Outer rounding", "mm"},
    {"grip-depth", 'R', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &gripdepth, 0, "Grip depth", "mm"},
    {"text-depth", 'D', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &textdepth, 0, "Text depth", "mm"},
    {"text-end", 'E', POPT_ARG_STRING | (textend ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &textend, 0, "Text (initial) on end", "X"},
    {"text-font", 'F', POPT_ARG_STRING | (textfont ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &textfont, 0, "Text font", "Font"},
    {"text-side", 'S', POPT_ARG_STRING | (textsides ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &textsides, 0, "Text on sides", "Line1\\Line2..."},
    {"text-slow", 'Z', POPT_ARG_NONE, &textslow, 0, "Text with edges (slow)"},
    {"symmetric-cut", 'V', POPT_ARG_NONE, &symmectriccut, 0, "Symmetric maze cut"},
    {"logo", 'A', POPT_ARG_NONE, &logo, 0, "Include A&A logo"},
    {"text-depth", 'L', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &logodepth, 0, "Logo depth", "mm"},
    {"test", 'Q', POPT_ARG_NONE, &testmaze, 0, "Test pattern instead of maze"},
    {"mime", 0, POPT_ARG_NONE | (mime ? POPT_ARGFLAG_DOC_HIDDEN : 0), &mime, 0, "MIME Header"},
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
	      asprintf (&error, "Path error [%s]\n", path);
	      break;
	    }
	  char arg = *path++;
	  int o;
	  for (o = 0; optionsTable[o].longName && optionsTable[o].shortName != arg; o++);
	  if (!optionsTable[o].shortName)
	    {
	      asprintf (&error, "Unknown arg [%c]", arg);
	      break;
	    }
	  if (optionsTable[o].arg)
	    {
	      if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_INT && *path == '=')
		*(int *) optionsTable[o].arg = strtod (path + 1, &path);
	      else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_DOUBLE && *path == '=')
		*(double *) optionsTable[o].arg = strtod (path + 1, &path);
	      else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_NONE)
		{
		  *(int *) optionsTable[o].arg = 1;
		  if (*path == '=')
		    {		// Skip =on, etc.
		      path++;
		      while (*path && *path != pathsep)
			path++;
		    }
		}
	      else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_STRING && *path == '=')
		{
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
		}
	    }
	}
    }

  // Sanity checks and adjustments
  if (!outersides)
    textsides = NULL;
  if (textfont && !*textfont)
    textfont = NULL;
  if (textend && !*textend)
    textend = NULL;
  if (textsides && !*textsides)
    textsides = NULL;
  if (helix && nubs && nubs < helix)
    nubs = helix / (helix / nubs);
  if (helix && nubs > helix)
    nubs = helix;
  if (outersides)
    outersides = (outersides + nubs - 1) / nubs * nubs;
  if (gripdepth > (baseheight - outerround) / 6)
    gripdepth = (baseheight - outerround) / 6;
  if (textsides)
    {
      char *p;
      for (p = textsides; *p; p++)
	if (*p == '"')
	  *p = '\'';
    }
  if (textend)
    {
      char *p;
      for (p = textend; *p; p++)
	if (*p == '"')
	  *p = '\'';
	else if (*p == '\\')
	  *p = '/';
    }
  if (!logo)
    logodepth = 0;
  if (!textsides && !textend)
    textdepth = 0;
  if (basethickness < logodepth + (textend ? textdepth : 0) + 0.4)
    basethickness = logodepth + (textend ? textdepth : 0) + 0.4;

  // MIME header
  if (mime)
    {
      printf ("Content-Type: application/scad\r\nContent-Disposition: Attachment; filename=puzzlebox");
      int o;
      for (o = 0; optionsTable[o].longName; o++)
	if (optionsTable[o].shortName && optionsTable[o].arg)
	  {
	    if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_NONE && *(int *) optionsTable[o].arg)
	      printf ("-%c", optionsTable[o].shortName);
	    else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_INT && *(int *) optionsTable[o].arg)
	      printf ("-%d%c", *(int *) optionsTable[o].arg, optionsTable[o].shortName);
	    else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_DOUBLE && *(double *) optionsTable[o].arg)
	      {
		char temp[50], *p;
		sprintf (temp, "%f", *(double *) optionsTable[o].arg);
		p = temp + strlen (temp) - 1;
		while (*p == '0')
		  *p-- = 0;
		p = strchr (temp, '.');
		if (*p)
		  *p++ = 0;
		printf ("-%s%c%s", temp, optionsTable[o].shortName, p);
	      }
	    else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_STRING && *(int *) optionsTable[o].arg)
	      {			// String, but sanity check for filename
		char *p = strdupa (*(char * *) optionsTable[o].arg), *q;
		for (q = p; *q; q++)
		  if (*q <= ' ' || *q == '/' || *q == '\\' || *q == '"' || *q == '\'' || *q == ':')
		    *q = '_';
		printf ("-%c%s", optionsTable[o].shortName, p);
	      }

	  }
      printf (".scad\r\n\r\n");	// Used from apache
    }

  printf ("// Puzzlebox by RevK, @TheRealRevK www.me.uk\n");
  printf ("// Thingiverse examples and instructions https://www.thingiverse.com/thing:2410748\n");
  printf ("// GitHub source https://github.com/revk/PuzzleBox\n");
  {				// Document args
    time_t now = time (0);
    struct tm t;
    localtime_r (&now, &t);
    printf ("// Created %04d-%02d-%02d %02d:%02d:%02d %s\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, getenv ("REMOTE_ADDR") ? : "");
    int o;
    for (o = 0; optionsTable[o].longName; o++)
      if (optionsTable[o].shortName && optionsTable[o].arg)
	{
	  if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_NONE && *(int *) optionsTable[o].arg)
	    printf ("// %s: %c\n", optionsTable[o].descrip, optionsTable[o].shortName);
	  else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_INT && *(int *) optionsTable[o].arg)
	    printf ("// %s: %c=%d\n", optionsTable[o].descrip, optionsTable[o].shortName, *(int *) optionsTable[o].arg);
	  else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_DOUBLE && *(double *) optionsTable[o].arg)
	    printf ("// %s: %c=%f\n", optionsTable[o].descrip, optionsTable[o].shortName, *(double *) optionsTable[o].arg);
	  else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_STRING && *(int *) optionsTable[o].arg)
	    printf ("// %s: %c=%s\n", optionsTable[o].descrip, optionsTable[o].shortName, *(char * *) optionsTable[o].arg);
	}
  }
  if (error)
    {				// Problem
      printf ("// ** %s **\n", error);
      return 1;
    }

  double nubskew = (symmectriccut ? 0 : mazestep / 8);	// Skew the shape of the cut

  {				// Modules
    printf ("module nub(){rotate([%d,0,0])translate([0,0,-0.1])hull(){cube([%f,%f,0.1],center=true);translate([0,%f,%f])cube([%f,%f,0.1],center=true);};}\n", inside ? -90 : 90, mazestep * 3 / 4, mazestep * 3 / 4, nubskew, mazethickness - clearance / 2, mazestep / 4, mazestep / 4);
    if (parkheight)
      printf ("module park(){rotate([%d,0,0])translate([0,%f,%f])hull(){cube([%f,%f,0.1],center=true);translate([0,0,%f])cube([%f,%f,0.1],center=true);}}\n", inside ? -90 : 90, -nubskew, mazethickness - parkheight, mazestep, mazestep / 4, parkheight, mazestep, mazestep * 3 / 4);
    if (textslow)
      {
	printf ("module cuttext(s,t){translate([0,0,-1])minkowski(){rotate([0,0,45])cylinder(h=%f,d1=%f,d2=0,$fn=4);linear_extrude(height=1,convexity=2)mirror([1,0,0])text(t,valign=\"center\",halign=\"center\",size=s", textdepth, textdepth);
	if (textfont)
	  printf (",font=\"%s\"", textfont);
	printf (");}}\n");
      }
    else
      {
	printf ("module cuttext(s,t){linear_extrude(height=%f,convexity=2,center=true)mirror([1,0,0])text(t,valign=\"center\",halign=\"center\",size=s", textdepth, textdepth, textdepth * 2);
	printf (",font=\"%s\"", textfont);
	printf (");}\n");
      }
    // You can use the A&A logo on your maze print providing it is tasteful and not in any way derogatory to A&A or any staff/officers.
    if (logo)
      printf
	("module aa(w=100,white=0,$fn=100){scale(w/100){if(!white)difference(){circle(d=100.5);circle(d=99.5);}difference(){if(white)circle(d=100);difference(){circle(d=92);for(m=[0,1])mirror([m,0,0]){difference(){translate([24,0,0])circle(r=22.5);translate([24,0,0])circle(r=15);}polygon([[1.5,22],[9,22],[9,-18.5],[1.5,-22]]);}}}}} // A&A Logo is copyright (c) 2013 and trademark Andrews & Arnold Ltd\n");
  }
  // The base
  printf ("module outer(h,r){e=%f;minkowski(){cylinder(r1=0,r2=e,h=e,$fn=100);cylinder(h=h-e,r=r,$fn=%d);}}\n", outerround, outersides ? : 100);
  double x = 0;
  int box (int wall)
  {				// Make the box - wall 1 in inside
    // Dimensions
    double r1 = corediameter / 2 + wallthickness + (wall - 1) * (wallthickness + mazethickness + clearance);	// Outer
    double r0 = r1 - wallthickness;	// Inner
    double r2 = r1;		// Base outer
    if (wall < walls)
      r2 += wallthickness + mazethickness + clearance;
    if (!inside && wall < walls)
      {				// Allow for maze on outside
	r1 += mazethickness;
	if (wall + 1 < walls)
	  r2 += mazethickness;
      }
    if (inside && wall > 1)
      {				// Allow for maze on inside
	r0 -= mazethickness;
      }
    double r3 = r2;		// Base outer before adjust for sides
    if (outersides && wall + 1 >= walls)
      r2 /= cos ((double) M_PIl / outersides);
    double height = coreheight + basethickness + (basethickness + basegap) * (wall - 1);
    if (wall == 1)
      height -= coregap;
    if (wall > 1)
      height -= baseheight;	// base from previous unit is added to this
    // Output
    // Maze dimensions
    double r = (inside ? r0 : r1);
    double base = (inside ? basethickness : baseheight);
    if (inside && wall > 2)
      base += baseheight;	// Nubs don't go all the way to the end
    if (inside && wall == 2)
      base += coregap;		// First one is short...
    if (inside)
      base += basegap;
    double h = height - base - mazemargin - mazestep / 8;
    double w = r * 2 * M_PIl;
    int H = (int) (h / mazestep);
    int W = ((int) (w / mazestep)) / nubs * nubs;
    double y0 = base + mazestep / 2 - mazestep * (helix + 1) + (h - (mazestep * H));
    H += 2 + helix;		// Allow one above, one below and helix below
    if (W < 3 || H < 1)
      errx (1, "Too small");
    double a = 0, dy = 0;
    if (helix)
      {
	a = atan (mazestep * helix / w) * 180 / M_PIl;
	dy = mazestep * helix / W;
      }
    if ((!inside && wall < walls) || (inside && wall > 1))
      printf ("// Wall %d (%d/%d)\n", wall, W, H);
    else
      printf ("// Wall %d\n", wall);
    int entry = 0;		// Entry point and pips
    int X, Y, N, S;
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
	    v |= 0x80;
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
    printf ("translate([%f,0,0]){\n", x + r2);
    printf ("difference(){\n");
    if (wall == walls)
      printf ("outer(%f,%f);\n", height, (r3 - outerround) / cos ((double) M_PIl / outersides));
    else if (wall + 1 >= walls)
      printf ("mirror([1,0,0])outer(%f,%f);\n", baseheight, (r3 - outerround) / cos ((double) M_PIl / outersides));
    else
      printf ("hull(){cylinder(r=%f,h=%f,$fn=%d);translate([0,0,%f])cylinder(r=%f,h=%f,$fn=%d);}\n", r2 - mazethickness, baseheight, W * 4, mazemargin, r2, baseheight - mazemargin, W * 4);
    if (gripdepth && wall + 1 < walls)
      printf ("translate([0,0,%f])rotate_extrude(convexity=4,$fn=%d)translate([%f,0,0])circle(r=%f,$fn=100);\n", mazemargin + (baseheight - mazemargin) / 2, W * 4, r2 + gripdepth, gripdepth * 2);
    else if (gripdepth && wall + 1 == walls)
      printf ("translate([0,0,%f])rotate_extrude(convexity=4,$fn=%d)translate([%f,0,0])circle(r=%f,$fn=100);\n", outerround + (baseheight - outerround) / 2, outersides ? : 100, r2 + gripdepth, gripdepth * 2);
    if (!inside && wall + 1 < walls)
      {				// Connect endpoints over base
	int N;
	for (N = 0; N < nubs; N++)
	  printf ("rotate([0,0,%f])translate([0,%f,%f])hull(){nub();translate([0,0,%f])nub();}\n", -(double) N * 360 / nubs, r2, -mazestep, baseheight + mazestep);
      }
    printf ("translate([0,0,%f])cylinder(r=%f,h=%f,$fn=%d);\n", basethickness, r0 + (wall > 1 && inside ? mazethickness + clearance : 0) + (!inside && wall < walls ? clearance : 0), height, W * 4);	// Hole
    if (textend && wall + 1 >= walls)
      printf ("cuttext(%f,\"%s\");\n", r3 - outerround, textend);
    if (textsides && wall == walls && outersides)
      {
	double a = 90 + 180 / outersides;
	double h = r2 * sin (M_PIl / outersides);
	char *p = strdupa (textsides);
	while (p)
	  {
	    char *q = strchr (p, '\\');
	    if (q)
	      *q++ = 0;
	    if (*p)
	      printf ("rotate([0,0,%f])translate([0,%f,%f])rotate([90,90,0])cuttext(%f,\"%s\");", a, r3, outerround + (height - outerround) / 2, h, p);
	    a += 360 / outersides;
	    p = q;
	  }
      }
    if (logo && wall == walls)
      printf ("translate([0,0,%f])linear_extrude(height=%f,convexity=4)aa(%f,white=true);\n", basethickness - logodepth, basethickness, r0 * 1.8);
    printf ("}\n");
    if ((!inside && wall < walls) || (inside && wall > 1))
      {				// Maze
	double margin = mazemargin;
	if (!inside && wall > 1)
	  margin = 0;
	// Make maze
	int f = open ("/dev/urandom", O_RDONLY);
	if (f < 0)
	  err (1, "Open /dev/random");
	if (read (f, &entry, sizeof (entry)) != sizeof (entry))
	  err (1, "Read /dev/random");
	entry = 1 + (entry % (W / nubs - 1));	// Random entry point not matching exit
	// Clear too high/low
	for (Y = 0; Y < H; Y++)
	  for (X = 0; X < W; X++)
	    if (mazestep * Y + y0 + dy * X < base + mazestep / 2 + mazestep / 8 || mazestep * Y + y0 + dy * X > height - mazestep / 2 - margin - mazestep / 8)
	      maze[X][Y] |= 0x80;	// To high or low
	// Final park point, up one, and down off bottom
	for (N = 0; N < helix + 2; N++)
	  {
	    maze[0][N] |= U + D;
	    maze[0][N + 1] |= D;
	  }
	// Make maze
	if (testmaze)
	  {			// Simple test pattern
	    for (Y = 0; Y < H; Y++)
	      for (X = 0; X < W; X++)
		if (!(test (X, Y) & 0x80) && !(test (X + 1, Y) & 0x80))
		  {
		    maze[X][Y] |= R;
		    int x = X + 1, y = Y;
		    if (x >= W)
		      {
			x -= W;
			y += helix;
		      }
		    maze[x][y] |= L;
		  }
	  }
	else
	  {			// Actual maze
	    int x[W * H], y[W * H], p = 0;
	    x[0] = 0;		// Start at park point of maze as we have done the U/D to get there from below base
	    y[0] = helix + 2;
	    p++;
	    while (p)
	      {
		X = x[p - 1];
		Y = y[p - 1];
		unsigned int v, n = 0;
		if (read (f, &v, sizeof (v)) != sizeof (v))
		  err (1, "Read /dev/random");
		// Some bias for direction
		if (!test (X + 1, Y))
		  n += 3;
		if (!test (X - 1, Y))
		  n += 3;
		if (!test (X, Y - 1))
		  n += 2;
		if (!test (X, Y + 1))
		  n++;
		if (!n)
		  {
		    p--;
		    continue;
		  }
		v %= n;
		if (!test (X + 1, Y) && (!v-- || !v-- || !v--))
		  {
		    maze[X][Y] |= R;
		    X++;
		    if (X >= W)
		      {
			X -= W;
			Y += helix;
		      }
		    maze[X][Y] |= L;
		  }
		else if (!test (X - 1, Y) && (!v-- || !v-- || !v--))
		  {
		    maze[X][Y] |= L;
		    X--;
		    if (X < 0)
		      {
			X += W;
			Y -= helix;
		      }
		    maze[X][Y] |= R;
		  }
		else if (!test (X, Y - 1) && (!v-- || !v--))
		  {
		    maze[X][Y] |= D;
		    Y--;
		    maze[X][Y] |= U;
		  }
		else if (!test (X, Y + 1) && !v--)
		  {
		    maze[X][Y] |= U;
		    Y++;
		    maze[X][Y] |= D;
		  }
		else
		  errx (1, "WTF");
		if (p == W * H)
		  errx (1, "WTF");
		x[p] = X;
		y[p] = Y;
		p++;
	      }
	    close (f);
	  }
	// Entry point for maze
	for (X = entry; X < W; X += W / nubs)
	  {
	    Y = H - 1;
	    while (Y && (maze[X][Y] & 0x80))
	      maze[X][Y--] |= U + D;
	    maze[X][Y] += U;
	  }
	{			// Construct maze polyhedron
	  int MAXY = height / (mazestep / 4) + 8;
	  struct
	  {			// Data for each slive
	    // Pre calculated x/y for left side 0=back, 1=recess, 2=front - used to create points
	    double x[3], y[3];
	    // The last points as we work up slice (-ve for recess, 0 for not set yet)
	    int l, r;
	    // Points from bottom up on this slice in order - used to create manifold
	    int n;		// Points added to p
	    int p[MAXY];
	  } s[W * 4];
	  memset (&s, 0, sizeof (*s) * W * 4);
	  int p[W][H];		// The point start for each usable maze location (0 for not set) - 16 points
	  memset (*p, 0, sizeof (int) * W * H);
	  // Work out pre-sets
	  double r = r1;
	  if (wall == walls)
	    r = r0 + mazethickness + clearance;
	  for (S = 0; S < W * 4; S++)
	    {
	      double a = M_PIl * 2 * (S - 1.5) / W / 4;
	      if (!inside)
		a = M_PIl * 2 - a;
	      double sa = sin (a), ca = cos (a);
	      if (inside)
		{
		  s[S].x[0] = r * sa;
		  s[S].y[0] = r * ca;
		  s[S].x[1] = (r0 + mazethickness) * sa;
		  s[S].y[1] = (r0 + mazethickness) * ca;
		  s[S].x[2] = r0 * sa;
		  s[S].y[2] = r0 * ca;
		}
	      else
		{
		  s[S].x[0] = r0 * sa;
		  s[S].y[0] = r0 * ca;
		  s[S].x[1] = (r1 - mazethickness) * sa;
		  s[S].y[1] = (r1 - mazethickness) * ca;
		  s[S].x[2] = r1 * sa;
		  s[S].y[2] = r1 * ca;
		}
	    }
	  printf ("polyhedron(");
	  // Make points
	  printf ("points=[");
	  int P = 0;
	  void addpoint (int S, double x, double y, double z)
	  {
	    printf ("[%f,%f,%f],", x, y, z);
	    if (s[S].n >= MAXY)
	      errx (1, "WTF points");
	    s[S].p[s[S].n++] = P++;
	  }
	  void addpointr (int S, double x, double y, double z)
	  {
	    printf ("[%f,%f,%f],", x, y, z);
	    if (s[S].n >= MAXY)
	      errx (1, "WTF points");
	    s[S].p[s[S].n++] = -(P++);
	  }
	  int bottom = P;
	  // Base points
	  for (S = 0; S < W * 4; S++)
	    addpoint (S, s[S].x[0], s[S].y[0], basethickness / 2);
	  for (S = 0; S < W * 4; S++)
	    addpointr (S, s[S].x[1], s[S].y[1], basethickness / 2);
	  for (S = 0; S < W * 4; S++)
	    addpoint (S, s[S].x[2], s[S].y[2], basethickness / 2);
	  {			// Points for each maze location
	    double dy = mazestep * helix / W / 4;	// Step per S
	    double my = mazestep / 8;	// Vertical steps
	    double y = y0 - dy * 1.5;	// Y vertical centre for S=0
	    for (Y = 0; Y < H; Y++)
	      for (X = 0; X < W; X++)
		{
		  unsigned char v = test (X, Y);
		  if (!(v & A) || (v & 0x80))
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
	    addpoint (S, s[S].x[2], s[S].y[2], height - margin);	// lower
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
	      errx (1, "Bad render");
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
	      errx (1, "Bad render");
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
		if (!(v & A) || (v & 0x80))
		  continue;
		int P = p[X][Y];
		S = X * 4;
		// Left
		if (!(v & D))
		  slice (S + 0, P + 0, P + 1);
		slice (S + 0, P + 0, -(P + 5));
		if (v & L)
		  {
		    slice (S + 0, -(P + 4), -(P + 5));
		    slice (S + 0, -(P + 8), -(P + 9));
		  }
		slice (S + 0, P + 12, -(P + 9));
		if (!(v & U))
		  slice (S + 0, P + 12, P + 13);
		// Middle
		if (!(v & D))
		  slice (S + 1, P + 1, P + 2);
		slice (S + 1, -(P + 5), -(P + 6));
		slice (S + 1, -(P + 9), -(P + 10));
		if (!(v & U))
		  slice (S + 1, P + 13, P + 14);
		// Right
		if (!(v & D))
		  slice (S + 2, P + 2, P + 3);
		slice (S + 2, -(P + 6), P + 3);
		if (v & R)
		  {
		    slice (S + 2, -(P + 6), -(P + 7));
		    slice (S + 2, -(P + 10), -(P + 11));
		  }
		slice (S + 2, -(P + 10), P + 15);
		if (!(v & U))
		  slice (S + 2, P + 14, P + 15);
		{		// Joining to right
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
			  if (v & R)
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
	  printf (",\nconvexity=4");
	  // Done
	  printf (");\n");
	}
      }
    else if (wall < walls)	// Non maze
      printf ("difference(){translate([0,0,%f])cylinder(r=%f,h=%f,$fn=%d);translate([0,0,%f])cylinder(r=%f,h=%f,$fn=%d);}\n", basethickness / 2, r1, height - basethickness / 2, W * 4, basethickness, r0, height, W * 4);
    if (parkheight && ((!inside && wall < walls) || (inside && wall > 1)))
      {				// Park ridge
	for (X = 0; X < W; X += W / nubs)
	  printf ("rotate([0,0,%f])translate([0,%f,%f])park();\n", (double) X * 360 / W, r, base + mazestep);
      }
    if (inside && wall + 1 >= walls)
      entry = 0;		// Nubs needs to align for outside to align when closed
    if ((!inside && wall > 1) || (inside && wall < walls))
      {				// Nubs
	double rn = (inside ? r1 : r0);
	for (X = entry; X < W; X += W / nubs)
	  printf ("rotate([0,0,%f])translate([0,%f,%f])nub();\n", (double) X * 360 / W, rn, height - mazestep / 2);
      }
    printf ("}\n");
    x += r2 * 2 + 10;
  }

  if (wall)
    box (wall);
  else
    for (wall = 1; wall <= walls; wall++)
      box (wall);
  return 0;
}
