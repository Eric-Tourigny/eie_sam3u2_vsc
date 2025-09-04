# The Embedded Dinosaur Game

A side-scrolling game on the character LCD of a dev board, based on the [Google Chrome Dinosaur Game](https://en.wikipedia.org/wiki/Dinosaur_Game) as part of the University of Calgary club [embedded in embedded](https://embeddedinembedded.com/).

## Setup

To install the necessary tools to compile the project see [setup.md](docs/setup.md).
To set up the VSCode development environment see [vscode.md](docs/vscode.md).

## Compiling reference

To compile the project, in a new terminal within the project folder:

1. Run `./waf configure --board=ASCII`.
2. Run  `./waf build -F` to build and flash the device.
