//
//  CS4223.h
//  CS4223_MESI_Dragon_Simulator
//
//  Created by Roastedbill on 16/11/15.
//  Copyright © 2015 Roastedbill. All rights reserved.
//

#ifndef CS4223_h
#define CS4223_h

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <queue>
#include <fstream>

#include "Processor.h"

#define MEM_ACCESS_DELAY 100
#define WORD_SIZE 2
#define MEM_ACCESS_CYCLE 100
#define MAX_CORE_NUM 4

using namespace std;
    class Controller{
      public:
        string protocol;
        string input_file;
        string output_file;
        unsigned int no_processors;
        unsigned int cache_size;
        unsigned int associativity;
        unsigned int block_size;
        unsigned int bus_busy_cycle; // write data from cache to cache or fetch data from mem to cache
//        unsigned int mem_busy;
        unsigned long cycle;
        unsigned long total_bus_traffic_mem;
        unsigned long total_bus_traffic_cache;
        bool is_instr_prolonged; // if the last instr is prolonged by FlushMem
        
        // protocol dependent parameters
        unsigned int cache_update_size;
        
        // stuctures
        bitset<MAX_CORE_NUM> last_finished;
        vector<Processor> processors;
        queue<pair<unsigned int, pair<Bus_signal, string>>> held_transactions; //<index,<signal,addr>>
        pair<unsigned int,pair<Bus_signal, string>> bus_owner;
        
        Controller(const int argc,char* argv[]);
//        ~Controller();
        bool isPowerOf2(unsigned int n);
        void initialize();
        void run();
        void generateLogFile();
        
        Bus_signal MESIUpdate(unsigned int target_p_index, Bus_signal signal, string mem_addr);// return value shows if a flush to mem is necessary
        Bus_signal MESIFeedBack(unsigned int p_index, string mem_addr, Bus_signal signal, bool is_shared);
        Bus_signal DRAGONUpdate(unsigned int target_p_index, Bus_signal signal, string mem_addr);
        // if signal == BusUpd, time should be considered here!!!!
        Bus_signal DRAGONFeedBack(unsigned int p_index, string mem_addr, Bus_signal signal, bool is_shared);
        bool isDataShared(unsigned int target_p_index, string mem_addr);
        void busHang(unsigned int delay_cycle);
        bool isAllDone();
    };


#endif /* CS4223_h */
