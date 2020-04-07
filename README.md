# chips-sim

CHIPS detector simulation based on WCSim

## Building

You need an up-to-date version of ROOT6 and Geant4.10 with all the usual data files. This is provided by the chips-env repository.
The first time you run setup.sh it will buld chips-sim, any further changes will require a 'make' command

```
$ source setup.sh
```

## Running WCSim

```
$ source setup.sh
$ WCSim -g [geometrySetup.mac file] [generalConfiguration.mac file]
```

by default it will use the files found in ./config/example/

## Running the Geometry Helper

```
$ source setup.sh
$ geometryHelper
```

then just follow the prompt to generate geometry .xml

## Running the Event Display

```
$ source setup.sh
$ evDisplay
```

## Cleaning Everything Up

To remove all artifacts and return to the base state run...

```
$ source tidy.sh
```
