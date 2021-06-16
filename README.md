![Code linter on main](https://img.shields.io/github/workflow/status/marshallasch/thesis-plots/Clang-format/main?style=plastic)
![ns3 build checker on main](https://img.shields.io/github/workflow/status/marshallasch/thesis-plots/build-ns3/main?style=plastic)
![GitHub](https://img.shields.io/github/license/marshallasch/thesis-plots?style=plastic)
![Lines of code](https://img.shields.io/tokei/lines/github/marshallasch/thesis-plots?style=plastic)
![NS3 version](https://img.shields.io/badge/NS--3-3.32-blueviolet?style=plastic)

# Random Waypoint Mobility Model Test

This repository holds the code that is used to run the ns-3 simulations to generate the data and produce the RWPM plots used for my thesis. 

This code was written for use with the ns-3.32 simulator and is designed to be run from within the scratch folder. 


This is a very simple simulation of 500 nodes in a 1km x 1km area that move around according to the RWPM and it collects the nodes speed and position every second,



## Prereqs:

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
    git clone git@github.com:marshallasch/thesis-plots.git plots
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
   ./waf --run 'scratch/plots/gen_trace' > dump.txt
   ```
   
   This will produce an output file in the form of:
   ```
   < time="0" node="0" locX="831.69" locY="955.872" velX="0" velY="0" >
    ...
   ```

   Note that there is also some other output that is generate by the `./waf` tool that will need to be filtered out. 
   Anythinig that does not match the above sample line can be filtered out using grep 

   ```bash
   $ grep -E "^< time=.* >$" dump.txt > filtered.txt
   ```

6. The `genSpeed.py` script needs the data to be in a slightly different format for it to use it. 

    ```bash 
    $ sed -E 's/^< time="([0-9]+)" node="([0-9]+)" locX="(-?[0-9]+\.?[0-9]*)" locY="(-?[0-9]+\.?[0-9]*)" velX="(-?[0-9]+\.?[0-9]*(e-[0-9]+)?)" velY="(-?[0-9]+\.?[0-9]*(e-[0-9]+)?)" >$/\1 \2 \3 \4 \5 \7/' filtered.txt  > values.txt
    ```
7. run `./genSpeed.py` to generate the the node speed decay plot and the node position plot
