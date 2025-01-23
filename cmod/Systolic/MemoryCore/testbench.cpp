#include "MemoryCore.h"
#include "../include/SysSpec.h"
#include "../include/utils.h"

#include <systemc.h>
#include <mc_scverify.h>
#include <nvhls_int.h>
#include <vector>

#define NVHLS_VERIFY_BLOCKS (MemoryCore)
#include <nvhls_verify.h>
using namespace::std;

SC_MODULE (testbench) {
  sc_clock clk;
  sc_signal<bool> rst;

  NVHLS_DESIGN(MemoryCore) mem;

//  // Memory type
//   // typedef ArbitratedScratchpad<InputType, Capacity, N, N, 0> MemType;
//   typedef MemoryCore::req_t req_t;
//   typedef MemoryCore::rsp_t rsp_t;

//   Connections::Out<req_t> req_inter;
//   Connections::In<rsp_t> rsp_inter;

  SC_HAS_PROCESS(testbench);
  testbench(sc_module_name name)
  : sc_module(name),
    clk("clk", 1, SC_NS, 0.5,0,SC_NS,true),
    rst("rst")
  {
    // mem.clk(clk);
    // mem.rst(rst);
    // mem.req_inter(req_inter);
    // mem.rsp_inter(rsp_inter);
    SC_THREAD(Run);
  }

  void Run() {
    rst = 1;
    wait(10.5, SC_NS);
    rst = 0;
    cout << "@" << sc_time_stamp() << " Asserting Reset " << endl ;
    wait(1, SC_NS);
    cout << "@" << sc_time_stamp() << " Deasserting Reset " << endl ;
    rst = 1;

    // // Write request
    // MemoryCore::req_t write_req;
    // write_req.type.val = CLITYPE_T::STORE;
    // write_req.valids[0] = true;
    // write_req.addr[0] = 5; // Address
    // write_req.data[0] = 42; // Value
    // req_inter.Push(write_req);
    // wait(1, SC_NS); // Wait for the operation to complete

    // cout << "@" << sc_time_stamp() << " Sending Read Request" << endl;

    // // Read request
    // MemoryCore::req_t read_req;
    // read_req.type.val = CLITYPE_T::LOAD;
    // read_req.valids[0] = true;
    // read_req.addr[0] = 5; // Same address as write
    // req_inter.Push(read_req);
    // wait(1, SC_NS);

    // Wait for response
    // if (rsp_inter.PopNB(rsp_reg)) {
    //   cout << "@" << sc_time_stamp() << " Received Response: " << rsp_reg.data[0] << endl;
    // }
    
    wait(1000, SC_NS);
    cout << "@" << sc_time_stamp() << " Stop " << endl ;
    sc_stop();
  }
};



int sc_main(int argc, char *argv[])
{
  nvhls::set_random_seed();
  testbench my_testbench("my_testbench");


  sc_start();
  cout << "CMODEL PASS" << endl;
  return 0;
};




// #include "MemoryCore.h"
// #include "../include/SysSpec.h"
// #include "../include/utils.h"

// #include <systemc.h>
// #include <mc_scverify.h>
// #include <nvhls_int.h>
// #include <vector>

// #define NVHLS_VERIFY_BLOCKS (MemoryCore)
// #include <nvhls_verify.h>
// using namespace::std;

// SC_MODULE (testbench) {
//   sc_clock clk;
//   sc_signal<bool> rst;

//   Connections::Combinational<MemoryCore::req_t> req_inter;
//   Connections::Combinational<MemoryCore::rsp_t> rsp_inter;

//   MemoryCore mem_inst;

//   SC_HAS_PROCESS(testbench);
//   testbench(sc_module_name name)
//   : sc_module(name),
//     clk("clk", 1, SC_NS, 0.5, 0, SC_NS, true),
//     rst("rst"),
//     mem_inst("mem_inst")
//   {
//     mem_inst.clk(clk);
//     mem_inst.rst(rst);
//     mem_inst.req_inter(req_inter);
//     mem_inst.rsp_inter(rsp_inter);

//     SC_THREAD(Run);
//     // sensitive << clk.pos();
//     async_reset_signal_is(rst, false);
//   }

//   void Run() {
//     // Reset sequence
//     rst = 1;
//     wait(10, SC_NS);
//     rst = 0;
//     wait(1, SC_NS);

//     cout << "@" << sc_time_stamp() << " Sending Write Request" << endl;

//     // Write request
//     MemoryCore::req_t write_req;
//     write_req.type.val = CLITYPE_T::STORE;
//     write_req.valids[0] = true;
//     write_req.addr[0] = 5; // Address
//     write_req.data[0] = 42; // Value
//     req_inter.Push(write_req);
//     wait(1, SC_NS); // Wait for the operation to complete

//     cout << "@" << sc_time_stamp() << " Sending Read Request" << endl;

//     // Read request
//     MemoryCore::req_t read_req;
//     read_req.type.val = CLITYPE_T::LOAD;
//     read_req.valids[0] = true;
//     read_req.addr[0] = 5; // Same address as write
//     req_inter.Push(read_req);
//     wait(1, SC_NS);

//     // Wait for response
//     MemoryCore::rsp_t rsp_reg;
//     if (rsp_inter.PopNB(rsp_reg)) {
//       cout << "@" << sc_time_stamp() << " Received Response: " << rsp_reg.data[0] << endl;
//     }

//     wait(100, SC_NS);
//     sc_stop();
//   }

// };

// int sc_main(int argc, char *argv[])
// {
//   nvhls::set_random_seed();
//   testbench my_testbench("my_testbench");

//   sc_start();
//   cout << "CMODEL PASS" << endl;
//   return 0;
// };
