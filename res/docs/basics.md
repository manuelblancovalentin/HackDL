


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