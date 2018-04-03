# Scripts Folder

This is where the different scripts used by the game (e.g. Enemy logic) are stored. 

## Enemy Script Details
There are a few things to know when writing your own scripts for updating enemies.

* Each script has an implicit `self` object of type `Enemy*` that refers to the `Enemy` being updated
* Not yet, but at some point the `UpdateParams` will be provided to the script as well

## Issues

* Currently, scripts must end with the name of the funtion they are meant to run (c.f. shoe.chai)
* It's possible not everything you need is available to the script. Check out ChaiManager.cpp to see what all you have access to (open an issue if you see something that should be added)