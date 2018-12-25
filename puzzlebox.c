// Puzzle box maker

#include <stdio.h>
#include <string.h>
#include <popt.h>
#include <err.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define 	PI	3.1415926

int
main (int argc, const char *argv[])
{
  double baseheight = 5;
  double corediameter = 5;
  double coreheight = 50;
  double wallthickness = 2;
  double mazethickness = 2;
  double mazestep = 2;
  double clearance = 0.2;
  double fn = 0;
  int walls = 2;
  int wall = 0;
  int outside = 0;
  int flat = 0;

  {				// POPT
    poptContext optCon;		// context for parsing command-line options
    const struct poptOption optionsTable[] = {
      {"walls", 'm', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &walls, 0, "Walls", "N"},
      {"wall", 'n', POPT_ARG_INT, &wall, 0, "Wall", "N"},
      {"outside", 'o', POPT_ARG_NONE, &outside, 0, "Maze on outside (easy)"},
      {"flat", 'f', POPT_ARG_NONE, &flat, 0, "Flat (non helical)"},
      {"base-height", 'b', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &baseheight, 0, "Base height", "mm"},
      {"core-diameter", 'c', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &corediameter, 0, "Core diameter", "mm"},
      {"core-height", 'h', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coreheight, 0, "Core height", "mm"},
      {"wall-thickness", 'w', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &wallthickness, 0, "Wall thickness", "mm"},
      {"maze-thickness", 'd', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazethickness, 0, "Maze thickness", "mm"},
      {"maze-step", 's', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazestep, 0, "Maze step", "mm"},
      {"clearance", 'g', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &clearance, 0, "Clearance", "mm"},
      {"fn", 'a', POPT_ARG_DOUBLE, &fn, 0, "$fn", "N"},
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
	      fn = value;
	      break;
	    }
	}
    }
  printf ("// Puzzlebox by RevK\n");
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
  if (fn)
    printf ("$fn=%f;\n", fn);
  // The nub
  printf ("module nub(){cube([%f,%f,%f]);}\n", mazestep / 2, mazestep / 2, mazethickness);
  double x = 0;
  void box (int wall)
  {				// Make the box - wall 1 in inside
    printf ("// Wall %d\n", wall);
    // Dimensions
    double r = corediameter / 2 + wallthickness + (wall - 1) * (wallthickness + mazethickness);	// radius inside before maze added (or for outer)
    double height = coreheight + wallthickness;

    printf ("translate([%f,0,0]){\n", x + r + (wall < walls ? mazethickness : 0));
    // Make the unit
    if (wall < walls)
      printf ("cylinder(r=%f,h=%f);\n", r + mazethickness, baseheight);
    printf ("cylinder(r=%f,h=%g);\n", r, height);

    // Create the maze

    // Punch the maze
    printf ("}\n");
    x += (r + (wall < walls ? mazethickness : 0)) * 2 + 10;
  }
  if (wall)
    box (wall);
  else
    for (wall = 1; wall <= walls; wall++)
      box (wall);
  return 0;
}
