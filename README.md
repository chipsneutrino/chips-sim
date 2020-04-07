# chips-sim

CHIPS detector simulation based on WCSim

## Building

You need an up-to-date version of ROOT6 and Geant4.10 with all the usual data files. This is provided by the chips-env repository.

To compile all the apps run...

```
$ source setup.sh
$ cmake .
$ make -jN # Where N is the number of cores your machine has availiable
```

## Running WCSim

```
$ ./WCSim -g [geometrySetup.mac file] [generalConfiguration.mac file]
```

by default it will use the files found in ./config/example/

## Running the Geometry Helper

```
$ ./geometryHelper
```

then just follow the prompt to generate geometry .xml

## Running the Event Display

```
$ ./evDisplay
```
