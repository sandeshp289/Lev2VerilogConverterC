/////////////////////////////////////////////////////////////////////////////////////
// File       : lev2vlog_combo.c
// Language   : C/C++
// Description: Lev to Verilog Convertor for combinational circuits
// Author     : Sandesh Prabhakar
// Created    : February 18, 2009
// Modified   : August 21, 2026
// Version    : 1.0
//
// Copyright (c) 2026 Sandesh Prabhakar
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
   G_JUNK,         /* 0 */
   G_INPUT,        /* 1 */
   G_OUTPUT,       /* 2 */
   G_XOR,          /* 3 */
   G_XNOR,         /* 4 */
   G_DFF,          /* 5 */
   G_AND,          /* 6 */
   G_NAND,         /* 7 */
   G_OR,           /* 8 */
   G_NOR,          /* 9 */
   G_NOT,          /* 10 */
   G_BUF,          /* 11 */
};

struct node
{
	int data_value;
	node *link;
};

////////////////////////////////////////////////////////////////////////
// circuit class
////////////////////////////////////////////////////////////////////////

class circuit
{
    // circuit information
    int numgates;       // total number of gates (faulty included)
    int maxlevels;      // number of levels in gate level ckt
    int numInputs;	// number of inputs
    int numOutputs;	// number of outputs
    unsigned char *gtype;	// gate type
    short *fanin;		// number of fanins of gate
    short *fanout;		// number of fanouts of gate
    int *levelNum;		// level number of gate
    int **faninlist;		// fanin list of gate
    int **fanoutlist;		// fanout list of gate
    node *storegates_pi;
    node *storegates_po;

    // you may wish to add more private variables here

public:
    circuit(char *);            // constructor

    // you may wish to add more function declarations (or public variables) here
     void add_gates_pi (int data); 
     void add_gates_po (int data);
};

////////////////////////////////////////////////////////////////////////
// constructor: reads in the *.lev file and builds basic data structure
// 		for the gate-level ckt
////////////////////////////////////////////////////////////////////////
circuit::circuit(char *cktName)
{
    FILE *inFile, *outFile;
    char fName[40];
    int i, j, count;
    char c;
    int gatenum, junk, total_clauses;
    int f1, f2, f3;

    storegates_pi = new node;
    storegates_pi=NULL;

    storegates_po = new node;
    storegates_po=NULL;

    strcpy(fName, cktName);
    strcat(fName, ".lev");
    inFile = fopen(fName, "r");
    if (inFile == NULL)
    {
	fprintf(stderr, "Can't open .lev file\n");
	exit(-1);
    }

    numgates = maxlevels = 0;
    numInputs = numOutputs = 0;
    total_clauses=0;

    fscanf(inFile, "%d", &count);	// number of gates
    fscanf(inFile, "%d", &junk);	// skip the second line

    // allocate space for gates data structure
    gtype = new unsigned char[count];
    fanin = new short[count];
    fanout = new short[count];
    levelNum = new int[count];
    faninlist = new int * [count];
    fanoutlist = new int * [count];

    // now read in the circuit
    for (i=1; i<count; i++)
    {
	fscanf(inFile, "%d", &gatenum);
	fscanf(inFile, "%d", &f1);
	fscanf(inFile, "%d", &f2);
	fscanf(inFile, "%d", &f3);

	numgates++;
	gtype[gatenum] = (unsigned char) f1;
	if (gtype[gatenum] > 13)
	    printf("gate %d is an unimplemented gate type\n", gatenum);
	else if (gtype[gatenum] == G_INPUT)
	    numInputs++;
	else if (gtype[gatenum] == G_OUTPUT)
	    numOutputs++;

        if(gtype[gatenum] == 1)
        add_gates_pi(gatenum);

        if(gtype[gatenum] == 2)
        add_gates_po(gatenum);

	f2 = (int) f2;
	levelNum[gatenum] = f2;

	if (f2 >= (maxlevels))
	    maxlevels = f2 + 5;

	fanin[gatenum] = (int) f3;
	// now read in the faninlist
	faninlist[gatenum] = new int[fanin[gatenum]];
	for (j=0; j<fanin[gatenum]; j++)
	{
	    fscanf(inFile, "%d", &f1);
	    faninlist[gatenum][j] = (int) f1;
	}

	for (j=0; j<fanin[gatenum]; j++) // followed by samethings
	    fscanf(inFile, "%d", &junk);

	// read in the fanout list
	fscanf(inFile, "%d", &f1);
	fanout[gatenum] = (int) f1;

	fanoutlist[gatenum] = new int[fanout[gatenum]];
	for (j=0; j<fanout[gatenum]; j++)
	{
	    fscanf(inFile, "%d", &f1);
	    fanoutlist[gatenum][j] = (int) f1;
	}

	// skip till end of line
	while ((c = getc(inFile)) != '\n' && c != EOF)
	    ;
    }	// for (i...)
    fclose(inFile);

    printf("Successfully read in circuit:\n");
    printf("\t%d PIs.\n", numInputs);
    printf("\t%d POs.\n", numOutputs);
    printf("\t%d total number of gates\n", numgates);
    printf("\t%d levels in the circuit.\n", maxlevels / 5);

    strcpy(fName, cktName);
    strcat(fName, ".v");
    outFile = fopen(fName, "w");
    if (outFile == NULL)
    {
	fprintf(stderr, "Can't open .v file\n");
	exit(-1);
    }

    fprintf(outFile, "//  FILE:%s.v\n", cktName);

    fprintf(outFile, "module %s(", cktName);

    node *current = storegates_po; 
      while(current != NULL)
      { 
      fprintf(outFile, " po_%d, ", current->data_value); 
      current=current->link;
      }

      node *current1 = storegates_pi; 
      i=0;
      while(current1 != NULL)
      { 
      i++;
      if(i<numInputs) 
      fprintf(outFile, " pi_%d, ", current1->data_value); 
      else
      fprintf(outFile, " pi_%d ", current1->data_value); 

      current1=current1->link;
      }

      fprintf(outFile, " );\n\n");	     

      fprintf(outFile, "output ");

      i=0;
      node *current2 = storegates_po; 
      while(current2 != NULL)
      { 
      i++;
      if(i<numOutputs) 
      fprintf(outFile, " po_%d, ", current2->data_value); 
      else
      fprintf(outFile, " po_%d;\n", current2->data_value); 
      current2=current2->link;
      }
      
      fprintf(outFile, "input ");
      node *current3 = storegates_pi; 
      i=0;
      while(current3 != NULL)
      { 
      i++;
      if(i<numInputs) 
      fprintf(outFile, " pi_%d, ", current3->data_value); 
      else
      fprintf(outFile, " pi_%d;\n\n", current3->data_value); 

      current3=current3->link;
      }



	for(i=1;i<=numgates;i++)
	{
   		switch(gtype[i])
		{
 		case 0:
                case 1:
                       fprintf(outFile, "assign net_%d=pi_%d;\n", i, i);
                       break;
                case 2:
                       fprintf(outFile, "assign po_%d=net_%d;\n", i, faninlist[i][0]); 
                       break;
                //XOR
                case 3:
                       fprintf(outFile, "xor (net_%d, net_%d, net_%d);\n", i, faninlist[i][0], faninlist[i][1]);
                       break;
                //XNOR
                case 4:
                       fprintf(outFile, "xnor (net_%d, net_%d, net_%d);\n", i, faninlist[i][0], faninlist[i][1]); 
                       break;
                //DFF
                case 5:
                       break;
                //AND
                case 6:
                       fprintf(outFile, "and (net_%d, ", i);
                       for(j=0;j<fanin[i];j++)
		       {
                       if(j<fanin[i]-1)
                       fprintf(outFile, "net_%d, ",faninlist[i][j]);
		       else 
                       fprintf(outFile, "net_%d);\n",faninlist[i][j]);
		       }
                       break;
                //NAND
                case 7:
                       fprintf(outFile, "nand (net_%d, ", i);
                       for(j=0;j<fanin[i];j++)
		       {
                       if(j<fanin[i]-1)
                       fprintf(outFile, "net_%d, ",faninlist[i][j]);
		       else 
                       fprintf(outFile, "net_%d);\n",faninlist[i][j]);
		       } 
                       break;
                //OR 
                case 8:
                       fprintf(outFile, "or (net_%d, ", i);
                       for(j=0;j<fanin[i];j++)
		       {
                       if(j<fanin[i]-1)
                       fprintf(outFile, "net_%d, ",faninlist[i][j]);
		       else 
                       fprintf(outFile, "net_%d);\n",faninlist[i][j]);
		       } 
                       break;
                //NOR
                case 9:
                       fprintf(outFile, "nor (net_%d, ", i);
                       for(j=0;j<fanin[i];j++)
		       {
                       if(j<fanin[i]-1)
                       fprintf(outFile, "net_%d, ",faninlist[i][j]);
		       else 
                       fprintf(outFile, "net_%d);\n",faninlist[i][j]);
		       } 
                       break;
                //NOT
                case 10:
                       fprintf(outFile, "not (net_%d, net_%d);\n", i, faninlist[i][0]); 
                       break;
                //BUF
		case 11: 
                       fprintf(outFile, "buf (net_%d, net_%d);\n", i, faninlist[i][0]); 
                       break;
		default: 
                       break;
		}

	}

        fprintf(outFile, "\nendmodule");
        fclose(outFile);


}

////////////////////////////////////////////////////////////////////////
// other member functions here
////////////////////////////////////////////////////////////////////////

void circuit::add_gates_pi (int data)
  	{
	node *newlink = new node;
	newlink->data_value = data;
	newlink->link = storegates_pi;
        storegates_pi = newlink;
	}

void circuit::add_gates_po (int data)
  	{
	node *newlink = new node;
	newlink->data_value = data;
	newlink->link = storegates_po;
        storegates_po = newlink;
	}

////////////////////////////////////////////////////////////////////////
// main starts here
////////////////////////////////////////////////////////////////////////
main(int argc, char *argv[])
{
    char cktName[20];
    circuit *ckt;
    int vecWidth, poNum;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <ckt>\n", argv[0]);
        exit(-1);
    }

    strcpy(cktName, argv[1]);

    // read in the circuit and build data structure
    ckt = new circuit(cktName);

    // read in a number
   // printf("Please enter a primary input number:\n");
   // scanf("%d", &poNum);

    // NOW ADD YOUR FUNCTION HERE
}


