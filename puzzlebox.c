// Puzzle box maker

#include <stdio.h>
#include <string.h>
#include <popt.h>
#include <err.h>
#include <stdlib.h>
#include <ctype.h>

int
main (int argc, const char *argv[])
{
  double baseheight = 5;
  double coresize = 5;
  double coreheight = 50;
  double corewall = 2;
  double mazedepth = 2;
  double mazestep = 2;
  double clearance = 0.2;
  int walls = 2;
  int wall = 0;

  {				// POPT
    poptContext optCon;		// context for parsing command-line options
    const struct poptOption optionsTable[] = {
      {"walls", 'm', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &walls, 0, "Walls", "N"},
      {"wall", 'n', POPT_ARG_INT, &wall, 0, "Wall", "N"},
      {"base-height", 'b', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &baseheight, 0, "Base height", "mm"},
      {"core-size", 'c', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coresize, 0, "Core size", "mm"},
      {"core-height", 'h', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &coreheight, 0, "Core height", "mm"},
      {"core-wall", 'w', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &corewall, 0, "Core wall", "mm"},
      {"maze-depth", 'd', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazedepth, 0, "Maze depth", "mm"},
      {"maze-step", 's', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &mazestep, 0, "Maze step", "mm"},
      {"clearance", 'g', POPT_ARG_DOUBLE | POPT_ARGFLAG_SHOW_DEFAULT, &clearance, 0, "Clearance", "mm"},
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

  char *path = getenv ("PATH_INFO");
  if (path)
    {				// Look for settings in path used from apache
      printf ("Content-Type: application/scad\r\n\r\n");	// Used from apache
      while (*path)
	{
	  if (*path == '/')
	    {
	      path++;
	      continue;
	    }
	  if (!isalpha (path))
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
	      coresize = value;
	      break;
	    case 'h':
	      coreheight = value;
	      break;
	    case 'w':
	      corewall = value;
	      break;
	    case 'd':
	      mazedepth = value;
	      break;
	    case 's':
	      mazestep = value;
	      break;
	    case 'g':
	      clearance = value;
	      break;
	    }
	}
    }
  printf ("# Puzzlebox by RevK\n");
  printf ("# Walls=%d\n", walls);
  if (wall)
    printf ("# Wall=%d\n", wall);
  printf ("# Base-Height=%f\n", baseheight);
  printf ("# Core-Size=%f\n", coresize);
  printf ("# Core-Height=%f\n", coreheight);
  printf ("# Core-Wall=%f\n", corewall);
  printf ("# Maze-Depth=%f\n", mazedepth);
  printf ("# Maze-Step=%f\n", mazestep);
  printf ("# Clearance=%f\n", clearance);
  // The nub

  void box (int wall)
  {				// Make the box - wall 1 in inside
    printf ("# Wall %d\n", wall);
    // Make the unit

    // Create the maze

    // Punch the maze

  }
  if (wall)
    box (wall);
  else
    for (wall = 1; wall <= walls; wall++)
      box (wall);
  return 0;
}
