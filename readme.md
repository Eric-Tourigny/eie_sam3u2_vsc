# The Embedded Dinosaur Game

A side-scrolling game on the character LCD of a dev board, based on the [Google Chrome Dinosaur Game](https://en.wikipedia.org/wiki/Dinosaur_Game) as part of the University of Calgary club [embedded in embedded](https://embeddedinembedded.com/).

## Features

The game starts in a menu, allowing the user to choose between using the button of the board or using the space key on a laptop (via [ANT radio](https://en.wikipedia.org/wiki/ANT_(network))). Then, the game begins, with this button allowing the T-Rex to jump over randomly spaced cactuses. The game continues until the T-Rex hits a cactus, at which point red lights are flashed and the user may restart. The game is displayed on a ASCII character board, using custom characters to display and move the dino and cactuses.

## Setup

This software is intended for the [EiE ASCII Development Board](https://embeddedinembedded.com/hardware/)
To install the necessary tools to compile the project see [setup.md](docs/setup.md).
To set up the VSCode development environment see [vscode.md](docs/vscode.md).

## Compiling reference

To compile the project, in a new terminal within the project folder:

1. Run `./waf configure --board=ASCII`.
2. Run  `./waf build -F` to build and flash the device.
