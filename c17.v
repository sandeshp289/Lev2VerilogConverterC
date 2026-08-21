//  FILE:c17.v
module c17( po_13,  po_12,  pi_5,  pi_4,  pi_3,  pi_2,  pi_1  );

output  po_13,  po_12;
input  pi_5,  pi_4,  pi_3,  pi_2,  pi_1;

assign net_1=pi_1;
assign net_2=pi_2;
assign net_3=pi_3;
assign net_4=pi_4;
assign net_5=pi_5;
nand (net_6, net_1, net_3);
nand (net_7, net_3, net_4);
nand (net_8, net_7, net_2);
nand (net_9, net_7, net_5);
nand (net_10, net_8, net_6);
nand (net_11, net_8, net_9);
assign po_12=net_10;
assign po_13=net_11;

endmodule