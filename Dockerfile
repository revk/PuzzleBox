FROM debian:unstable-slim

RUN apt-get update \
 && apt-get install -y \
            build-essential \
            gcc \
            libpopt-dev \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp/

COPY . /tmp/PuzzleBox/

RUN cd /tmp/PuzzleBox \
 && make puzzlebox \
 && mv -v puzzlebox entrypoint.sh /usr/local/bin/ \
 && /bin/rm -rv /tmp/PuzzleBox

WORKDIR /opt/

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

# Mandatory Labels
LABEL MAINTAINER="slash5toaster@gmail.com"
LABEL NAME=puzzlebox
LABEL PROJECT=revk
LABEL VERSION=1.0.0

#### End of File, if this is missing the file has been truncated
