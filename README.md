# chips-sim

CHIPS detector simulation based on WCSim

# Building

You need an up-to-date version of ROOT6 and Geant4.10 with all its data files

To compile run...

```
$ source setup.sh
$ cmake .
$ make -jN # N is the number of cores your machine has availiable
```

# Running

To run call...

```
$ WCSim -g [geometrySetup.mac file] [generalConfiguration.mac file]
```

by dafault it will use the files found in ./config/example/
