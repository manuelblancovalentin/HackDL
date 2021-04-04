`include "dff.v"

module counter(clk, in, rstb, Q);

	// Define inputs
	input clk;
	input in;
	input rstb;

	// Define outputs 
	output wire [3:0] Q;

	// Temp wires
	wire D0;
	wire D1; 
	wire D2;

	// Instantiate bits
	dff DF0 (.clk(clk), .rstb(rstb), .in(in), .D(D0), .Q(Q[0]), .nQ(D0));
	dff DF1 (.clk(D0), .rstb(rstb), .in(in), .D(D1), .Q(Q[1]), .nQ(D1));
	dff DF2 (.clk(D1), .rstb(rstb), .in(in), .D(D2), .Q(Q[2]), .nQ(D2));
	dff DF3 (.clk(D2), .rstb(rstb), .in(in), .D(D3), .Q(Q[3]), .nQ(D3));


endmodule