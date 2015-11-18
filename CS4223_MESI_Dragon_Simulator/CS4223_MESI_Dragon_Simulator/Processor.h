//
//  Processor.h
//  CS4223_MESI_Dragon_Simulator
//
//  Created by Roastedbill on 16/11/15.
//  Copyright © 2015 Roastedbill. All rights reserved.
//

#ifndef Processor_h
#define Processor_h

#include "CS4223.h"

using namespace std;
enum Bus_signal {BusRd, BusRdX, BusUpd, Flush, FlushMem, None};
class Processor{
  public:
    bool done;
    unsigned int p_index;
    string protocol;
    string input_file;
    unsigned int cache_size;
    unsigned int associativity;
    unsigned int block_size;
    
    Processor(unsigned int p_index, string protocol, string input_file, unsigned int cache_size, unsigned int associativity, unsigned int block_size){
        this->done = true;
        this->p_index = p_index;
        this->protocol = protocol;
        this->input_file = input_file;
        this->cache_size = cache_size;
        this->associativity = associativity;
        this->block_size = block_size;
    }
    
    pair<Bus_signal,string> next_cycle(unsigned long cycle, bool is_last_finished){
        return make_pair(None, "111");
    }
    
    bool is_data_cached(string addr){
        return false;
    }
    
    Bus_signal MESIUpdate(Bus_signal s, string mem_addr){
        return None;
    }
    
    Bus_signal MESIFeedBack(string mem_addr, Bus_signal signal, bool is_shared){
        return None;
    }
    
    Bus_signal DRAGONUpdate(Bus_signal s, string mem_addr){
        return None;
    }
    
    Bus_signal DRAGONFeedBack(string mem_addr, Bus_signal signal, bool is_shared){
        return None;
    }
    
    float get_cache_miss_ratio(){
        return 1;
    }
    
    unsigned int get_total_cache_hit(){
        return 1;
    }
    
    unsigned int get_total_cache_access(){
        return 1;
    }
    
    unsigned int get_total_cycles(){
        return 1;
    }
    
};


#endif /* Processor_h */
