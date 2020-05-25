|*********************************************************************
 * entropy
 * custom.inc
 * this file is used for custom mods to entropy.
 ***|




|***
Hooks..

hooks are enabled with /entropy hook enabled

GETINPUTS can be tagged with a subroutine to access and a variable to pass if needed.

e.g.
at the start of the main macro loop there is the following

GETINPUT main_loop swAuto|${maEnv.Find[swAuto].Value}

this line will look at the sub hook_main_loop() ( that you have created in custom.inc) and pass the usable vaiable value for swAuto in this case.

in the custom file you can access thes data the following way:

you youd creat the name of the sub you are calling and process whatever you like in that sub.
- do not use delays


sub hook_main_sub(string _auto)

	/if (${_auto}) {
		/echo hi this is my hook, not yours. get bent..
	}

/return



GETINPUT is asimple mechanic to process /doevents and bind requests. the define can be added just about anywhere in the code that is needed.
As it is, there is ~1 instance at the start of any given sub. However, more can be added with little to no effort depending on where a person
would need the hook placed.

Every time that GETINPUT is hit, if the sub that is added is create in custom.inc, it will be processed.


***|