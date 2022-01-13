ARG BUILD_PROFILE=debug
ARG NS3_VERSION=3.32

FROM marshallasch/ns3:${NS3_VERSION}-${BUILD_PROFILE}

LABEL org.opencontainers.version="v1.0.0"

LABEL org.opencontainers.image.authors="Marshall Asch <masch@uoguelph.ca> (https://marshallasch.ca)"
LABEL org.opencontainers.image.url="https://github.com/MarshallAsch/speedfalloff-plots.git"
LABEL org.opencontainers.image.source="https://github.com/MarshallAsch/speedfalloff-plots.git"
LABEL org.opencontainers.image.vendor="Marshall Asch"
LABEL org.opencontainers.image.licenses="Apache-2.0"
LABEL org.opencontainers.image.title="Speed Falloff Plots"
LABEL org.opencontainers.image.description="ns-3 simulation to generate mobility model speed falloff plots for Marshall Asch's Masters research"

ENV BUILD_PROFILE=$BUILD_PROFILE
ENV NS3_VERSION=$NS3_VERSION

VOLUME [ "/results" ]

RUN apt-get update && apt-get install -y python3 python3-pip

COPY requirements.txt scratch/speedfalloff-plots/requirements.txt
RUN pip install -r scratch/speedfalloff-plots/requirements.txt

COPY simulation.py ../simulation.py

COPY . scratch/speedfalloff-plots

RUN ./waf configure --enable-examples --enable-tests --build-profile=${BUILD_PROFILE} && ./waf build


ENTRYPOINT [ "scratch/speedfalloff-plots/entrypoint.sh" ]


# these two labels will change every time the container is built
# put them at the end because of layer caching
ARG VCS_REF
LABEL org.opencontainers.image.revision="${VCS_REF}"

ARG BUILD_DATE
LABEL org.opencontainers.image.created="${BUILD_DATE}"
