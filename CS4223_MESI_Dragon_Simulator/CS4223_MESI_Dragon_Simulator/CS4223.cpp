//
//  CS4223.cpp
//  CS4223_MESI_Dragon_Simulator
//
//  Created by Roastedbill on 9/10/15.
//  Copyright © 2015 Roastedbill. All rights reserved.
//

#include "CS4223.h"

Controller::Controller(const int argc, char* argv[]){
    cout << "Reading configurations" << endl;
    if (argc != 7) {
        cout << "7 parameters are needed, please follow this way:" << endl;
        cout << "CS4223.cpp <protocol> <input_file> <no_processors> <cache_size> <associativity> <block_size>" << endl;
        exit(0);
    }
    this->protocol = argv[1];
    this->input_file = argv[2];
    this->no_processors = atoi(argv[3]);
    this->cache_size = atoi(argv[4]);
    this->associativity = atoi(argv[5]);
    this->block_size = atoi(argv[6]);
    
    //Create the output filename
    this->output_file += protocol;
    this->output_file += "_";
    this->output_file += input_file;
    this->output_file += "_";
    this->output_file += no_processors;
    this->output_file += "_";
    this->output_file += cache_size;
    this->output_file += "_";
    this->output_file += associativity;
    this->output_file += "_";
    this->output_file += block_size;
    this->output_file += ".txt";
        
    // Verification & Assignment
    transform(this->protocol.begin(), this->protocol.end(), this->protocol.begin(), ::toupper);
    if(this->protocol != "MESI" && this->protocol != "DRAGON"){
        cout << "Protocol not supported, exit!" << endl;
        exit(0);
    }
    
    transform(this->input_file.begin(), this->input_file.end(), this->input_file.begin(), ::toupper);
    if(this->input_file!="WEATHER" && this->input_file!="FFT"){
        cout << "Input file not supported, exit!" << endl;
        exit(0);
    }
        
    if(this->no_processors>4 ||  this->no_processors<1){
        cout << "Number of Cores must in range 1-4, exit!" << endl;
        exit(0);
    }
        
    if(!isPowerOf2(this->cache_size)){
        cout << "Cache size must be power of 2, exit!" << endl;
        exit(0);
    }
        
    if(!isPowerOf2(this->block_size)){
        cout << "Block size must be power of 2, exit!" << endl;
        exit(0);
    }
}
    
//Controller::~Controller(){
//    cout << "Hope you enjoy using our system. Bye!" << endl;
//}

bool Controller::isPowerOf2(unsigned int n){
    return ((n & (n - 1)) == 0 && n);
}

bool Controller::isAllDone(){
    bool done = 1;
    for(int i=0; i<this->no_processors; i++){
        done &= this->processors[i].done;
    }
    return done;
}

bool Controller::isDataShared(unsigned int target_p_index, string mem_addr){
    bool shared = 0;
    for(int i=0; i<this->no_processors; i++){
        if(target_p_index != i){
            shared |= this->processors[i].is_data_cached(mem_addr);
        }
    }
    return shared;
}

void Controller::busHang(unsigned int delay_cycle){
    this->bus_busy_cycle+=delay_cycle;
}
    
void Controller::initialize(){
    cout << "Initializing" << endl;
    // Controller setup
    this->bus_busy_cycle = 0;
    this->cycle = 0;
    this->total_bus_traffic_mem = 0;
    this->total_bus_traffic_cache = 0;
    this->is_instr_prolonged = false;
    // Processor setup
    this->processors = vector<Processor::Processor>();
    this->held_transactions = queue<pair<unsigned int,pair<Bus_signal, string>>>();
    this->last_finished = bitset<MAX_CORE_NUM>();
    this->last_finished.set();
    for(int i=0; i<no_processors; i++){
        this->processors.push_back(Processor::Processor(i,this->protocol,this->input_file,this->cache_size,this->associativity,this->block_size));
    }
    if(this->protocol == "MESI"){
        this->cache_update_size = this->block_size;
    }else{
        this->cache_update_size = WORD_SIZE;
    }
}
    
void Controller::run(){
    cout  << "Begin execution" << endl;
    while(!isAllDone()){
        this->cycle++;
        if(this->bus_busy_cycle > 0){
            this->bus_busy_cycle--;
            // if last fetch finishes
            if(bus_busy_cycle == 0){
                this->last_finished.set(bus_owner.first);
                if(this->protocol == "MESI" && !is_instr_prolonged){
                    for(int i=0; i<this->no_processors; i++){
                        if(i != bus_owner.first){
                            // MESIUpdate(unsigned int target_p_index, Bus_signal signal, string mem_addr)
                            if(Controller::MESIUpdate(i, bus_owner.second.first, bus_owner.second.second) == FlushMem){
                                // Flush to mem needed
                                bus_busy_cycle += MEM_ACCESS_CYCLE - this->block_size/WORD_SIZE;
                                is_instr_prolonged = true;
                            }
                        }else{ // feedback to itself
                            // Controller::MESIFeedBack(unsigned int p_index, string mem_addr, bool is_shared)
                            if(Controller::MESIFeedBack(i, bus_owner.second.second, bus_owner.second.first, isDataShared(bus_owner.first, bus_owner.second.second)) == FlushMem){
                                bus_busy_cycle += MEM_ACCESS_CYCLE;
                                is_instr_prolonged = true;
                            }
                        }
                    }
                }else if(this->protocol == "DRAGON" && !is_instr_prolonged){
                    for(int i=0; i<this->no_processors; i++){
                        if(i != bus_owner.first){
                            // DRAGONUpdate(unsigned int target_p_index, Bus_signal signal, string mem_addr)
                            if(Controller::DRAGONUpdate(i, bus_owner.second.first, bus_owner.second.second) == FlushMem){
                                bus_busy_cycle += MEM_ACCESS_CYCLE - WORD_SIZE;
                                is_instr_prolonged = true;
                            }
                        }else{ // feedback to itself
                            // Controller::DRAGONFeedBack(unsigned int p_index, string mem_addr, bool is_shared)
                            if(Controller::DRAGONFeedBack(i, bus_owner.second.second, bus_owner.second.first, isDataShared(bus_owner.first, bus_owner.second.second)) == FlushMem){
                                bus_busy_cycle += MEM_ACCESS_CYCLE;
                                is_instr_prolonged = true;
                            }
                        }
                    }
                }
                
                // if prolonged, toggle
                if(is_instr_prolonged){
                    is_instr_prolonged = false;
                }
                
                // fetch next instr if exist
                if(!this->held_transactions.empty() && !is_instr_prolonged){
                    this->bus_owner = this->held_transactions.front();
                    held_transactions.pop();
                    if((this->bus_owner.second.first ==FlushMem) || (!isDataShared(this->bus_owner.first, this->bus_owner.second.second) && (this->bus_owner.second.first == BusRd || this->bus_owner.second.first == BusRdX))){
                        busHang(MEM_ACCESS_CYCLE); // from mem
                    }else{
                        busHang(block_size/WORD_SIZE); // from other caches
                    }
                }
            }
        }
        //A single clock cycle
        for(int processor_index=0; processor_index<this->no_processors; processor_index++){
            pair<Bus_signal,string> processor_feedback = processors[processor_index].next_cycle(this->cycle,this->last_finished[processor_index]);
            // If the instr is blocked
            if(this->bus_busy_cycle > 0 && processor_feedback.first != None){
                held_transactions.push(make_pair(processor_index, processor_feedback)); // hold the transaction in queue
                this->last_finished.reset(processor_index);
            }else if(processor_feedback.first != None){ // not blocked, with stall, bus_busy_cycle=0 now
                this->bus_owner=make_pair(processor_index, processor_feedback);
                if((processor_feedback.first ==FlushMem) || (!isDataShared(processor_index, processor_feedback.second) && (processor_feedback.first == BusRd || processor_feedback.first == BusRdX))){
                    this->total_bus_traffic_mem += this->cache_update_size;
                    busHang(MEM_ACCESS_CYCLE); // from mem
                }else{
                    this->total_bus_traffic_cache += this->cache_update_size;
                    busHang(block_size/WORD_SIZE); // from other caches
                }
            }
        }
    }
}

Bus_signal Controller::MESIUpdate(unsigned int target_p_index, Bus_signal signal, string mem_addr){
    return this->processors[target_p_index].MESIUpdate(signal, mem_addr);
}

Bus_signal Controller::MESIFeedBack(unsigned int p_index, string mem_addr, Bus_signal signal, bool is_shared){
    return this->processors[p_index].MESIFeedBack(mem_addr, signal, is_shared);
}

Bus_signal Controller::DRAGONUpdate(unsigned int target_p_index, Bus_signal signal, string mem_addr){
    return this->processors[target_p_index].DRAGONUpdate(signal, mem_addr);
}

Bus_signal Controller::DRAGONFeedBack(unsigned int p_index, string mem_addr, Bus_signal signal, bool is_shared){
    return this->processors[p_index].DRAGONFeedBack(mem_addr, signal, is_shared);
}
    
void Controller::generateLogFile(){
    cout  << "Generating log file: " << this->output_file << endl;
    ofstream f_out;
    f_out.open(this->output_file);

    if(f_out.is_open()){
        f_out << "*************************************************************" << endl;
        f_out << "Total Number of Clk Cycle: " << this->cycle << endl;
        f_out << "*************************************************************" << endl;
        f_out << "Cache Miss Ratio:" << endl;
        for(int i=0; i<this->no_processors; i++){
            f_out << "Processor" << " [" << i << "]" << " Miss Ratio : " << this->processors[i].get_cache_miss_ratio() << endl;
            f_out << "Processor" << " [" << i << "]" << " Cache Hit : " << this->processors[i].get_total_cache_hit() << endl;
            f_out << "Processor" << " [" << i << "]" << " Total Access : " << this->processors[i].get_total_cache_access() << endl;
        }
        f_out << "*************************************************************" << endl;
        f_out << "Address & Data Traffic:" << endl;
        f_out << "*************************************************************" << endl;
        f_out << "Execution Cycles:" << endl;
        for(int i=0; i<this->no_processors; i++){
            f_out << "Processor Cycles" << "[" << i << "]" << ":" << this->processors[i].get_total_cycles() << endl;
        }
        f_out << "Total bus traffic between cache and memory: " << this->total_bus_traffic_mem << endl;
        f_out << "Total bus traffic between caches: " << this->total_bus_traffic_cache << endl;
        f_out << "*************************************************************" << endl;
    }
    f_out.close();
    
    cout << "*************************************************************" << endl;
    cout << "Total Number of Clk Cycle: " << this->cycle << endl;
    cout << "*************************************************************" << endl;
    cout << "Cache Miss Ratio:" << endl;
    for(int i=0; i<this->no_processors; i++){
        cout << "Processor" << " [" << i << "]" << " Miss Ratio : " << this->processors[i].get_cache_miss_ratio() << endl;
        cout << "Processor" << " [" << i << "]" << " Cache Hit : " << this->processors[i].get_total_cache_hit() << endl;
        cout << "Processor" << " [" << i << "]" << " Total Access : " << this->processors[i].get_total_cache_access() << endl;
    }
    cout << "*************************************************************" << endl;
    cout << "Address & Data Traffic:" << endl;
    cout << "*************************************************************" << endl;
    cout << "Execution Cycles:" << endl;
    for(int i=0; i<this->no_processors; i++){
        cout << "Processor Cycles" << "[" << i << "]" << ":" << this->processors[i].get_total_cycles() << endl;
    }
    cout << "Total bus traffic between cache and memory: " << this->total_bus_traffic_mem << endl;
    cout << "Total bus traffic between caches: " << this->total_bus_traffic_cache << endl;
    cout << "*************************************************************" << endl;
}


int main(int argc, char* argv[]) {
    Controller controller = Controller(argc, argv);
    controller.initialize();
    controller.run();
    controller.generateLogFile();
    getchar();
    return 0;
}
