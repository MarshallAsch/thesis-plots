
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
ENV BUILD_PROFILE=$BUILD_PROFILE

VOLUME [ "/results" ]

RUN apt-get update && apt-get install -y python3-pip

RUN cd /opt && \
    git clone https://github.com/signetlabdei/sem && \
    cd sem && \
    git checkout develop && \
    pip install . && \
    cd .. && \
    rm -rf sem

ADD . scratch/speedfalloff-plots
RUN pip install -r scratch/speedfalloff-plots/requirements.txt

RUN cp scratch/speedfalloff-plots/simulation.py ../simulation.py
RUN ./waf configure --enable-examples --enable-tests --build-profile=${BUILD_PROFILE} && ./waf build


ENTRYPOINT [ "scratch/speedfalloff-plots/entrypoint.sh" ]


# these two labels will change every time the container is built
# put them at the end because of layer caching
ARG GIT_COMMIT
LABEL org.opencontainers.image.revision="${GIT_COMMIT}"

ARG BUILD_DATE
LABEL org.opencontainers.image.created="${BUILD_DATE}"
