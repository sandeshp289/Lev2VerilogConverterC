# Lev2VerilogConverterC

Description:

    Lev to Verilog file converter in C/C++

Input Files:

    lev2vlog_combo.c               : C/C++ code (for combinational circuits)

    lev2vlog_seq.c                 : C/C++ code (for sequential circuits)

    c17.lev                        : Input circuit file (for e.g. combinational circuit c17) in lev format
    
    s27.lev                        : Input circuit file (for e.g. sequential circuit s27) in lev format

Output Files:

    c17.v                          : Output Verilog Netlist (for e.g. c17)

    s27.v                          : Output Verilog Netlist (for e.g. s27)
    
Compilation:

    g++ lev2vlog_combo.c
    
    g++ lev2vlog_seq.c

Execution:

    ./a.out c17

    ./a.out s27
