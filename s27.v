//  FILE:s27.v
module s27( po_18,  clk,  pi_4,  pi_3,  pi_2,  pi_1  );

output  po_18;
input clk, pi_4,  pi_3,  pi_2,  pi_1;

assign net_1=pi_1;
assign net_2=pi_2;
assign net_3=pi_3;
assign net_4=pi_4;
dff dff_5 (net_5, net_17, clk);
dff dff_6 (net_6, net_15, clk);
dff dff_7 (net_7, net_11, clk);
not (net_8, net_1);
nor (net_9, net_7, net_2);
and (net_10, net_6, net_8);
nor (net_11, net_9, net_3);
or (net_12, net_10, net_9);
or (net_13, net_10, net_4);
nand (net_14, net_12, net_13);
nor (net_15, net_14, net_5);
not (net_16, net_15);
nor (net_17, net_15, net_8);
assign po_18=net_16;

endmodule