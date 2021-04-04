module dff(clk, rstb, in, D, Q, nQ);
	
	parameter N = 2;
	parameter K = 4;
	parameter J = 3;

	// Define input buses and signals
	input clk; // Clock signal
	input rstb; // Reset signal
	input in; // Enable signal (enables change of the data)
	input D; // D input (1-bit)

	// Define output buses and signals
	output reg Q; // Q output (1-bit)
	output wire nQ; // ~Q

	assign nQ = ~Q;

    CBKF auto(.clk(clk));

	always @(negedge rstb) begin
		Q <= 1'b0;
	end

	always @(posedge clk)
	begin
		// if reset, go to 0
		if ((rstb == 1'b1) & (in == 1'b1)) begin
			Q <= D;
		end
	end

endmodule