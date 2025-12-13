# build the puzzlebox binary
FROM debian:unstable-slim AS puzzlebuilder

# Otherwise you will get an interactive setup session
ENV DEBIAN_FRONTEND=noninteractive
RUN rm -f /etc/apt/apt.conf.d/docker-clean; echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' > /etc/apt/apt.conf.d/keep-cache

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update \
 && apt-get install -y \
            build-essential \
            gcc \
            libpopt-dev \
 && apt-get clean \
 && pwck -s \
 && grpck -s \
 && rm -rf \
    /tmp/* \
    /var/tmp/*

WORKDIR /tmp/PuzzleBox

COPY . /tmp/PuzzleBox/

RUN cd /tmp/PuzzleBox \
 && make puzzlebox \
 && mv -v puzzlebox entrypoint.sh /usr/local/bin/

WORKDIR /opt/

FROM debian:unstable-slim

# Otherwise you will get an interactive setup session
ENV DEBIAN_FRONTEND=noninteractive
RUN rm -f /etc/apt/apt.conf.d/docker-clean; echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' > /etc/apt/apt.conf.d/keep-cache

RUN useradd puzzle -s /usr/sbin/nologin

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update \
 && apt-get install -y \
            libpopt-dev \
            openscad \
 && apt-get clean \
 && pwck -s

COPY --from=puzzlebuilder /usr/local/bin/ /usr/local/bin/
COPY makesamples.sh /usr/local/bin/

USER puzzle

VOLUME [ "/home/puzzle/samples" ]

WORKDIR /home/puzzle

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

# Mandatory Labels
LABEL org.opencontainers.image.authors="slash5toaster@gmail.com" \
      org.opencontainers.image.vendor=slash5toaster \
      org.opencontainers.image.ref.name=puzzlebox \
      org.opencontainers.image.version=1.1.0

#### End of File, if this is missing the file has been truncated
