[![Build Status](https://travis-ci.com/manuelblancovalentin/HackTMR.svg?branch=master)](https://travis-ci.com/manuelblancovalentin/HackTMR)


<img src="res/artwork/HackTMR.png" alt="drawing" width="400"/>

# HackTMR: a C++ tool for automated hardware description language manipulation

## What is this for?
HackTMR is a tool for automatic HLS manipulation. It allows the user 
to perform multiple tasks with the purpose of modifying, 
enhancing or visualizing custom HLS code. 

Although this tool is currently only accepting verilog/system-verilog 
codes, we expect to expand it in the future to other languages
such as VHDL. 

The tool acts mainly as a (pseudo)compiler, parsing the HDL language 
introduced by the user in order to get the hierarchy of the design first.
Depending on the structure of the project, the top_design can be inferred
automatically by HacKTMR. As it will be seen later, passing a testbench file
where the top_module is instantiated usually renders the best performance
while using this tool. 

HackTMR is able to detect all verilog/systemverilog instructions, however 
it currently ignores most behavioral blocks. As the original main purpose of this tool
was to provide an easy way to visualize and modify structural data
behavioral blocks were considered not essential (as it is always possible
for the user to provide a gate-level description of the behavioral code and
use that as input to HackTMR). We do expect to be able to parse these blocks in
the future, but for now using and decoding them would slow down the execution of
our tool, thus becoming counter-productive at the current stage of the software.

The code is written in C++ and although it has some dependencies, we have tried to
keep it as self-contained as possible. Writing it in C++ would allow users from any
platform to compile, install and use it. 

Below we introduce HackTMR functionalities and how to use them in your code.




## Installation

```bash
$> git clone git@github.com/manuelblancovalentin/HackTMR
$> cd HackTMR
$> # Install dependencies
$> chmod +x build.sh && chmod +x ./build/dependencies.sh
$> ./build/dependencies.sh
$> ./build.sh --cc=clang --cxx=clang++
```

## How to use

In order to get a list with all the possible options and functionalities HackTMR has
simply type:
```bash
$> ./HackDL -h
```

A complete list of available arguments can be seen [here](./res/docs/basics.md).


### Hierarchy inspection
Most digital architectures have a pretty well idea of what the hierarchy of their
HDL code looks like, but sometimes it might happen that our designs get so big that
we need to take a look at the whole structure; or maybe we need to use or modify
someone else's code.

In those cases, it is useful to take a look at the structure of the design. Although
you can see this hierachy using property tools like Cadence Genus, Xcelium or others,
these tools are usually installed in a single server to which we might not necessarily
have access at all times. Apart from that, if that specific code is not compilable,
those tools will not allow you to visualize the structure. Furthermore, if your code is
missing some libraries or files, it is impossible to use such tools to check the hierarchy
of the design.

HackTMR offers HDL designers the ability to parse a set of HDL codes and display the
hierarchy of the design even if the code is not compilable or it has missing libraries
or files. Our software has been optimized to run as fast as possible, so that such tasks
can be executed rapidly and efficiently in order to guarantee that the design workflow can
move forward.

The easiest way to get the hierarchy of a certain design is to point HackTMR to
such file with the ``--source`` or ``-s`` flag. This flag should point to a
``verilog`` or ``systemverilog`` file containing the top_module (although it
can contain any other modules apart from the top).

If such file has dependencies on other files, and uses instructions such as
`` `include <other_file>.{v | sv} `` then you need to specify the path or paths
to the folders where these files can be found and HackTMR will look for them
automatically. This path can be specified with the ``--dependencies`` or ``-d``
flag.

Any instruction passed to HackTMR will generate some outputs. The outputs
will be stored in the directory pointed by the flag ``--output`` or ``-o``.
If not specified, the software will try to create a new directory in the
current working directory.

The name of the outputs can also be specified using the ``--name`` or ``-n``
flag.

Thus, the pattern for an instruction to get the hierarchy of a design is
shown below:

```bash
$> HackDL -n <out_name> 
          -s <path_file_top_design>.{v | sv}
          -d <path_to_sources> 
          -o <output_path>
```

For a particular example see [this](./res/docs/basics.md).


### Module renaming/extraction/deletion
> TODO

### Triple Module Redundancy (TMR)
Triplicating a design is almost as easy as getting its hierarchy.
The following block shows a generic instruction to triplicate all 
instances, modules and/or signals that follow the pattern specified 
by `` -p ``.

```bash
$> HackDL -n <out_name> 
          -s <path_file_top_design>.{v | sv}
          -d <path_to_sources> 
          -o <output_path>
          --tmr <int>
          --tmr_suffix <string>
```

For a particular example see [this](./res/docs/tmr.md).


###  Generation of signals monitor testbench
> TODO

### Single-Event Effects modeling
> TODO


## How does HackTMR work?
If you are curious about the internal mechanism of HackTMR and want to understand
better how this code works, feel free to analyze the code or check [this](./res/docs/internal.md) out.

## Citing us
Please, consider citing us in your publications using the following bibtex entry, in case you find this code useful:

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