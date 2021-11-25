
FROM marshallasch/ns3:3.32


LABEL org.opencontainers.version="v1.0.0"

LABEL org.opencontainers.image.authors="Marshall Asch <masch@uoguelph.ca> (https://marshallasch.ca)"
LABEL org.opencontainers.image.url="https://github.com/MarshallAsch/speedfalloff-plots.git"
LABEL org.opencontainers.image.source="https://github.com/MarshallAsch/speedfalloff-plots.git"
LABEL org.opencontainers.image.vendor="University of Guelph School of Computer Science"
LABEL org.opencontainers.image.licenses="ISC"
LABEL org.opencontainers.image.title="Speed Falloff Plots"
LABEL org.opencontainers.image.description="ns-3 simulation to generate mobility model speed falloff plots for Marshall Asch's Masters research"

ARG BUILD_PROFILE=debug

ADD . scratch/plots

RUN ./waf configure --enable-examples --enable-tests --build-profile=${BUILD_PROFILE} && ./waf build


ENTRYPOINT [ "/bin/bash" ]


# these two labels will change every time the container is built
# put them at the end because of layer caching
ARG GIT_COMMIT
LABEL org.opencontainers.image.revision="${GIT_COMMIT}"

ARG BUILD_DATE
LABEL org.opencontainers.image.created="${BUILD_DATE}"
