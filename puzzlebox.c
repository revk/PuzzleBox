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

#define 	PI	3.1415926

int
main (int argc, const char *argv[])
{
  double baseheight = 5;
  double corediameter = 10;
  double coreheight = 50;
  double wallthickness = 1.6;
  double mazethickness = 1;
  double mazestep = 3;
  double clearance = 0.2;
  int fn = 100;
  int walls = 4;
  int wall = 0;
  int outside = 0;
  int flat = 0;
  int single = 0;

  {				// POPT
    poptContext optCon;		// context for parsing command-line options
    const struct poptOption optionsTable[] = {
      {"walls", 'm', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &walls, 0, "Walls", "N"},
      {"wall", 'n', POPT_ARG_INT, &wall, 0, "Wall", "N"},
      {"outside", 'o', POPT_ARG_NONE, &outside, 0, "Maze on outside (easy)"},
      {"flat", 'f', POPT_ARG_NONE, &flat, 0, "Flat (non helical)"},
      {"single", 'S', POPT_ARG_NONE, &single, 0, "Single pip"},
      {"base-height", 'b', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &baseheight, 0, "Base height", "mm"},
      {"core-diameter", 'c', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &corediameter, 0, "Core diameter", "mm"},
      {"core-height", 'h', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coreheight, 0, "Core height", "mm"},
      {"wall-thickness", 'w', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &wallthickness, 0, "Wall thickness", "mm"},
      {"maze-thickness", 'd', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazethickness, 0, "Maze thickness", "mm"},
      {"maze-step", 's', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazestep, 0, "Maze step", "mm"},
      {"clearance", 'g', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &clearance, 0, "Clearance", "mm"},
      {"fn", 'a', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &fn, 0, "$fn", "N"},
      POPT_AUTOHELP {}
    };

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

  if (getenv ("HTTP_HOST"))
    printf ("Content-Type: application/scad\r\nContent-Disposition: Attachment; filename=maze.scad\r\n\r\n");	// Used from apache
  char *path = getenv ("PATH_INFO");
  if (path)
    {				// Look for settings in path used from apache
      while (*path)
	{
	  if (*path == '/')
	    {
	      path++;
	      continue;
	    }
	  if (!isalpha (*path))
	    errx (1, "Path should be X=value pairs separated by /");
	  char arg = *path++;
	  double value = 1;
	  if (*path == '=')
	    {
	      path++;
	      value = strtod (path, &path);
	    }
	  switch (arg)
	    {
	    case 'o':
	      outside = 1;
	      break;
	    case 'f':
	      flat = 1;
	      break;
	    case 'S':
	      single = 1;
	      break;
	    case 'm':
	      walls = (int) value;
	      break;
	    case 'n':
	      wall = (int) value;
	      break;
	    case 'b':
	      baseheight = value;
	      break;
	    case 'c':
	      corediameter = value;
	      break;
	    case 'h':
	      coreheight = value;
	      break;
	    case 'w':
	      wallthickness = value;
	      break;
	    case 'd':
	      mazethickness = value;
	      break;
	    case 's':
	      mazestep = value;
	      break;
	    case 'g':
	      clearance = value;
	      break;
	    case 'a':
	      fn = (int) value;
	      break;
	    }
	}
    }
  printf ("// Puzzlebox by RevK, @TheRealRevK www.me.uk\n");
  printf ("// Walls=%d\n", walls);
  if (wall)
    printf ("// Wall=%d\n", wall);
  printf ("// Base-Height=%f\n", baseheight);
  printf ("// Core-Diameter=%f\n", corediameter);
  printf ("// Core-Height=%f\n", coreheight);
  printf ("// Wall-Thickness=%f\n", wallthickness);
  printf ("// Maze-Thickness=%f\n", mazethickness);
  printf ("// Maze-Step=%f\n", mazestep);
  printf ("// Clearance=%f\n", clearance);
  if (outside)
    printf ("// Maze outside\n");
  if (flat)
    printf ("// Non helical maze\n");
  if (single)
    printf ("// Single pip\n");
  if (fn)
    printf ("$fn=%d;\n", fn);
  // The nub
  printf ("module nub(){rotate([%d,0,0])translate([0,0,%f])cylinder(d1=%f,d2=%f,h=%f,$fn=8);}\n", outside ? 90 : -90, -mazethickness, mazestep, mazestep / 3, mazethickness * 2);
  double x = 0;
  void box (int wall)
  {				// Make the box - wall 1 in inside
    // Dimensions
    double r1 = corediameter / 2 + wallthickness + (wall - 1) * (wallthickness + mazethickness + clearance);
    double r0 = r1 - wallthickness;
    double r2 = r1;
    if (wall < walls)
      r2 += wallthickness + clearance;
    if (outside && wall < walls)
      {				// Allow for maze on outside
	r1 += mazethickness;
	r2 += mazethickness;
      }
    if (!outside && wall > 1)
      {				// Allow for maze on inside
	r0 -= mazethickness;
      }
    double height = coreheight + wallthickness * wall;
    if (wall > 1)
      height -= baseheight;
    // Output
    printf ("// Wall %d\n", wall);
    // Maze dimensions
    double r = (outside ? r1 : r0);
    double y = (outside ? baseheight : wallthickness);
    double h = height - y;
    double w = r * 2 * PI;
    int H = (int) (h / mazestep) + 1;
    int W = (int) (w / mazestep) / 2 * 2;
    double a = 0, dy = 0;
    if (!flat)
      {
	a = atan (mazestep / w);
	dy = mazestep / W;
      }
    void nub (int X, int Y, char *t)
    {
      printf ("rotate([0,0,%f])translate([0,0,%f])nub%d%s();\n", (double) X * 360 / W, Y * mazestep + y + dy * X, wall, t);
    }
    printf ("module nub%d(){rotate([0,%f,0])translate([0,%f,0])nub();}\n", wall, a, r + clearance / 2);
    printf ("module nub%dx(){hull(){nub%d();rotate([0,0,%f])translate([0,0,%f])nub%d();}hull(){rotate([0,0,%f])translate([0,0,%f])nub%d();rotate([0,0,%f])translate([0,0,%f])nub%d();}}\n", wall, wall, (double) 360 / W / 2, dy / 2, wall, (double) 360 / W / 2, dy / 2, wall, (double) 360 / W, dy, wall);
    printf ("module nub%dy(){hull(){nub%d();translate([0,0,%f])nub%d();}}\n", wall, wall, mazestep, wall);
    printf ("translate([%f,0,0]){\n", x + r2);
    printf ("difference(){\n");
    if (r2 > r1)
      printf ("union(){cylinder(r=%f,h=%f);cylinder(r=%f,h=%f);}\n", r2, baseheight, r1, height);
    else
      printf ("cylinder(r=%f,h=%f);\n", r1, height);
    printf ("translate([0,0,%f])cylinder(r=%f,h=%f);\n", wallthickness, r0, height);
    if ((outside && wall < walls) || (!outside && wall > 1))
      {				// Maze cut out
#define	L 1
#define R 2
#define U 4
#define D 8
	// Make maze
	int f = open ("/dev/urandom", O_RDONLY);
	if (f < 0)
	  err (1, "Open /dev/random");
	int X, Y;
	unsigned char maze[W][H];
	memset (maze, 0, W * H);
	int x[W * H], y[W * H], p = 0;
	for (X = 0; X < W; X++)
	  {
	    maze[X][0] = 0x80;
	    if (!flat)
	      maze[X][H - 1] = 0x80;
	  }
	x[0] = 0;
	y[0] = 1;
	p++;
	int test (int x, int y)
	{			// Test if in use...
	  while (x < 0)
	    {
	      x += W;
	      if (!flat)
		y--;
	    }
	  while (x >= W)
	    {
	      x -= W;
	      if (!flat)
		y++;
	    }
	  if (y < 0 || y >= H)
	    return 0x80;
	  unsigned char v = maze[x][y];
	  if (single)
	    return v;
	  x += W / 2;
	  while (x >= W)
	    {
	      x -= W;
	      if (!flat)
		y++;
	    }
	  if (y < 0 || y >= H)
	    return 0x80;
	  return v | maze[x][y];
	}
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
		    if (!flat)
		      Y++;
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
		    if (!flat)
		      Y--;
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
	maze[0][0] |= U;
	maze[W / 4][H - 1] |= U;
	if (!flat)
	  maze[W / 4][H - 2] |= U;
	// Cut maze
	for (Y = 0; Y < H; Y++)
	  {
	    printf ("render(){\n");
	    for (X = 0; X < W; X++)
	      {
		unsigned char v = test (X, Y);
		if (v & R)
		  nub (X, Y, "x");
		if (v & U)
		  nub (X, Y, "y");
	      }
	    printf ("}\n");
	  }
      }
    if (outside && wall < walls)
      {
	// TODO outside notches
      }
    printf ("}\n");
    if ((outside && wall > 1) || (!outside && wall < walls))
      {
	double rn = (outside ? r0 : r1);
	printf ("translate([0,%f,%f])nub();\n", rn, height - mazestep / 2);
	if (!single)
	  printf ("rotate([0,0,180])translate([0,%f,%f])nub();\n", rn, height - mazestep / 2 - (flat ? 0 : mazestep / 2));
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
