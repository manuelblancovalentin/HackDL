/* USAGE EXAMPLE. Add this snippet to your testbench file:
	`include "/Users/mbvalentin/CLionProjects/HackDL/examples/counter/COUNTER_see.v"

	// SEE (signal-event-effect) ENABLING SIGNAL (WHEN THIS IS TRUE AND (vectornum_total_next > ((`SIM_TEST_SIZE * `SIM_MODEL_COUNT)))) simulation finishes

	reg see_en;
	reg see_done;
	reg see_hold;
	reg see_ready;
	input[2*8:0] mode = 'set';
	integer SEECounter;
	integer seeg_log_file; // File-variable reference where we will save the logs
	integer max_num_regs = 4;

	//Initial log-file setup
	initial begin
		//Open file where we will store the logs
		seeg_log_file = $fopen("seeg.log", "w+");
		print_header;
	end


	// SEE process

	always @(posedge clk) begin
		if (see_en == 1'b0) begin
			// Reset counter 
			SEECounter = 1'b0;
		end else begin

			// Only proceed if not on hold
			if (see_hold == 1'b0) begin
				if (SEECounter < max_num_regs) begin
					// Call event pulse task
					single_event_pulse(SEECounter, `CLK_PERIOD, mode);

					// Check the outputs now 
					#`CLK_PERIOD;
					check_outputs();

					// Increase counter
					SEECounter = SEECounter + 1;

					// See is ready now 
					$display("@%04d INFO: SEE applied on registers [%04d]", $time, SEECounter - 1);
					see_ready = 1'b1;
					see_hold = 1'b1;
				end else begin

					// Process is done
					see_done = 1'b1;
					if (SEECounter == max_num_regs) begin
						// All registers have been upset
						$display ("@%04d INFO: See process done", $time);
					end
				end
			end
		end
	end
*/


//Max number of events (registers) we will toggle
`ifndef MAX_NUM_EVENTS
`define MAX_NUM_EVENTS 4
`endif

// Internal clock
real INTERNAL_CLK_PERIOD = 0.0;

//Verbose flag
`ifndef VERBOSE
`define VERBOSE 1'b1
`endif

// SEE mode parameters
parameter SEE_MODES = 2;
parameter SEE_MODE_SET = 2'b00;
parameter SEE_MODE_SEU = 2'b01;

// Print header of log file 
task print_header;
	begin
		$fdisplay(seeg_log_file, "time, eventid,reg[0],reg[1],xor_data,xor_sum");
	end
endtask
// Print upset logs into log file 
task log_reg;
	input eventid;
	integer eventid;
	begin

		// Print time and eventid first
		$fwrite(seeg_log_file, "@%04d,%d,", $time, eventid);

		case (eventid)
			0: begin
					$fwrite(seeg_log_file,"top_modules.counter_tb.C1.DF1.Q");
					$fwrite(seeg_log_file,"top_modules.counter_tb.C1.DF3.Q");
				 end
			1: begin
					$fwrite(seeg_log_file,"top_modules.counter_tb.C1.DF3.D");
					$fwrite(seeg_log_file,"top_modules.counter_tb.C1.DF1.D");
				 end
			2: begin
					$fwrite(seeg_log_file,"top_modules.counter_tb.C1.DF0.D");
					$fwrite(seeg_log_file,"top_modules.counter_tb.C1.DF2.Q");
				 end
			3: begin
					$fwrite(seeg_log_file,"top_modules.counter_tb.C1.DF2.D");
					$fwrite(seeg_log_file,"top_modules.counter_tb.C1.DF0.Q");
				 end
		endcase
	end
endtask


// Single-Event Transient Pre-clock hold (so that signal gets triggered right before clock) 
task pre_hold;
	input eventid;
	integer eventid;
	real HoldDuration;
	begin

		case (eventid)
			0: HoldDuration = INTERNAL_CLK_PERIOD/2 - 0.500035;
			1: HoldDuration = INTERNAL_CLK_PERIOD/2 - 1.091920;
			2: HoldDuration = INTERNAL_CLK_PERIOD/2 - 3.900224;
			3: HoldDuration = INTERNAL_CLK_PERIOD/2 - 2.563926;
		endcase
		//Wait for HoldDuration
		#(HoldDuration);
	end
endtask


// Single-Event Transient toggle registers
task force_toggle_net;
	input eventid;
	integer eventid;
	begin

		case (eventid)
			0: begin
					force top_modules.counter_tb.C1.DF1.Q = ~top_modules.counter_tb.C1.DF1.Q;
					force top_modules.counter_tb.C1.DF3.Q = ~top_modules.counter_tb.C1.DF3.Q;
				 end
			1: begin
					force top_modules.counter_tb.C1.DF3.D = ~top_modules.counter_tb.C1.DF3.D;
					force top_modules.counter_tb.C1.DF1.D = ~top_modules.counter_tb.C1.DF1.D;
				 end
			2: begin
					force top_modules.counter_tb.C1.DF0.D = ~top_modules.counter_tb.C1.DF0.D;
					force top_modules.counter_tb.C1.DF2.Q = ~top_modules.counter_tb.C1.DF2.Q;
				 end
			3: begin
					force top_modules.counter_tb.C1.DF2.D = ~top_modules.counter_tb.C1.DF2.D;
					force top_modules.counter_tb.C1.DF0.Q = ~top_modules.counter_tb.C1.DF0.Q;
				 end
		endcase
	end
endtask


//Display task info (optional)
task display_pulse_info;
	input eventid;
	integer eventid;
	input see_mode;
	reg [SEE_MODES-1:0] see_mode;
	reg [8*3:1] mode;
	begin

		mode = "SE";
		if (see_mode == SEE_MODE_SEU) begin
			mode = {mode,"U"};
		end else begin
			mode = {mode,"T"};
		end
		if (`VERBOSE == 1'b1)
		begin
		case (eventid)
			0: begin
						$display ("@%04d INFO: %s on top_modules.counter_tb.C1.DF1.Q", $time, mode);
						$display ("@%04d INFO: %s on top_modules.counter_tb.C1.DF3.Q", $time, mode);
				 end
			1: begin
						$display ("@%04d INFO: %s on top_modules.counter_tb.C1.DF3.D", $time, mode);
						$display ("@%04d INFO: %s on top_modules.counter_tb.C1.DF1.D", $time, mode);
				 end
			2: begin
						$display ("@%04d INFO: %s on top_modules.counter_tb.C1.DF0.D", $time, mode);
						$display ("@%04d INFO: %s on top_modules.counter_tb.C1.DF2.Q", $time, mode);
				 end
			3: begin
						$display ("@%04d INFO: %s on top_modules.counter_tb.C1.DF2.D", $time, mode);
						$display ("@%04d INFO: %s on top_modules.counter_tb.C1.DF0.Q", $time, mode);
				 end
			endcase
		end
	end
endtask


//Single-Event Transient Delay (toggle up time)
task hold_toggle;
	input eventid;
	integer eventid;
	input see_mode;
	reg [SEE_MODES-1:0] see_mode;
	real SEEDuration;
	begin

		if (see_mode == SEE_MODE_SET) begin
			case (eventid)
				0: SEEDuration = 1.000070;
				1: SEEDuration = 2.183840;
				2: SEEDuration = 7.800448;
				3: SEEDuration = 5.127851;
			endcase
		end else begin
			SEEDuration = INTERNAL_CLK_PERIOD;

		end
		//Wait for SEEDuration
		#(SEEDuration);
	end
endtask


// Single-Event Transient Release
task release_net;
	input eventid;
	integer eventid;
	begin

		case (eventid)
			0: begin
					release top_modules.counter_tb.C1.DF1.Q;
					release top_modules.counter_tb.C1.DF3.Q;
				 end
			1: begin
					release top_modules.counter_tb.C1.DF3.D;
					release top_modules.counter_tb.C1.DF1.D;
				 end
			2: begin
					release top_modules.counter_tb.C1.DF0.D;
					release top_modules.counter_tb.C1.DF2.Q;
				 end
			3: begin
					release top_modules.counter_tb.C1.DF2.D;
					release top_modules.counter_tb.C1.DF0.Q;
				 end
		endcase
	end
endtask


//Setup pulse on registers according to task number
task single_event_pulse;
	input eventid;
	integer eventid;
	input CLK_PERIOD;
	real CLK_PERIOD;
	input [SEE_MODES-1:0] see_mode;
	begin
		//Avoid overflowing eventid
		if (eventid >= `MAX_NUM_EVENTS)
			eventid = (eventid%`MAX_NUM_EVENTS);

		//Set clock period value
		INTERNAL_CLK_PERIOD = CLK_PERIOD;
		//Prehold so signal starts right before clock edge
		pre_hold(eventid);
		//Force toggle
		force_toggle_net(eventid);
		//Display info
		display_pulse_info(eventid, see_mode);
		//Hold value
		hold_toggle(eventid, see_mode);
		//Release toggle
		release_net(eventid);
	end
endtask
