# Rincewind compiler

Rincewind is a magic machine built by druids, it can talk with anyone in any language. This compiler pronounce the enchanted words that allows you to talk with this machine without being evaporated. Say something wise, the elders are waiting.


## How to perform the invocation ritual

* Download [Win-Bison](https://sourceforge.net/projects/winflexbison/files/), add bison.exe to system's variable.
* Download [Re/Flex](https://github.com/Genivia/RE-flex), add reflex.exe to the system's variable.
* Copy and paste `unicode` and `lib` directories from reflex to your project.
* Open the transdimensional gate and talk through Rincewind.

## What can I do if Rincewind do not compile?
Does that means I am not the chosen one?  
* Review that you have a correct path in the `include directories` that points to `YOUR_PATH\ReFlex\include`. To do this, go to the project properties, then to `VC++ Directories` tab.
* Check that you added ReFlex and Bison to your system Path.
* Check your Bison version, to compile the `parser.y` file you need at least `3.2` version
* If Bison and ReFlex are generating all the files, but Rincewind is unable to compile, check whether or not you have the generated files loaded in the project under `Generated Files` folder, if not, do it.
* If you still cannot talk with the elders, well, you may not be the chosen one. (Then contact with the elders through email).