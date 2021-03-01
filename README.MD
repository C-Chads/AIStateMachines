# State machines for C

A basic framework for building state machine structs and handler functions, primarily for use in entity component systems.

Entity component systems are at their fastest (smallest number of cache misses, greatest compiletime optimization) when
the same code is run on every object in a contiguous array of objects, with their entire state (Except global state)
being contained in that contiguous space.

Unfortunately, programmers like to write large, complicated game entities with dozens of behaviors.

The key problem of taking advantage of ECS in a game engine is then subdividing the behavior of the whole entity
into many smaller tasks which can be executed in parallel or in batches.

This is quite an honorous task in and of itself, but,
additionally, there is a massive disconnect between the way in which entity logic code and game logic code (I.E. how an 
enemy decides to move/attack versus detecting collisions) are written that often makes writing solutions rather difficult.

entity logic code can easily turn into spaghetti, often spanning multiple functions depending
on the current state of the entity, which obviously isn't good for ECS. it's not an easy problem to solve.

This header-only C99 library attempts to solve both of these problems in one fowl swoop
by providing a unique way of writing entity logic code that makes it easy and intuitive to subdivide behaviors into per-frame code,
which is very cache friendly and very easy to follow, avoiding the spaghetti mess of member functions that riddle
many-a-C++-codebase and demolish cache.

To illustrate, let's look at the attached example program.

"fluffle" is an extremely basic state machine with only two variables: "mood" and "target"

```c
typedef struct {
	SM_VARS;
	int mood;
	int target;
} fluffle;
```

Every time the fluffle is in the idle state, it decides on a mood to shift to.

```c
STATE(idle):
{
	int a = (int)(rand()%256);
	a -= 128;
	sm->target = a + sm->mood;
	if(sm->target < -100)
		sm->target = -100;
	if(sm->target > 100)
		sm->target = 100;
}
```

Across multiple iterations, it will slowly shift to this target mood:

```c
STATE(changing):
//puts("Changing...\n");
if(sm->target > sm->mood)
	{sm->mood++; JMP_STATE(changing);}
if(sm->target < sm->mood)
	{sm->mood--; JMP_STATE(changing);}
```

After it reaches the target mood, it takes the correct course of action next iteration.

```c
//We have reached the target mood.
if(sm->mood > 80)
	JMP_STATE(singing);
if(sm->mood > 30)
	JMP_STATE(happy);
if(sm->mood < 30)
	JMP_STATE(sad);
JMP_STATE(idle);
```

If a fluffle's mood is greater than 80, it sings.

```c
STATE(singing):
puts("Fo la lo di!\n");
JMP_STATE(idle);
```

if a fluffle's mood is greater than 50, it makes happy noises.

```c
STATE(happy):
puts("Happy noises!\n");
JMP_STATE(idle);
```

if a fluffle's mood is less than 30, it will make sad noises.

```c
STATE(sad):
puts("Sad noises.\n");
JMP_STATE(idle);
```

You can tell very clearly what's happening here

1. JMP_STATE sets the state of the fluffle and upon the next execution of the handler, it will begin execution there.

2. the STATE(statename) statements are the locations in the code that the handler jumps to upon the next iteration

In fact, the whole thing just breaks down to a switch-case underneath, it may seem like some sort of black magic, but
in reality this is just a cleaner, nicer, easier-to-read way of writing the same code you'd write otherwise.

Now you can probably tell why this is so useful!

1. Subdividing multi-frame tasks is extremely simple and intuitive- Everyone who's ever used goto in BASIC can figure it out

2. Loops... Look like loops! In most AI/entity component programming implementations i've seen, the loops do not look like loops.

This makes debugging, analyzing, or even just READING the code and following the execution a real PITA!

I could have written code that looked more like this:

```c
void handle_fluffle(fluffle* f){
	if(state == idle){
		//make decision...
		break;
	}
	if(state == changing){
		if(target < mood) {mood--;break;}
		if(target > mood) {mood++;break;}
		if(mood > 80) state = singing;
		else if(mood > 50) state = happy;
		//...
		break;
	}
	if(state == singing){
		puts("Fa lo di da!\n");
		break;
	}
}
```

Or worse yet, I could break each state out into separate functions (Not shown)

Even though changing is very clearly a loop with the statemachine code, it is
not very easy to see in the "if" version. Additionally it's harder to follow, and there is no handling of "default".

Even if you change it to a switch case (as exists underneath the state machine version) it is still not easy to tell.

Meanwhile with the statemachine macros, you can see "JMP" (like the assembly language instruction)
so you know very clearly what's happening at a glance.

3. Multiple entities in the exact same state will execute the exact same code.

This can be used to speed up the processing of (larger, more complex) state machines by processing them in batches.

```c
//in your EC code...
uint8_t handled[n]; 
//Presumably you zero-out handled
for(int i = 0; i < n; i++)
	if(array[i].state == changing && !handled[i]){
		SM_HANDLE_fluffle(array+i);
		handled[i] = 1;
	}
//Repeat for other states
```

~~~~~~~~~~~~~~~~~~~~~~~~

This repository available under the CC0 license

if you desire, credit Gek/DMHSW for "statemachine.h"
