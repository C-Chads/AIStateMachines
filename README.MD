# State machines for C

A framework and notation for building state machine structs and handler functions, primarily for use in entity component systems.

Entity component systems are at their fastest (smallest number of cache misses, greatest compiletime optimization) when
the same code is run on every object in a contiguous array of objects, with their entire state to be processed that iteration
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

This means fewer instruction cache misses for large, complex state machines.

This is done with the "hcode" system

Every fluffle stores an "hcode" which tells the processing unit whether it has "handled" the fluffle this frame.
when the handler is invoked on a fluffle, its hcode is updated to indicate that it has been handled.

The hcode of the current frame iterates every processing cycle.

Fluffles in the same state can be processed using the exact same code which can be optimized for minimal branching.

This is the code which handles fluffles in identical states.

```c

void process_state_machines(){
	SM_NEXT_HCODE(fluffle);	//"Handled Code" this is a way of avoiding creating an array of booleans.
	//Cache efficient way to handle states.
	for(uint32_t i = 0; i < n; i++)
		if(array[i].state == happy && SM_HCODE_CHECK(fluffle, array[i]))
			SM_HANDLE_fluffle(array+i);
	for(uint32_t i = 0; i < n; i++)
			if(array[i].state == sad && SM_HCODE_CHECK(fluffle, array[i]))
				SM_HANDLE_fluffle(array+i);
	for(uint32_t i = 0; i < n; i++)
			if(array[i].state == singing && SM_HCODE_CHECK(fluffle, array[i]))
				SM_HANDLE_fluffle(array+i);
	for(uint32_t i = 0; i < n; i++)
			if(array[i].state == changing && SM_HCODE_CHECK(fluffle, array[i]))
				SM_HANDLE_fluffle(array+i);
	for(uint32_t i = 0; i < n; i++)		//Handle the idle, init, and default states (for state machines that started with an invalid state)
			if(SM_HCODE_CHECK(fluffle, array[i]))
				SM_HANDLE_fluffle(array+i);
}

```

### How would I use this in a game or engine?

You might have trouble figuring out how you can integrate this code into your engine, for the following reasons:

1) How do I reduce my AI into a state machine?

2) AI's need to react to each other and the environment. How do state machines interact with each other?

3) Large state machines may still be slow due to large strides in their arrays.

4) Complex behaviors can still be very slow.

Problem 1 is a matter of design refactoring, by definition, your computer *is* (an amalgamation of) state machine(s).

Problem 2 has a few different solutions, some of which may be more useful in some situations than others.

1) Add "Perception" variables to your state machines which they can act upon in their state machine code.

	* This is the most elegant solution.

2) Store global state which is unchanging during the execution of the state machines which can be accessed by all of them

	* For state that needs to be read (not written) by all state machines, this is optimal.

3) Store global state from the previous iteration for the state machines to access

	* You should avoid this and where possible cache information from the previous iteration inside each state machine.

You should NOT opt to have state machines interact with each other during the same iteration.

Doing so will not only cause an O(n^2) or at best O(logn) problem to emerge, but also cause significant numbers of cache misses
for large state machines

Problem 3 can be solved by offloading as much state as possible to constant 
(During the SM_HANDLE function call, you can change it between SM iterations)
global state.

Problem 4 can be solved by dividing behavior code across iterations.

### Tips for optimization

1) Try to reduce the amount of per-object state stored. If multiple state machines are going to read the same data, store it in one place.

2) When you want to write state machines which are reactive to their environment (or each other),
perform checks 

3) Avoid large branches per-frame. If a decision in code leads to two
(computationally complex) outcomes, offload it until next iteration.

4) Avoid large amounts of code being executed per-state-machine per iteration. Split code for complex actions
in state machines into actions across multiple iterations.

you can easily do it like this:

```c
STATE(action):
	//your code here.
	//You might have to store some state for the continuation of this action into the state machine.
JMP_STATE(actionpart2);
STATE(actionpart2):
	//The rest of your code here...
```

(Notice again how nice this notation is. Imagine doing this in C++ code with multiple functions and tons of ifs.
It would be 100% unreadable.)

### Important notes

1) Variables (static or not) you declare inside the SM_HANDLE function ***will not*** have the same value the next iteration.
Store state machine variables that must be saved INSIDE THE STATE MACHINE STRUCT.

2) I will continue to write this section later.

~~~~~~~~~~~~~~~~~~~~~~~~

This repository available under the CC0 license

if you desire, credit Gek/DMHSW for "statemachine.h"
