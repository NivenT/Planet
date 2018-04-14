# Scripts Folder

This is where the different scripts used by the game (e.g. Enemy logic) are stored.

The ChaiScript language is essentially C++ if it were a scripting language. Beyond what's in this foler, you can [look here](http://chaiscript.com/examples.html#ChaiScript_Basic_Example) for more coding examples. Do not be afraid to open an issue if you have questions about writing scripts.

# Enemy Script Details
There are a few things to know when writing your own scripts for updating enemies.

* Each script has an implicit `self` object of type `Enemy` that refers to the `Enemy` being updated
* The `UpdateParams` are provided to the script as well under the name `params`

# Item Script Details
These are much the same as for Enemy scripts

* Each script has an implicit `self` object of type `Item` that refers to the `Item` being updated
* Each script has an implicit `params` object of type `UpdateParams` coming from the Player invoking the script

# Issues

* Currently, scripts must end with the name of the funtion they are meant to run (c.f. shoe.chai)
* It's possible not everything you need is available to the script. Check out [ChaiManager.cpp](https://github.com/NivenT/Planet/blob/master/src/ChaiManager.cpp) to see what all you have access to (open an issue if you think of something that should be added)
