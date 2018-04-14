#LambertW

This code came from [here](http://keithbriggs.info/software.html). I did not see anything about who was allowed to use it, so I hope Keith Briggs is ok with this.

This implements the [lambert W](https://www.wikiwand.com/en/Lambert_W_function) function which satisfies

`W(x)exp(W(x)) = x` 

In this project, this function is needed to covert mouse coordinates to world coordinates because of how the [planet shader](https://github.com/NivenT/Planet/blob/master/shaders/planet.vert) works.