CSceneNodeAnimatorNewton
------------------------

An animator that moves the scene nodes using the Newton Game Dynamics(v1.53) lib.
This means that you have to have the Newton Game Dynamics SDK, to use this animator.

After the animator is added to a scene node it takes care 
of updating the newton world, so there's no need to add 
an update call in the main loop. 
Only one newton body is created per animator, and the 
animator is only expecting one animateNode call per frame, 
that means that the animator can only be added to one scene node.

To use the animator with the newton functions use getNewtonBody() to get the body of a specific animator, 
and getNewtonWorld() to get the newton world.

Enjoy
- JonLT

--------
Changes:
--------

08. july 2007:

- first added to the SVN

16. july 2007:

- the body is created in the constructor, so it can be accesed right after the creation of the animator.

- gravity is now a core::vector3df, in case you want sideways gravity :)

- addForce and addTorque functions has been addded (by using more than the scene node as user data for the newton body)

- if the animator is added to more than one scene node, an error message is printed (using printf), and the animator is removed from the scene node.