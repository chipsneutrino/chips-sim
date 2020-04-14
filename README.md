# chips-sim

The CHIPS detector simulation. A heavily modified version of [WCSim](https://github.com/WCSim/WCSim)

## Building

You need an up-to-date version of ROOT6 and Geant4.10 with all the usual data files. This is provided by the chips-env repository.
The first time you run sim-setup.sh it will build chips-sim, any further changes will require a 'make' command

```
$ source sim-setup.sh
```

## Running WCSim

```
$ source sim-setup.sh
$ WCSim -g [geometrySetup.mac file] [generalConfiguration.mac file]
```

by default it will use the files found in ./config/example/

## Running the Geometry Helper

```
$ source sim-setup.sh
$ geometryHelper
```

then just follow the prompt to generate geometry .xml

## Running the Event Display

```
$ source sim-setup.sh
$ evDisplay
```

## Cleaning Everything Up

To remove all artifacts and return to the base state run...

```
$ source sim-tidy.sh
```

## Creating emission profiles

You can create photon emission files that describe how cherenkov photons are emitted from a primary particle using the example macro file at ./config/example/emission_profiles.mac with the geofile ./config/example/emission_geo.mac
