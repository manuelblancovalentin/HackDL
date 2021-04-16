
## Verilog Cheatsheet

### Line-break instructions
These instructions start and look for a new-line ending. Anything between their start-word and the next \n (end of the line) will be considered as part of the instruction.

| Instruction   |      Example      |  Start word | End word | Ignore Recursive? |
|---------------|:-----------------:|:------------:|:---------:|:---------------:|
| include		  | ``include <module>` |  \`         | \n |✅|
| define       | ``define <module>`  | \`          | \n |✅|
| timescale    | ``timescale 1ns/1ps`| \`          | \n |✅|
| Single-line Comment | `\\ Comment` | \\\         | \n |✅|


These instructions are easily parsed precisely because they do not need accumulation at all. Once you find a word, you can match it. Also, we don't care about any further keywords that might appear. For instance, an include followed by the word define in the same sentence, should be captured as an include. The define is ignored. Thus, we can ignore any recursive search for other keywords.


### Recursivity-blocking instructions
Say that we find the start of a block comment ``/*``, we know that we have to keep looking until we find the end of that comment ``*/``. That can happen either in the same line or in following lines. If it's not in the same line, then we gotta ignore whatever there is on those lines until we find the endword. No matter if there's an initial/always/module block, whatever. It's all a comment. All of these are nul. These are recursivity-blocking instructions.

| Instruction   |      Example      |  Start word | End word | Ignore Recursive? |
|---------------|:-----------------:|:------------:|:---------:|:---------------:|
| Multi-line comment | ``\* example *\`` |  ``\*`` | ``*\`` |✅|

> TODO