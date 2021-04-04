/* USAGE EXAMPLE. Add this snippet to your testbench file:
	`include "/Users/mbvalentin/CLionProjects/HackDL/examples/counter/COUNTER_monitors.v"
// Monitor registers
reg[100*8:0] monitor_prefix;
always @(posedge clk) begin
	monitor_prefix = "pos";

	monitor_instances(monitor_prefix);
end
// Initial setups
initial begin
	// Open file where we will safe the seeg logs (if doesn't exist, create it)
	seeg_log_file = $fopen("seeg.log", "w+");
	// Print header with all monitored registers 
	print_header; // this function is defined in this file

// Rest of your custom initial functions ... 

end
IMPORTANT NOTE: REMEMBER TO CLOSE THE FILE USING "$fclose(seeg_log_file);" before calling $finish;*/

// Print header with all the registers 
task print_header;
	begin
		$fdisplay(seeg_log_file,"time,id,top_modules.counter_tb.C1.DF0.D,top_modules.counter_tb.C1.DF0.Q,top_modules.counter_tb.C1.DF1.D,top_modules.counter_tb.C1.DF1.Q,top_modules.counter_tb.C1.DF2.D,top_modules.counter_tb.C1.DF2.Q,top_modules.counter_tb.C1.DF3.D,top_modules.counter_tb.C1.DF3.Q");
	end
endtask;


// Monitor registers task
task monitor_instances;
	input[100*8:0] prefix;
	begin
		$fdisplay(seeg_log_file, "@%04d,%s,%X,%X,%X,%X,%X,%X,%X,%X",$time,prefix,top_modules.counter_tb.C1.DF0.D,top_modules.counter_tb.C1.DF0.Q,top_modules.counter_tb.C1.DF1.D,top_modules.counter_tb.C1.DF1.Q,top_modules.counter_tb.C1.DF2.D,top_modules.counter_tb.C1.DF2.Q,top_modules.counter_tb.C1.DF3.D,top_modules.counter_tb.C1.DF3.Q);
	end
endtask

