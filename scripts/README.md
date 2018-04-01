# Scripts Folder

This is where the different scripts used by the game (e.g. Enemy logic) are stored. 

## Enemy Script Details
There are a few things to know if you are going to write your own scripts for updating enemies.

* Each script has an implicit self object of type `Enemy*` that refers to the `Enemy` being updated
* Not yet, but at some point the `UpdateParams` will be provided to the script as well

## Issues

* Currently, scripts are not allowed to define any variables or functions in them
  * If so, those definitions will stick around inbetween calls to the script, and so you'll get errors about redefining things the second time to script is run. This will be fixed, so it's only a temporary issue
* It's possible not everything you need is available to the script. Check out ChaiManager.cpp to see what all you have access to (open an issue if you see something that should be added)