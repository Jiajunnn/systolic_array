// #ifndef __INPUTSETUP_H__
// #define __INPUTSETUP_H__

// #include <systemc.h>
// #include <nvhls_int.h>
// #include <nvhls_connections.h>
// #include <nvhls_vector.h>

// #include <string>

// #include "../Memory/Memory.h"
// #include "../SysPE/SysPE.h"
// #include "../SysArray/SysArray.h"
// #include "../include/SysSpec.h"

// SC_MODULE(InputSetup)
// {
//  public: 
//   sc_in_clk     clk;
//   sc_in<bool>   rst;

//   typedef spec::InputType InputType;  
//   static const int N = spec::N;     // # banks = N

//   // Start with M_1 to start Sytolic array data read
//   // after receive all outputs, fire interrupt (Use counter to count last col output)

//   // I/O 
//   Connections::In<Memory::IndexType>  start;      // push input "#col-1" m_1 as start signal
//   Connections::Out<bool>              com_IRQ;    // computation IRQ
  
//   Connections::In<InputType>      act_out_vec[N];  // from SysArray
// //   Connections::Out<InputType>     act_in_vec[N];   // to SysArray
// //   Connections::Out<InputType>     add_in_vec[N];   // to SysArray

//   // Memory channels
//   Connections::Out<Memory::req_t>   rd_req; 
//   Connections::In<Memory::rsp_t>    rd_rsp; 
//   Connections::Out<Memory::req_t>   wr_req; 

//   // interconnect trigger
//   Connections::Combinational<Memory::IndexType>   start_wr;

//   SC_HAS_PROCESS(InputSetup);
//   InputSetup(sc_module_name name_) : sc_module(name_) {

//     SC_THREAD (ReadReqRun);
//     sensitive << clk.pos();
//     NVHLS_NEG_RESET_SIGNAL_IS(rst); 
    
//     SC_THREAD (ReadRspRun);
//     sensitive << clk.pos();
//     NVHLS_NEG_RESET_SIGNAL_IS(rst);

//     SC_THREAD (WriteReqRun);
//     sensitive << clk.pos();
//     NVHLS_NEG_RESET_SIGNAL_IS(rst);
  
//   }

//   // The main process that generate read req that matches
//   // systolic array data setup
//   void ReadReqRun() {
//     start.Reset();
//     rd_req.Reset();   
//     start_wr.ResetWrite();

//     //#pragma hls_pipeline_init_interval 1
//     while(1) {
//       Memory::IndexType M_1;
//       M_1 = start.Pop();
//       //cout << "compute M_1 " << M_1 << endl; 
//       start_wr.Push(M_1);
//       // Now we want to generate sequencial read that 
//       // matches SysArray setup
//       // 1, 2, 3 ...  N-1, N ... N, N-1 .. 3, 2, 1
       
//       //IndexType ctr[N];
//       // Must explicitly initialize
//       //#pragma hls_unroll yes
//       //for (int i = 0; i < N; i++) {
//       //  ctr[i] = 0;
//       //}
//       Memory::req_t req_reg;
//       req_reg.type.val = CLITYPE_T::LOAD;
//       //req_reg.opcode = LOAD;

//       #pragma hls_pipeline_init_interval 1
//       //for (int i = 0; i < N+M-1; i++) { 
//       for (int i = 0; i < N+M_1; i++) { 
//         #pragma hls_unroll yes
//         for (int j = 0; j < N; j++) {
//           //req_reg.addr[j] = ctr[j]*N + j;
//           if (i >= j && (i-j) <= M_1) {
//             req_reg.valids[j] = 1;
//             req_reg.addr[j] = N*(i-j) + j;
//           }
//           else {
//             req_reg.valids[j] = 0;
//             req_reg.addr[j] = 0;
//           }
//           //if (i < N-1 && j <= i) {
//           //  req_reg.valids[j] = true;
//           //  ctr[j] += 1;
//           //}
//           //else if (i >= N-1 && i < M) {
//           //else if (i >= N-1 && i < M_1+1) {
//           //  req_reg.valids[j] = true;
//           //  ctr[j] += 1;
//           //}
//           //else if (i >= M && j > i-M) {
//           //else if (i >= M_1+1 && j > i-M_1-1) {
//           //  req_reg.valids[j] = true;
//           //  ctr[j] += 1;
//           //}
//           //else {
//           //  req_reg.valids[j] = false;
//           //}
//    	    } 
//        // cout << "@" << sc_time_stamp() << "\tm=" << i << endl;
//         rd_req.Push(req_reg);
//         wait();
//       }
//       wait();
//     }
//   }

//   // Push data read from SRAM to SysArray
//   // only push banks with valid rsp
//   void ReadRspRun() {
//     #pragma hls_unroll yes
//     for (int i = 0; i < N; i++) {
//     //   act_in_vec[i].Reset();
//       add_in_vec[i].Reset();
//     }
//     rd_rsp.Reset();

//     #pragma hls_pipeline_init_interval 1
//     while(1) {
//       Memory::rsp_t rsp_reg; 
//       if (rd_rsp.PopNB(rsp_reg)) {
//         #pragma hls_unroll yes
//         for (int i = 0; i < N; i++) {
//           if (rsp_reg.valids[i] == true) {
//             cout << "CHECK DATA ACT IN@" << sc_time_stamp() << "\t" << rsp_reg.data[i] << endl;
//             // act_in_vec[i].Push(rsp_reg.data[i]);
//             // wait();
    
//             add_in_vec[i].Push(rsp_reg.data[i]);

//             cout << "CHECK DATA ADD IN @" << sc_time_stamp() << "\t" << rsp_reg.data[i] << endl;
//             // assert (act_in_vec[i] == add_in_vec[i]);
//           }
//         }
//         // #pragma hls_unroll yes
//         // for (int i = 0; i < N; i++) {
//         //   if (rsp_reg.valids[i] == true) {
//         //     add_in_vec[i].Push(0);
//         //     cout << "CHECK DATA ADD IN @" << sc_time_stamp() << "\t" << rsp_reg.data[i] << endl;
//         //   }
//         // }
//       }
//       //copy data to add_in_vec
//       // #pragma hls_unroll yes
//       // for (int i = 0; i < N; i++) {
//       //   add_in_vec[i].Push(0);
//       // }
//       wait();
//     }
//   }
// };

// #endif

