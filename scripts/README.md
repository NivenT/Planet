# Scripts Folder

This is where the different scripts used by the game (e.g. Enemy logic) are stored.

The ChaiScript language is essentially C++ if it were a scripting language. Beyond what's in this foler, you can [look here](http://chaiscript.com/examples.html#ChaiScript_Basic_Example) for more coding examples. Do not be afraid to open an issue if you have questions about writing scripts.

# Details
Each script makes use of 5 implicit parameters

* `params` which is of type `UpdateParams` and contains various information related to updates.
* `ecs` which is of type `nta::ECS*` that can be used for accessing entites and components.
* `self` which is of type `nta::EntityID` that is the ID of the entity controlled by the script.
* `physics` which is of type `PhysicsComponent*`. This is the PhysicsComponent associated to `self`
  * This is potentially null (but almost certainly won't be in practise)
  * This is useful for scripts controlling enemy behavior (e.g. ghost.chai)
* `pickup` which is of type `PickupComponent*`. This is the PickupComponent associated to `self`
  * This is potentially null.
  * This is useful for scripts controlling item behavior (e.g. shovel.chai)

# Issues

* Currently, scripts must end with the name of the funtion they are meant to run (c.f. shoe.chai)
  * Furthermore, no two scripts can name their function the same thing
* It's possible not everything you need is available to the script. Check out [ChaiManager.cpp](https://github.com/NivenT/Planet/blob/master/src/ChaiManager.cpp) to see what all you have access to (open an issue if you think of something that should be added)
