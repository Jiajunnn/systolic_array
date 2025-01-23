#ifndef __MEMORYCORE_H__
#define __MEMORYCORE_H__

#include <systemc.h>
#include <nvhls_int.h>
#include <nvhls_connections.h>
#include <nvhls_vector.h>

#include "ArbitratedScratchpad.h"
#include <ArbitratedScratchpad/ArbitratedScratchpadTypes.h>

#include <string>
#include <one_hot_to_bin.h>

#include "../SysPE/SysPE.h"
#include "../SysArray/SysArray.h"
#include "../include/SysSpec.h"

//#include <scratchpad.h>
//#include <scratchpad/scratchpadtypes.h>

SC_MODULE(MemoryCore)
{
 public: 
  sc_in_clk     clk;
  sc_in<bool>   rst;

  // static const int addr_width = nvhls::nbits<CapacityInBytes - 1>::val;
  // static const int log2_nbanks = nvhls::nbits<NumBanks - 1>::val;

  typedef spec::InputType InputType;  
  static const int N = spec::N;               // # banks = N
  //static const int N = 2; //spec::N;               // # banks = N
  static const int Entries = spec::Entries;   // # of entries per bank
  static const int Capacity = N*Entries;

  static const int addr_width = nvhls::nbits<Capacity - 1>::val;
  static const int log2_nbanks = nvhls::nbits<N - 1>::val;


  // Memory type
  typedef ArbitratedScratchpad<InputType, Capacity, N, N, 0> MemType;

  static const int IndexBits = nvhls::nbits<Entries-1>::val;

  typedef NVUINTW(IndexBits) IndexType;
  //typedef typename nvhls::nv_scvector <InputType, N> VectorType;
  typedef NVUINTW(MemType::addr_width) AddrType;
  
  typedef MemType::req_t req_t;
  typedef MemType::rsp_t rsp_t;  
   
  MemType mem_inst;
  
  Connections::In<req_t> req_inter;
  Connections::Out<rsp_t> rsp_inter;
 
  
  SC_HAS_PROCESS(MemoryCore);
  MemoryCore(sc_module_name name_) : sc_module(name_) {

    SC_THREAD (MemoryRun);
    sensitive << clk.pos();
    NVHLS_NEG_RESET_SIGNAL_IS(rst); 
  }

  void MemoryRun() {
    req_inter.Reset();
    rsp_inter.Reset();
    
    #pragma hls_pipeline_init_interval 1
    while(1) {
      
      req_t req_reg;
      rsp_t rsp_reg;
      bool input_ready[N];
      bool is_read = 0;     
      bool is_write = 0; 
      if (req_inter.PopNB(req_reg)) {
        is_read = (req_reg.type.val == CLITYPE_T::LOAD);
        is_write = (req_reg.type.val == CLITYPE_T::STORE);
        // for (int i = 0; i < N; i++){
        //   cout <<"Before: " << input_ready[i] << endl;
        // }
        // // Print req_reg contents
        // cout << "Request Contents:" << endl;
        // cout << "  Input Ready: ";
        // for (int i = 0; i < N; i++) {
        //     cout << input_ready[i] << " ";
        // }
        // cout << "  Type: " << (req_reg.type.val == CLITYPE_T::LOAD ? "LOAD" : "STORE") << endl;
        // cout << "  Valid Flags: ";
        // for (int i = 0; i < N; i++) {
        //     cout << req_reg.valids[i] << " ";
        // }
        // cout << endl;
        // cout << "  Addresses:   ";
        // for (int i = 0; i < N; i++) {
        //     cout << req_reg.addr[i] << " ";
        // }
        // cout << endl;
        // cout << "  Data: ";
        // for (int i = 0; i < N; i++) {
        //     cout << req_reg.data[i] << " ";
        // }
        // cout << endl;


        mem_inst.load_store(req_reg, rsp_reg, input_ready);
        // for (int i = 0; i < N; i++){
        //   cout << "After: " << input_ready[i] << endl;
        // }

        // // Print rsp_reg contents
        // cout << "Response Contents:" << endl;
        // cout << "  Input Ready: ";
        // for (int i = 0; i < N; i++) {
        //     cout << input_ready[i] << " ";
        // }
        // cout << "  Valid Flags: ";
        // for (int i = 0; i < N; i++) {
        //     cout << rsp_reg.valids[i] << " ";
        // }
        // cout << endl;
        // cout << "  Data: ";
        // for (int i = 0; i < N; i++) {
        //     cout << rsp_reg.data[i] << " ";
        // }
        // cout << endl;


      
        if (is_read) {
          rsp_inter.Push(rsp_reg);
          for (int i = 0; i < N; i++){
            if (req_reg.valids[i]){
              int bank_sel = nvhls::get_slc<log2_nbanks>(req_reg.addr[i], 0);
              int addr_sel = nvhls::get_slc<addr_width - log2_nbanks>(req_reg.addr[i], log2_nbanks);
              cout << "Gainsight Read at Bank " << bank_sel << " at address " << addr_sel << " at time stamp: " << sc_time_stamp() << " with data " << rsp_reg.data[i] << endl;
              // printf("Bank and input sizes are both %d\n",N);
              // cout <<"Bank and input sizes: "<< N << endl;
            }
          }
        }
        if (is_write) {
          for (int i = 0; i < N; i++){
            if (req_reg.valids[i]){
              int bank_sel = nvhls::get_slc<log2_nbanks>(req_reg.addr[i], 0);
              int addr_sel = nvhls::get_slc<addr_width - log2_nbanks>(req_reg.addr[i], log2_nbanks);
              cout << "Gainsight Write at Bank " << bank_sel << " at address "  << addr_sel << " at time stamp: " << sc_time_stamp() << " with data " << req_reg.data[i] << endl;
            }
          }  
        }
      }
      wait();
    }
  }
};

#endif

