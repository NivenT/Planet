# Planet
WIP Planet hopping game

# The Vision
The main premise of the game is that you start off on some simple planet, and start exploring. There will be enemies to shoot, and puzzles to solve; once you complete some objective (maybe reaching the center of the planet, maybe something else) you get to fly to other planets and do the same thing. There's no story with this game, only gameplay; go from planet to planet, solve puzzles and having fun and whatnot. I envision the game having no loading screens or fast travel, even as you go from planet to planet. You'll have to actually fly through the space inbetween.

A little more on the techincal side of the vision, planets should be round with believable (although almost certainly inaccurate) physics. This is currently acheived by having all the physics work on a rectangular planet, but using shader trickery to render things as if they were round. In addition, I'd like for the different enemy types to have programmable AIs with their logic written in something like [ChaiScript](https://github.com/ChaiScript/ChaiScript). Depending on how well I can implement things, this philosophy might extend to customizable items and who knows what else.

# Current Progress (as of December 22, 2017)
Todo

# Beyond the Vision
On the off chance that this thing gets developed enough to realize the (vague) vision I have above, there are a number of extras I have thought about throwing in the keep things interesting. These are low priority until something more playable than the current state is acheived, but in no particular, some possible features include

* A leveling or skill system
* procedurally generated planets
* meaningful playtime while in outer space
  * maybe some kind of minigame
  * maybe a trading system with NPCs

# How to Build/Run
To play the game, you'll want to clone the repo and compile the code using cmake. On Ubuntu, you'll run something like

```bash
git clone https://github.com/NivenT/Planet
cd Planet
git submodule update --init --recursive
mkdir build
cd build
cmake ../
make
ln -s ../resources
ln -s ../shaders
```

Once that's setup, you can run the game from the `build` folder using `./game`

# Controls
TODO

# Requirements
Besides the [jubilant-funicular](https://github.com/NivenT/jubilant-funicular) submodule (+ its requirements), the other requirements for building this game are

* [Box2D](https://github.com/erincatto/Box2D)
  * On Ubuntu, you can just run `sudo apt-get install libbox2d-dev`
