# Planet
WIP Planet hopping game

<p align="center">
 <img src="https://github.com/NivenT/Planet/blob/master/screenshots/screen1.png" width=400>
 <img src="https://github.com/NivenT/Planet/blob/master/screenshots/screen2.png" width=400>
</p>

Above are screenshots of the game.
- The left one shows debug mode and the various items/enemies that currently spawn for testing purposes
- The right one shows the view during regular gameplay (the shoe is what's being used as a test enemy)

The planet is made from square tiles, and all the physics is calculated like on a normal flat world. A shader is used to render the rectangular world as a circle.

# The Vision
The main premise of the game is that you start off on some simple planet, and start exploring. There will be enemies to shoot, and puzzles to solve; once you complete some objective (maybe reaching the center of the planet, maybe something else) you get to fly to other planets and do the same thing. There's no story with this game, only gameplay; go from planet to planet, solve puzzles, have fun and whatnot. I envision the game having no loading screens or fast travel. Even as you go from planet to planet you'll have to actually fly through the space inbetween.

A little more on the techincal side of the vision, planets should be round with believable (although almost certainly inaccurate) physics. This is currently acheived by having all the physics work on a rectangular planet, but using shader trickery to render things as if they were round. In addition, I'd like for the different enemy types to have programmable AIs with their logic written in something like [ChaiScript](https://github.com/ChaiScript/ChaiScript). Depending on how well I can implement things, this philosophy might extend to customizable items and who knows what else.

# Current Progress (as of March 30, 2018)
There is currently only a single planet which is in development as a testbed for the features being added into the game. You control a little light (will change once I figure out how I'll get decent sprites) that can move around and jump. With you on the planet are a stick and a rock which you can pick up to add to your inventory, and a shoe that serves as an enemy.

The next big milestone is giving the enemy some form a behavior controlled by a ChaiScript script.

## Known Bugs

* Rendering issues in round planet mode
  * The edge of the planet is visible as a white line (probably alpha blending issues since the world overlaps a little at the edge)
  * the planet is distorted as you move up and down in debug mode
    * not sure if this is actually a bug or just an artifact of how things are rendered
* The physics could use some fine tuning
* The game segfaults when you exit
  * Probably ChaiScript-related
 
## (Very) Rough Roadmap
This is a sketch of what I think I should try to do/add next. The items are not listed exactly in the order they will be taken care of because (I suspect) there will be a lot of going back and forth between different things as the game is being fleshed out.

* Add Enemies
  * Make their behavior Programmable with Chaiscript
* Make Items usable
  * Undecided on how this will be implemented. At the very least, I don't want each item to need to be it's own class
    * Possibly also use Chaiscript for this but I imagine item effects being less constrained than enemy movement, so I don't know how well that would work
    * Items could also store some `use_func` variable that gets passed in with creation.
* Build a level editor
   * Ideally, would allow for custom items/enemies defined using text files and whatnot
   * This is the stage at which I imagine adding a lot of variety to items, enemies, etc.
* Allow for multiple planets in one game with travel between them
   * It's gonna be fun figuring out how to make the transition from planet to outer space back to planet look smooth
* TBA

# Beyond the Vision
On the off chance that this thing gets developed enough to realize the (vague) vision I have above, there are a number of extras I have thought about throwing in the keep things interesting. These are low priority until something more playable than the current state is acheived, but in no particular, some possible features include

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

Note: If possible, compile with gcc instead of clang.

## Requirements
The [jubilant-funicular](https://github.com/NivenT/jubilant-funicular) submodule (+ its requirements) are all you should need.

# Controls
For an up-to-date account of the controls, check out the update functions in [MainGame.cpp](https://github.com/NivenT/Planet/blob/master/src/MainGame.cpp) and [Player.cpp](https://github.com/NivenT/Planet/blob/master/src/Player.cpp). 

As of Feb 27, the controls are as below

## Normal Mode
* WASD - Move and jump
* EQ - Change selected item
* P - Toggle Paused
* F1 - Toggle Dev Mode

## Dev Mode
* Space - Toggle Debug Mode
* I - Toggle Soft Debug Mode (draws bounding boxes used for physics)
* B - Toggle Drawing of axis aligned bounding boxes (Easiest to understand by pressing it and seeing what happens)

## Debug Mode
* WASD - Moves the center of the camera
* EQ - Rotates camera
* Mouse Wheel (i.e. scrolling) - Zooms in and out
* G - Resets camera
* Enter - Toggle between square and round planet viewing modes
