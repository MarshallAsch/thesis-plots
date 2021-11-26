[![Clang-format](https://github.com/MarshallAsch/speedfalloff-plots/actions/workflows/clang-format.yml/badge.svg)](https://github.com/MarshallAsch/speedfalloff-plots/actions/workflows/clang-format.yml)
[![build-ns3](https://github.com/MarshallAsch/speedfalloff-plots/actions/workflows/ns3-build.yml/badge.svg)](https://github.com/MarshallAsch/speedfalloff-plots/actions/workflows/ns3-build.yml)
[![Create Docker Container](https://github.com/MarshallAsch/speedfalloff-plots/actions/workflows/deployment.yml/badge.svg)](https://github.com/MarshallAsch/speedfalloff-plots/actions/workflows/deployment.yml)
![GitHub](https://img.shields.io/github/license/marshallasch/speedfalloff-plots?style=plastic)
![Lines of code](https://img.shields.io/tokei/lines/github/marshallasch/speedfalloff-plots?style=plastic)
![NS3 version](https://img.shields.io/badge/NS--3-3.32-blueviolet?style=plastic)

# Random Waypoint Mobility Model Test

This repository holds the code that is used to run the ns-3 simulations to generate the data and produce the RWPM plots used for my thesis. 

This code was written for use with the ns-3.32 simulator and is designed to be run from within the scratch folder. 


This is a very simple simulation of 500 nodes in a 1km x 1km area that move around according to the RWPM and it collects the nodes speed and position every second,



## Prereqs:

`pip install matplotlib sem seaborn requests`


If building netanim:
- qt4-qmake
- libqt4-dev

(additionally, on ubuntu 20.10, you need to add an additional repo because qt4 no longer ships with it)
- https://launchpad.net/~gezakovacs/+archive/ubuntu/ppa

## Building this project

Reproducibility is one of the key goals of simulator studies.

 1. Download and build copy of the ns-3.32 all-in-one distribution.

    ```sh
    wget https://www.nsnam.org/release/ns-allinone-3.32.tar.bz2
    tar xjvf ns-allinone-3.32.tar.bz2
    cd ns-allinone-3.32
    python3 ./build.py --enable-examples --enable-tests
    ```

 2. Change directories to the `scratch/` folder of the ns-3.32 source
    distribution.

    ```sh
    cd ns-3.32/scratch/
    ```

 3. Clone this repository.

    ```sh
    git clone git@github.com:marshallasch/speedfalloff-plots.git
    ```

4. Change directory back to the `ns-3.32` folder of the source distribution
   and re-configure `./waf` so it can cache the new changes

   ```sh
   cd ..
   ./waf configure --enable-examples --enable-tests
   ./waf build
   ```

5. Run the example simulation that is included within the module.

   ```sh
   ./waf --run 'scratch/speedfalloff-plots/speedfalloff-plots'
   ```
   
   This will produce an output file in the form of:
   ```
   # time(seconds) nodeID xLocation yLocation xVelocity yVelocity
   0 0 831.69 955.872 0 0
   ...
   ```

   There are a number of flags that can be given to change various parameters.
   See `./waf --run 'scratch/speedfalloff-plots/speedfalloff-plots --help'` for the available options


## Building the container

```bash
$ docker build --build-arg GIT_COMMIT=$(git rev-parse -q --verify HEAD) --build-arg BUILD_DATE=$(date -u +"%Y-%m-%dT%H:%M:%SZ") -t marshallasch/speedfalloff:latest .
```

The build variant can be configured using the `BUILD_PROFILE` `build-arg`, it can be set to either `debug`, `release`, or `optimized`.
It is set to `debug` by default.


## Run using Docker

In order to keep a version of the figure generator we have developed a prebuilt docker image that can be used to run the simulation without needing to install ns3 or any of its dependencies.

The prebuilt docker container can be used in one of three ways:

- interactively using a `bash` shell
- directly to run the simulation script with the desired flags
- to run the SEM script directly to collect the results and generate the simulation plots


### Interactive bash terminal

The interactive shell will put you in the ns3 root directory.
Depending on which docker image tag is used ns3 can either be built in debug mode or optimized mode.
The simulation script has been pre installed in the `scratch` folder. 
SEM has been installed and the sem runner script can be accessed in the parent directory.

```bash
docker run --rm -it marshallasch/speedfalloff:optimized bash
```


### Directly run the simulation

Any flags that are given to the container in the docker run command are passed directly to the waf script. 
It is equivalent to running `./waf --run "scratch/speedfalloff-plots/speedfalloff-plots <my flags>"`.

```bash
docker run --rm  marshallasch/speedfalloff:optimized --runTime=5
```

### Collect the simulation results and generate the figues

By giving the `sem` command to the docker container the sem script will be run to collect the simulation results and generate the figures. 
The results will be put into the `/results` directory, which can be mounted so the figures can be used. 

```bash
docker run --rm  -v "$(pwd)/results:/results" marshallasch/speedfalloff:optimized sem
```


## Generating the figures

Alternatively you can run the [SEM](https://github.com/signetlabdei/sem) script manually by placing it in the parent directory of the ns3-3.2 folder. 
Some modifications will need to me made to the `simulation.py` script as it is developed to be used in the docker container.
Specifically the location that it writes the files to should be modified. 

To use it ns3 needs to be installed on the system and the script needs to be placed **outside** of the ns3 directory, as shown below.

Note, this currently requires features from SEM that are only in the `develop` branch.


```
ns-3.32/
  scratch/
    speedfalloff-plots/
      ....
simulation.py
```

This directory structure can be changed somewhat by editing the script, but it must be outside the directory.
Then running `./simulation.py` from the parent directory will compile and run the simulations as well as generate the figures.
