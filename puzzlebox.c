// Puzzle box maker

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

#define 	PI	3.1415926

int
main (int argc, const char *argv[])
{
  char *path = getenv ("PATH_INFO");
  double baseheight = 5;
  double corediameter = 10;
  double coreheight = 50;
  double wallthickness = 2;
  double mazethickness = 2;
  double mazestep = 3;
  double clearance = 0.25;
  double coregap = 0;
  int curvesteps = 100;
  int walls = 4;
  int wall = 0;
  int inside = 0;
  int sides = 7;
  int testmaze = 0;
  int helix = 2;
  int nubs = 2;
  int nubdetail = 8;
  int mime = (getenv ("HTTP_HOST") ? 1 : 0);

  const struct poptOption optionsTable[] = {
    {"inside", 'i', POPT_ARG_NONE, &inside, 0, "Maze on inside (hard)"},
    {"wall", 'n', POPT_ARG_INT, &wall, 0, "Wall", "N"},
    {"walls", 'm', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &walls, 0, "Walls", "N"},
    {"sides", 'x', POPT_ARG_INT | (sides ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &sides, 0, "Outer sides", "N"},
    {"nubs", 'N', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &nubs, 0, "Nubs", "N"},
    {"helix", 'H', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &helix, 0, "Helix", "N"},
    {"test", 't', POPT_ARG_NONE, &testmaze, 0, "Test pattern instead of maze"},
    {"base-height", 'b', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &baseheight, 0, "Base height", "mm"},
    {"core-diameter", 'c', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &corediameter, 0, "Core diameter", "mm"},
    {"core-height", 'h', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coreheight, 0, "Core height", "mm"},
    {"core-gap", 'C', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coregap, 0, "Core gap", "mm"},
    {"wall-thickness", 'w', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &wallthickness, 0, "Wall thickness", "mm"},
    {"maze-thickness", 'd', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazethickness, 0, "Maze thickness", "mm"},
    {"maze-step", 's', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazestep, 0, "Maze step", "mm"},
    {"clearance", 'g', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &clearance, 0, "Clearance", "mm"},
    {"curve-steps", 'a', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &curvesteps, 0, "Curve steps", "N"},
    {"mime", 0, POPT_ARG_NONE | (mime ? POPT_ARGFLAG_DOC_HIDDEN : 0), &mime, 0, "MIME Header"},
    {"path", 0, POPT_ARG_STRING | (path ? POPT_ARGFLAG_SHOW_DEFAULT : 0), &path, 0, "Path header", "{/x=var}"},
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
      while (*path)
	{
	  if (*path == '/')
	    {
	      path++;
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
		*(int *) optionsTable[o].arg = 1;
	    }
	}
    }

  // Sanity checks
  if (testmaze)
    {
      curvesteps = 20;
      nubdetail = 4;
    }
  if (helix && nubs && nubs < helix)
    nubs = helix / (helix / nubs);
  if (helix && nubs > helix)
    nubs = helix;

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
	    printf ("// %s\n", optionsTable[o].descrip);
	  else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_INT && *(int *) optionsTable[o].arg)
	    printf ("// %s=%d\n", optionsTable[o].descrip, *(int *) optionsTable[o].arg);
	  else if ((optionsTable[o].argInfo & POPT_ARG_MASK) == POPT_ARG_DOUBLE && *(double *) optionsTable[o].arg)
	    printf ("// %s=%f\n", optionsTable[o].descrip, *(double *) optionsTable[o].arg);
	}
  }
  if (error)
    {				// Problem
      printf ("// ** %s **\n", error);
      return 1;
    }

  if (curvesteps)
    printf ("$fn=%d;\n", curvesteps);
  // The nub
  printf ("module nub(){rotate([%d,0,0])translate([0,0,%f])cylinder(d1=%f,d2=%f,h=%f,$fn=%d);}\n", inside ? -90 : 90, -mazethickness, mazestep, mazestep / 3, mazethickness * 2, nubdetail);
  double x = 0;
  void box (int wall)
  {				// Make the box - wall 1 in inside
    // Dimensions
    double r1 = corediameter / 2 + wallthickness + (wall - 1) * (wallthickness + mazethickness + clearance);
    double r0 = r1 - wallthickness;
    double r2 = r1;
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
    if (sides && wall + 1 >= walls)
      {
	r2 /= cos ((double) PI / sides);
	if (wall == walls)
	  r1 = r2;
      }
    double height = coreheight + wallthickness * wall;
    if (wall == 1)
      height -= coregap;
    if (wall > 1)
      height -= baseheight;
    // Output
    // Maze dimensions
    double r = (inside ? r0 : r1);
    double base = (inside ? wallthickness : baseheight);
    if (inside && wall > 1)
      base += baseheight;	// Nubs don't go all the way to the end
    double y0 = base;
    double h = height - base;
    double w = r * 2 * PI;
    int H = (int) (h / mazestep);
    int W = ((int) (w / mazestep)) / (nubs * 2) * (nubs * 2);
    y0 += (h - (mazestep * (H - 1))) / 2;	// Centre
    double a = 0, dy = 0;
    if (helix)
      {
	a = atan (mazestep * helix / w) * 180 / PI;
	dy = mazestep * helix / W;
      }
    printf ("// Wall %d (%d/%d)\n", wall, W, H);
    if (helix)
      {				// Adjust edges below and above?
	H += (helix + 1);	// Extra base layers
	y0 -= mazestep * helix;
      }
    int X, Y, N;
    unsigned char maze[W][H];
    memset (maze, 0, W * H);
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
	    return 0x80;
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
    void nub (int X, int Y, char *t)
    {
      printf ("nub%d%s(%f,%f);\n", wall, t, (double) X * 360 / W, mazestep * Y + y0 + dy * X);
    }
    printf ("module nub%d(){rotate([0,%f,0])translate([0,%f,0])nub();}\n", wall, a, r + clearance / 2);
    printf ("module nub%dxa(a){render()rotate([0,0,a]){hull(){nub%d();rotate([0,0,%f])translate([0,0,%f])nub%d();}hull(){rotate([0,0,%f])translate([0,0,%f])nub%d();rotate([0,0,%f])translate([0,0,%f])nub%d();}}}\n", wall, wall, (double) 360 / W / 2, dy / 2, wall, (double) 360 / W / 2, dy / 2, wall,
	    (double) 360 / W, dy, wall);
    printf ("module nub%dx(a,h){translate([0,0,h])nub%dxa(a);}\n", wall, wall);
    printf ("module nub%dya(a,h){render()rotate([0,0,a])hull(){nub%d();translate([0,0,%f])nub%d();}}\n", wall, wall, mazestep, wall);
    printf ("module nub%dy(a,h){translate([0,0,h])nub%dya(a);}\n", wall, wall);
    printf ("translate([%f,0,0]){\n", x + r2);
    printf ("difference(){\n");
    if (r2 > r1)
      printf ("union(){translate([0,0,%f])rotate([0,180,0])cylinder(r=%f,h=%f,$fn=%d);cylinder(r=%f,h=%f);}\n", baseheight, r2, baseheight, (sides && wall + 1 >= walls ? sides : curvesteps), r1, height - clearance);
    else
      printf ("cylinder(r=%f,h=%f,$fn=%d);\n", r1, height - clearance, (sides && wall + 1 >= walls ? sides : curvesteps));
    printf ("translate([0,0,%f])cylinder(r=%f,h=%f);\n", wallthickness, r0, height);
    if ((!inside && wall < walls) || (inside && wall > 1))
      {				// Maze cut out
#define	L 1
#define R 2
#define U 4
#define D 8
	// Make maze
	int f = open ("/dev/urandom", O_RDONLY);
	if (f < 0)
	  err (1, "Open /dev/random");
	if (helix)
	  {			// Block any out of scope
	    for (Y = 0; Y < H; Y++)
	      for (X = 0; X < W; X++)
		if (mazestep * Y + y0 + dy * X < base + mazestep / 2 || mazestep * Y + y0 + dy * X > height - mazestep / 2)
		  maze[X][Y] = 0x80;	// To high or low
	  }
	if (testmaze)
	  {			// Simple test pattern
	    for (Y = 0; Y < H; Y++)
	      for (X = 0; X < W; X++)
		if (!(test (X, Y) & 0x80) && !(test (X + 1, Y) & 0x80))
		  maze[X][Y] |= R;
	  }
	else
	  {
	    int x[W * H], y[W * H], p = 0;
	    x[0] = W / nubs / 2;
	    y[0] = H / 2;
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
	// Cut maze
	for (Y = 0; Y < H; Y++)
	  {
	    int render = 0;
	    for (X = 0; X < W; X++)
	      {
		unsigned char v = test (X, Y);
		if (v & 0x80)
		  continue;
		if (!render++)
		  printf ("%srender(){ // Row %d\n", testmaze ? "#" : "", Y + 1);
		if (v & R)
		  nub (X, Y, "x");
		if (v & U)
		  nub (X, Y, "y");
	      }
	    if (render)
	      printf ("}\n");
	  }
	// End points of maze
	for (N = 0; N < nubs; N++)
	  {
	    double dy = 0;
	    if (helix > nubs)
	      dy = mazestep * (N % (helix / nubs)) / (helix / nubs);
	    printf ("rotate([0,0,%f])translate([0,%f,%f])hull(){nub();translate([0,0,%f])nub();}\n", (double) N * 360 / nubs, r + clearance / 2, base + mazestep / 2, y0 + dy + mazestep * helix - (base + mazestep / 2));
	    if (helix)
	      dy += mazestep * helix / nubs / 2;
	    printf ("rotate([0,0,%f])translate([0,%f,%f])hull(){nub();translate([0,0,%f])nub();}\n", (double) (N + 0.5) * 360 / nubs, r + clearance / 2, y0 + mazestep * (H - 1 - (helix ? 2 : 0)) + dy, height - (y0 + mazestep * (H - 1 - (helix ? 2 : 0)) + dy));
	  }
      }
    if (!inside && wall + 1 < walls)
      {				// Connect endpoints over base
	int N;
	for (N = 0; N < nubs; N++)
	  printf ("rotate([0,0,%f])translate([0,%f,%f])hull(){nub();translate([0,0,%f])nub();}\n", -(double) (N + 0.5) * 360 / nubs, r2, -mazestep, baseheight + mazestep);
      }
    printf ("}\n");
    if ((!inside && wall > 1) || (inside && wall < walls))
      {				// Nubs
	double rn = (inside ? r1 : r0);
	int N;
	for (N = 0; N < nubs; N++)
	  printf ("rotate([0,0,%f])translate([0,%f,%f])nub();\n", (double) N * 360 / nubs, rn, height - mazestep / 2);
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
