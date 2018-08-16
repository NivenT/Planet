# Planet
[![Build Status](https://travis-ci.org/NivenT/Planet.svg?branch=master)](https://travis-ci.org/NivenT/Planet)

WIP Planet hopping game

<p align="center">
 <img src="https://image.ibb.co/foD9GU/Screenshot_from_2018_08_15_21_08_20.png" width=400>
 <img src="https://image.ibb.co/eJfpGU/Screenshot_from_2018_08_15.png" width=400>
 <img src="https://image.ibb.co/f3pc39/Peek_2018_08_15_21_02.gif" width=400>
</p>

Above are (old) screenshots of the game.
- The (top) left one shows the game in debug mode
- The (top) right one shows the view during regular gameplay
- The bottom one shows the World Editor

The planet is made from square tiles, and all the physics is calculated like on a normal flat world. A shader is used to render the rectangular world as a circle.

# The Vision
The main premise of the game is that you start off on some simple planet, and start exploring. There will be enemies to fight, and puzzles to solve; once you complete some objective (maybe reaching the center of the planet, probably something else) you get to fly to other planets and explore more. There's no story with this game, only gameplay; go from planet to planet, solve puzzles, have fun and whatnot. I envision the game having no loading screens or fast travel. Even as you go from planet to planet you'll have to actually fly through the space inbetween.

A little more on the techincal side of the vision, planets should be round with believable (although almost certainly inaccurate) physics. This is currently acheived by having all the physics work on a rectangular planet, but using shader trickery to render things as if they were round. In addition, I am hoping for a good amount of customizability. This includes having enemy AIs and item effects programmed in [ChaiScript](https://github.com/ChaiScript/ChaiScript), and including a world editor where users can build new planets, put together new enemies, etc.

# Current Progress (as of August 15, 2018)
There's still much to be done, but the game is fairly playable. The world editor is basically finished (except for some conveince features like deleting objects), so you are free to create custom worlds, items, enemies, etc. The things you make are stored in the `resources/data` folder, so check it out to see what creations are available to you.

By default, the repo comes with 3 items, 3 enemies (each with an associated spawner), and a couple of premade worlds (`test_planet2` is the better of them). The items are

* A stick which can be used to attack enemies
* A shovel which destorys whatever tile the mouse is hovering over
* A rock that does nothing (but will be throwable in the (near) future)

The enemies are

* A shoe that moves to the right
* A slime that moves to the left
* A ghost that follows the player

You currently cannot change your own sprite.

## Known Bugs

* Rendering issues in round planet mode
  * The edge of the planet is visible as a white line (probably alpha blending issues since the world overlaps a little at the edge)
* The game segfaults when you exit
  * Probably ChaiScript-related
  * Does not affect gameplay
* Physics at the edge of the planet can be off
  * This is a fundamental issue in how the physics is set up (e.g. Box2D expects flat worlds, but the game is made to look like a circular planet), and so cannot be "fixed"
 
## Rough Roadmap
This is a sketch of what I think I should try to do/add next. The items are not listed exactly in the order they will be taken care of because (I suspect) there will be a lot of going back and forth between different things as the game is being fleshed out.

* Add projectiles (e.g. a bullet or a rock)
* Improve world editor
  * Allow objects to be deleted or modified after they've been placed
  * Allow large areas of tiles to be adjusted at once
* Clean up the code (Optional)
  * Things are somehow less messy than I expected
* Add obstacles (e.g. a crate)
* Allow for multiple planets in one game with travel between them
   * It's gonna be fun figuring out how to make the transition from planet to outer space back to planet look smooth
   * Will I ever actually get here or just keep working on the earlier stuff indefinietly?
* TBA

# Beyond the Vision
On the off chance that this thing gets developed enough to realize the (vague) vision I have above, there are a number of extras I have thought about throwing in the keep things interesting. These are low priority for the forseeable future, but in no particular order, some possible features include

* A leveling or skill system
* procedurally generated planets
* meaningful playtime while in outer space
  * maybe some kind of minigame
  * maybe a trading system with NPCs

# How to Build/Run
Before building, make sure you have all the requirements on your system. To play the game, you'll want to clone the repo and compile the code using cmake. On Ubuntu, you'll run something like

```bash
git clone https://github.com/NivenT/Planet
cd Planet
git submodule update --init --recursive
mkdir build
cd build
cmake ../
make
```

Once that's setup, you can run the game from the `build` folder using `./game`

## Alternate How to Build
If you have issues with getting all the dependencies or don't want to install them globally, you also have the option of running the code using [docker](https://www.docker.com/community-edition). This has only been tested on Ubuntu (with spotty success), so mileage may vary. For this, you'll first get the code:

```bash
git clone https://github.com/NivenT/Planet
cd Planet
git submodule update --init --recursive
```

At this point, you'll want to read the comments in the Dockerfile to see if there is anything you need to change. After that, you can build and run the game using the provided bash script.
```bash
./docker-build.sh -r
```

If the `./docker-build.sh` command fails, try adding `sudo`. If it runs but something in the container goes wrong, open an issue.

## Requirements
The [jubilant-funicular](https://github.com/NivenT/jubilant-funicular) submodule (+ its requirements) are all you should need. Technically, you do not need SDL2_mixer since this game has no audio.

This project also makes use of the [Box2D](https://github.com/erincatto/Box2D) library but it is already included here.

# Controls
For an up-to-date account of the controls, check out the update functions in [MainGame.cpp](https://github.com/NivenT/Planet/blob/master/src/MainGame.cpp) and [Player.cpp](https://github.com/NivenT/Planet/blob/master/src/Player.cpp). 

As of July 12, the controls are as below

## Gameplay
### Normal Mode
* WASD - Move and jump
* EQ - Change selected item
* Space - Use selected item
* P - Toggle Paused
* F1 - Toggle Dev Mode

### Dev Mode
* Space - Toggle Debug Mode
* I - Toggle Soft Debug Mode (draws bounding boxes used for physics)
* B - Toggle Drawing of axis aligned bounding boxes (Easiest to understand by pressing it and seeing what happens)

### Debug Mode
* WASD - Moves the center of the camera
* EQ - Rotates camera
* Mouse Wheel (i.e. scrolling) - Zooms in and out
* G - Resets camera
* Enter - Toggle between square and round planet viewing modes

## World Editor
* Space - toggle between "GUI Focus" and "Editor Focus"
 * You can only make changes to the planet when you're in Editor Focus*
* Enter - Toggle between square and round planet modes
* Left Click - Add tile/item/etc. to planet
* Right Click (Tile Tab Only) - Copy the tile you clicked on

# Scripts
As alluded to above, a main focus of this game is customizable items and enemies. A big part of implementing this involves various scripts loaded in during runtime. For more information on this and how to write your own, check out the [scripts folder](https://github.com/NivenT/Planet/tree/master/scripts).
