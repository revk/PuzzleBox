# Puzzle Box
Makes OpenSCAD file for a puzzle box

Has been tested on linux, you probably need libpopt-dev

As you will see from the Makefile, you will need popt development library.
You also need gcc supporting _GNU_SOURCE for strdupa() and asprintf()

And, for the avoidance of doubt, obviously, this is normally built using "make"

When built, use --help for options, or see https://www.me.uk/puzzlebox

On macOS, this requires gcc, so "brew install gcc", then should make with no problem.

(c) Copyright 2019 Adrian Kennard. See LICENSE file (GPL)

------------------

N.B.

I (Clyde) have added support for building an OCI image (aka docker) to the make file.  You have the option to build locally, or push to a registry.
N.B. If you want to push to a registry, you'll need to change the vendor and author labels in the dockerfile, 
```
      org.opencontainers.image.vendor=slash5toaster \
      org.opencontainers.image.authors="slash5toaster@gmail.com" \

```

```
make 
=======
make
help                      This help.
puzzlebox                 Build the puzzlebox binary
envs                      Show the environments
sif                       Pull the docker container as a sif
docker                    Build the docker image locally
docker-multi              Multi-platform build
run                       Launch shell into the container, with this directory mounted to /opt/source
pull                      Pull Docker image
publish                   Push ldocal image to remote server
docker-lint               Check Dockerfile for errors
```

For Example, to run locally
```
docker run --rm -it \
        -v $(pwd)/source/samples:/opt/samples/ \
        slash5toaster/puzzlebox:1.2.0 \
        puzzlebox --stl --core-diameter=40 --core-height=15 --text-end="Surprise" \
                  --out-file=/opt/samples/surpise.stl
```
