# Main

Main is a 2D platformer game written in C using the SDL2 library. In this game, you control a bunch of lightning strikes that can jump around and teleport. The objective is to destroy the world by using the WASD and SPACE commands to conquer and make the lightning cloud bigger. 

You control main, a program that was accidentally created by a shoddy developer, resulting in a calamity that threatens to destroy the world with its vectorized horror.

## Features

The current version of Main is a prototype with partial implementation of movement, camera, and hitbox functionalities. 

## System Requirements

To run Main, you need a Unix-based system and the SDL2 library installed on your computer.

## Compilation

Main can be compiled using the provided Makefile. Simply run the `make` command, which includes the following rules:

- `make`: Compiles the program.
- `make clean`: Removes object files.
- `make fclean`: Removes object files and the compiled executable.
- `make all`: Equivalent to `make`.

## Dependencies

Main relies on the following external library:

- SDL2: A cross-platform development library designed to provide low-level access to audio, keyboard, mouse, joystick, and graphics hardware.

Please make sure you have the SDL2 library installed before attempting to compile and run Main.

## Usage

Once you have successfully compiled Main, you can run the executable to start the game. The controls are as follows:

- Use the WASD keys to control the lightning strikes.
- Press the SPACE key to teleport the lightning strikes.
- The objective is to destroy the world by strategically conquering and expanding the lightning cloud.

---
## Example
[Screencast from 07-04-2023 09:56:31 AM.webm](https://github.com/DaadfroGG/main.c/assets/101118957/321fb737-e6f0-44e3-835e-84e39e92e410)
