![Code linter on main](https://img.shields.io/github/workflow/status/marshallasch/speedfalloff-plots/Clang-format/main?style=plastic)
![ns3 build checker on main](https://img.shields.io/github/workflow/status/marshallasch/speedfalloff-plots/build-ns3/main?style=plastic)
![GitHub](https://img.shields.io/github/license/marshallasch/speedfalloff-plots?style=plastic)
![Lines of code](https://img.shields.io/tokei/lines/github/marshallasch/speedfalloff-plots?style=plastic)
![NS3 version](https://img.shields.io/badge/NS--3-3.32-blueviolet?style=plastic)

# Random Waypoint Mobility Model Test

This repository holds the code that is used to run the ns-3 simulations to generate the data and produce the RWPM plots used for my thesis. 

This code was written for use with the ns-3.32 simulator and is designed to be run from within the scratch folder. 


This is a very simple simulation of 500 nodes in a 1km x 1km area that move around according to the RWPM and it collects the nodes speed and position every second,



## Prereqs:

`pip install matplotlib sem seaborn numpy`


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
   ./waf --run 'scratch/speedfalloff-plots'
   ```
   
   This will produce an output file in the form of:
   ```
   # time(seconds) nodeID xLocation yLocation xVelocity yVelocity
   0 0 831.69 955.872 0 0
   ...
   ```

   There are a number of flags that can be given to change various parameters.
   See `./waf --run 'scratch/speedfalloff-plots --help'` for the available options


## Building the container

```bash
$ docker build --build-arg GIT_COMMIT=$(git rev-parse -q --verify HEAD) --build-arg BUILD_DATE=$(date -u +"%Y-%m-%dT%H:%M:%SZ") -t marshallasch/speedfalloff:latest .
```

The build varient can be configured using the `BUILD_PROFILE` `build-arg`, it can be set to either `debug`, `release`, or `optimized`.
It is set to `debug` by default.


## Run using Docker

In order to keep a version of the figure generator we have developed a prebuilt docker image that can be used to run the simulation without needing to install ns3 or any of its dependencies.


```bash
docker run --rm -it marshallasch/speedfalloff:optimized
> ./waf --run "speedfalloff-plots"
```


## Generating the figures

Currently the figures can be generated using the packaged [SEM](https://github.com/signetlabdei/sem) project script `simulation.py`.
To use it ns3 needs to be installed on the system and the script needs to be placed **outside** of the ns3 directory, as shown below.

```
/allinone2/
         ns-3.32/
                scratch/
                       speedfalloff-plots/
                                        ....
/experiment/
         simulation.py
```

This direcory scructture can be changed somewhat by editing the script, but it must be outside the directory.
Then running `./simulation.py` from within the `experiment` directory will compile and run the simulations as well as generate the figures.


