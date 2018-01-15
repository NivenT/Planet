# Planet
WIP Planet hopping game

![image](https://github.com/NivenT/Planet/blob/master/screenshots/screen1.png)

Above is a screenshot of the game in debug mode, mainly showing that the planet is made of tiles. All the physics is calculated as if the planet is a rectangle, but things are rendered as if the planet is a circle. The little box near the top is the player, and the red dot is just there to keep track of the center of the screen.

# The Vision
The main premise of the game is that you start off on some simple planet, and start exploring. There will be enemies to shoot, and puzzles to solve; once you complete some objective (maybe reaching the center of the planet, maybe something else) you get to fly to other planets and do the same thing. There's no story with this game, only gameplay; go from planet to planet, solve puzzles, have fun and whatnot. I envision the game having no loading screens or fast travel. Even as you go from planet to planet you'll have to actually fly through the space inbetween.

A little more on the techincal side of the vision, planets should be round with believable (although almost certainly inaccurate) physics. This is currently acheived by having all the physics work on a rectangular planet, but using shader trickery to render things as if they were round. In addition, I'd like for the different enemy types to have programmable AIs with their logic written in something like [ChaiScript](https://github.com/ChaiScript/ChaiScript). Depending on how well I can implement things, this philosophy might extend to customizable items and who knows what else.

# Current Progress (as of January 14, 2018)
A good amount of the initial setting up of things is done, so the game is technically in a playable state. However, so far that just means that you can move the player around. You control a little light (will change once I figure out where I'm gonna get decent sprites) that can move around and jump. That's it so far.

## Known Bugs
* The camera is mispositioned in round planet mode
 * this has to do with the planet shader "rounding out" the (rectangular) planet before applying the camera matrix, so the intended center has already been messed with before the camera is applied
 * ideally, the camera stays centered at the player and is tilted so that one of its axes is tangent to the planet
 
## Next Steps (besides bug fixes)
These are not necessarily in the order they will be implemented in, and the next things to be implemented will not necessarily come from this list, but hopefully this gives me enough of an idea of what I can work on to keep me from abandoning this project.
* Add Items
 * Weapons like guns/swords
 * Tools like a shovel
* Add obstacles/scenary
 * Trees and grass (likely background and not interactive)
 * Crates to jump over
* Add Enemies

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
ln -s ../resources
ln -s ../shaders
```

Once that's setup, you can run the game from the `build` folder using `./game`

# Controls
I am currently to lazy to type up what all the controls are, but the update function in [MainGame.cpp](https://github.com/NivenT/Planet/blob/master/src/MainGame.cpp) should be readable enough, and so check it out. You'll also want to look at the update function in [Player.cpp](https://github.com/NivenT/Planet/blob/master/src/Player.cpp). 

# Requirements
Besides the [jubilant-funicular](https://github.com/NivenT/jubilant-funicular) submodule (+ its requirements), the other requirements for building this game are

* [Box2D](https://github.com/erincatto/Box2D)
  * On Ubuntu, you can just run `sudo apt-get install libbox2d-dev`
