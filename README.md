acStarter
======
##### A simple server manager for Assetto Corsa.

#### Warning
This is in early development, functionality may be missing

## About
acStarter is a small program written in C to manage Assetto Corsa servers.

## Current Features
 * Allows multiple track lists for free join servers
  * Allows free join timed practice servers with multiple track lists
  * Allows different cars per track(any session)
  * Allows different amount of laps per track
  * Allow different max clients per track
  * Allows randomized skin selection
   * These Depending on how many you have, MAX_SKINS may need adjustment in cars.h
 * Restarts the server if it crashes
 * Support for drifting, practice or race sessions
 * Plus more that I'm likely forgetting.

## Installation
 * `git clone git@github.com:nuggs/acStarter.git`
 * Compile the code from the src directory using `make`
 * Configure the files in the cfg directory
 * Copy acStarter to your base Assetto Corsa directory(s) or else it won't read in surfaces.ini
# Notice: At the moment, the program expects configuration to be in /home/$USER/acstarter

## Running
 * Launch using:
 * `./acServer [-h] [-d] [-p] [-r] [-v]`
 * -p will boot using practice configuration/track list
 * -d will boot using drift configuration/track list
 * -r will boot using race configuration/track list

## License
See LICENSE for license information.
