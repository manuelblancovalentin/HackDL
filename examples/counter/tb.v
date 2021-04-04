// Initial comment

//Test 2

/* This is a fake multi-line comment */ //`include "mymama.v"

/* 

	Test 3

*/

/*
	// Test 4 #23
	module
*/


`include "counter.v"

module counter_tb;

	// clock
	reg clk; 

	// reset
	reg rstb;

	// Enable signal
	reg in;

	// out Q
	wire [3:0] Q; //assign Q[0] = 1;

	// Instantiate counter
	counter C1 (.clk(clk), .in(in), .rstb(rstb), .Q(Q));


	//counter #(N=2,K=2) C2 (.clk(clk), .in(in), .rstb(rstb), .Q(Q));

	initial begin

		// Init vars
		in = 1'b0; 	 // Disable counter

		// Start simulation by setting clock to zero
		clk = 1'b0;

		// Pulse reset
		#2 rstb = 1'b0; 
		#1 rstb = 1'b1;

		// Keep in=0 for a while (output should remain 0)
		#5 in = 1'b1;

		// Now reset again
		#182 rstb = 1'b0; 
		#10 rstb = 1'b1;

		// Disable counter
		#40 in = 1'b0;

		#20 $finish;

	end

	// Generate clock signal
	always begin
		#5 clk = ~clk;
	end



endmodule