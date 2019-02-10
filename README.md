# bingo
Client-server and peer-to-peer automatic bingo game written in C++

## Building the Project
### Full Build
To build everything, simply run the following in the project root:
```
$ make
```

To clean up the build files, libraries, and binaries:
```
$ make clean
```

### Partial Build
For testing, you may want to build a single application within the project, i.e. `manager` or `bingo`. To do this, you will need to build the global static library first. Within the project root:
```
$ cd lib && make
```

Then, you can `cd` into the appropriate app (i.e. `bingo` or `manager` and build it.

To clean the partial build, just run `make clean` from the application directory. Optionally, if you want to clean the static library, you may do the same in the `lib` directory.

Of course, to clean everything, simply run `make clean` from the project root.
