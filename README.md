
<img src="./__res__/artwork/logo_blur.png" alt="drawing" width="400"/>

# HackDL: a C++ tool for automated hardware description language manipulation

## What is this for?
This is missing

## Installation

This is missing

## How to use

This is missing



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


## Example: Counter

```
top_modules
 └---counter_tb
     ├---C1
     │   ├---D0
     │   │   ├---arrayspan : [0:0]
     │   │   ├---bitspan : [0:0]
     │   │   ├---type : wire
     │   │   └---value : 
     │   ├---D1
     │   │   ├---arrayspan : [0:0]
     │   │   ├---bitspan : [0:0]
     │   │   ├---type : wire
     │   │   └---value : 
     │   ├---D2
     │   │   ├---arrayspan : [0:0]
     │   │   ├---bitspan : [0:0]
     │   │   ├---type : wire
     │   │   └---value : 
     │   ├---DF0
     │   │   ├---D
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---Q
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : reg
     │   │   │   └---value : 
     │   │   ├---clk
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---in
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---nQ
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---parameters
     │   │   │   ├---J : = 3
     │   │   │   ├---K : = 4
     │   │   │   └---N : = 2
     │   │   ├---ports
     │   │   │   ├---inout : 6
     │   │   │   ├---input
     │   │   │   │   ├---0 : clk
     │   │   │   │   ├---1 : rstb
     │   │   │   │   ├---2 : in
     │   │   │   │   └---3 : D
     │   │   │   └---output
     │   │   │       ├---0 : Q
     │   │   │       └---1 : nQ
     │   │   ├---ref : dff
     │   │   └---rstb
     │   │       ├---arrayspan : [0:0]
     │   │       ├---bitspan : [0:0]
     │   │       ├---type : wire
     │   │       └---value : 
     │   ├---DF1
     │   │   ├---D
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---Q
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : reg
     │   │   │   └---value : 
     │   │   ├---clk
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---in
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---nQ
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---parameters
     │   │   │   ├---J : = 3
     │   │   │   ├---K : = 4
     │   │   │   └---N : = 2
     │   │   ├---ports
     │   │   │   ├---inout : 6
     │   │   │   ├---input
     │   │   │   │   ├---0 : clk
     │   │   │   │   ├---1 : rstb
     │   │   │   │   ├---2 : in
     │   │   │   │   └---3 : D
     │   │   │   └---output
     │   │   │       ├---0 : Q
     │   │   │       └---1 : nQ
     │   │   ├---ref : dff
     │   │   └---rstb
     │   │       ├---arrayspan : [0:0]
     │   │       ├---bitspan : [0:0]
     │   │       ├---type : wire
     │   │       └---value : 
     │   ├---DF2
     │   │   ├---D
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---Q
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : reg
     │   │   │   └---value : 
     │   │   ├---clk
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---in
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---nQ
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---parameters
     │   │   │   ├---J : = 3
     │   │   │   ├---K : = 4
     │   │   │   └---N : = 2
     │   │   ├---ports
     │   │   │   ├---inout : 6
     │   │   │   ├---input
     │   │   │   │   ├---0 : clk
     │   │   │   │   ├---1 : rstb
     │   │   │   │   ├---2 : in
     │   │   │   │   └---3 : D
     │   │   │   └---output
     │   │   │       ├---0 : Q
     │   │   │       └---1 : nQ
     │   │   ├---ref : dff
     │   │   └---rstb
     │   │       ├---arrayspan : [0:0]
     │   │       ├---bitspan : [0:0]
     │   │       ├---type : wire
     │   │       └---value : 
     │   ├---DF3
     │   │   ├---D
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---Q
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : reg
     │   │   │   └---value : 
     │   │   ├---clk
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---in
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---nQ
     │   │   │   ├---arrayspan : [0:0]
     │   │   │   ├---bitspan : [0:0]
     │   │   │   ├---type : wire
     │   │   │   └---value : 
     │   │   ├---parameters
     │   │   │   ├---J : = 3
     │   │   │   ├---K : = 4
     │   │   │   └---N : = 2
     │   │   ├---ports
     │   │   │   ├---inout : 6
     │   │   │   ├---input
     │   │   │   │   ├---0 : clk
     │   │   │   │   ├---1 : rstb
     │   │   │   │   ├---2 : in
     │   │   │   │   └---3 : D
     │   │   │   └---output
     │   │   │       ├---0 : Q
     │   │   │       └---1 : nQ
     │   │   ├---ref : dff
     │   │   └---rstb
     │   │       ├---arrayspan : [0:0]
     │   │       ├---bitspan : [0:0]
     │   │       ├---type : wire
     │   │       └---value : 
     │   ├---Q
     │   │   ├---arrayspan : [0:0]
     │   │   ├---bitspan : [3:0]
     │   │   ├---type : wire
     │   │   └---value : 
     │   ├---clk
     │   │   ├---arrayspan : [0:0]
     │   │   ├---bitspan : [0:0]
     │   │   ├---type : wire
     │   │   └---value : 
     │   ├---in
     │   │   ├---arrayspan : [0:0]
     │   │   ├---bitspan : [0:0]
     │   │   ├---type : wire
     │   │   └---value : 
     │   ├---ports
     │   │   ├---inout : 6
     │   │   ├---input
     │   │   │   ├---0 : clk
     │   │   │   ├---1 : in
     │   │   │   └---2 : rstb
     │   │   └---output
     │   │       └---0 : Q
     │   ├---ref : counter
     │   └---rstb
     │       ├---arrayspan : [0:0]
     │       ├---bitspan : [0:0]
     │       ├---type : wire
     │       └---value : 
     ├---Q
     │   ├---arrayspan : [0:0]
     │   ├---bitspan : [3:0]
     │   ├---type : wire
     │   └---value : 
     ├---clk
     │   ├---arrayspan : [0:0]
     │   ├---bitspan : [0:0]
     │   ├---type : reg
     │   └---value : 
     ├---in
     │   ├---arrayspan : [0:0]
     │   ├---bitspan : [0:0]
     │   ├---type : reg
     │   └---value : 
     ├---ref : module
     └---rstb
         ├---arrayspan : [0:0]
         ├---bitspan : [0:0]
         ├---type : reg
         └---value : 
         
```

## Citing us
Please, consider citing us in your publications if you found this code useful. If so, please use the following bib entry:

```
@misc{hackdl,
  author = {B. Valentin, Manuel and Ogrenci-Memik, Seda and Fahim, Farah and Miranda, Llovizna},
  title = {HackDL: a C++ tool for automated hardware description language manipulation},
  year = {2021},
  publisher = {GitHub},
  journal = {GitHub repository},
  howpublished = {\url{https://github.com/manuelblancovalentin/HackDL}
}
```