// Tobby Lie
// Operating Systems: Lab 3
// Due May 12, 2019 @ 11:55PM
// Updated: 5/6/19 @ 4:31PM

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <queue>
#include <algorithm>
#include <sstream>

using namespace std;

// struct for process to somewhat encapsulate data of each process
struct process{
    int pid;
    int AT;
    int BT;
    int FT;
    int WT;
    int TT;
    int contextSwitch;
    bool arrived;
    bool finished;
    
    friend bool operator<(const process& a, const process& b) {
        return a.BT > b.BT;
    }
};

// function to use with sort, sorts in ascending order of AT
bool compareAT(process a, process b){
    return a.AT < b.AT;
}

// function to use with sort, sorts in ascending order of BT
bool compareBT(process a, process b){
    return a.BT < b.BT;
}

void menuLoop(); // main menu
void FCFS(string filename, vector<process> procVec); // fcfs algo
void SRTF(string filename, vector<process> procVec); // srtf algo
void RR(string filename, vector<process> procVec, int quantum); // rr algo

int main() {
     // only need to run menuLoop function in main
    menuLoop();
    
    return 0;
}

// main menu loop
void menuLoop(){
    // hold user input
    string input = "";
    // -1 is exit condition
    while(input != "-1"){
        // vector to hold processes
        vector<process> procVec;
        // input variables
        string fname, algoName, quantum, myscheduler;
        
        // clear input
        input = "";
        quantum = "";
        myscheduler = "";
        fname = "";
        algoName = "";
        
        //clear procVec
        procVec.clear();
        
        cout << "Please input in the following format <\"myscheduler\" filename algorithm (quantum for RR)> or input -1 to exit: ";
        getline(cin, input);
        // sstream variable for input
        istringstream iss(input);
        // split up input arguments
        iss >> myscheduler >> fname >> algoName >> quantum;
        
        // open infile with fname specified
        ifstream infile;
        infile.open(fname.c_str());
        
        // error if no fname input
        if(fname == "" && input != "-1"){
            cout << "Error! Did not input filename." << endl;
            continue;
        }
        // error if file did not open correctly or invalid filename
        else if(!infile && fname != "" & input != "-1"){
            cout << fname << " is an invalid filename. Error opening file!" << endl;
            continue;
        }
        
        // ints for struct variables
        int pid, arrival, burst;
        process proc;
        while(infile >> pid >> arrival >> burst){
            proc.pid = pid;
            proc.AT = arrival;
            proc.BT = burst;
            proc.FT = 0;
            proc.WT = 0;
            proc.TT = 0;
            proc.contextSwitch = 0;
            proc.arrived = false;
            proc.finished = false;
            
            // add to proc vector
            procVec.push_back(proc);
        }
        
        // close file
        infile.close();
        
        // sort procs by arrival time
        sort(procVec.begin(), procVec.end(), compareAT);
        
        // convert to upper case for algo name
        for(int i = 0; i < algoName.size(); i++){
            algoName[i] = toupper(algoName[i]);
        }
        
        // choose which algo will be implemented
        if(algoName == "FCFS"){
            FCFS(fname.c_str(), procVec);
        }
        else if(algoName == "SRTF"){
            SRTF(fname.c_str(), procVec);
        }
        else if(algoName == "RR"){
            if(quantum == ""){
                cout << "Error! Must input quantum." << endl;
                continue;
            }
            int quant = 0;
            stringstream stream(quantum);
            stream >> quant;
            RR(fname.c_str(), procVec, quant);
        }
        // exit
        else if(input == "-1"){
            continue;
        }
        // error catch for invalid input
        else{
            cout << "Invalid input! Try again." << endl;
        }
    }
}

// first come first serve algorithm
void FCFS(string filename, vector<process> procVec){
    
    cout << endl;
    cout << "************************************************************" << endl;
    cout << "************ Scheduling algorithm : FCFS *******************" << endl;
    cout << "************************************************************" << endl;
    
    int totalTime = 0;
    // vectors to hold stats for processes
    vector<int> finishVec;
    vector<int> turnaroundVec;
    vector<int> waitVec;
    int turnaround = 0;
    int wait = 0;
    // Calculate Finish, Waiting time, turn around, Note for FCFS No. of context will always be 0 for all processes
    for(int i = 0; i < procVec.size(); i++){
        // push onto queue burst times of each process one after another
        // Note: Finish = BT(previous) + BT(current)
        // Turnaround time = completion time - arrival time
        // Waiting time = turnaround time - burst time
        
        if(i == 0){
            finishVec.push_back(procVec[i].BT);
        }
        else{
            finishVec.push_back(procVec[i].BT + finishVec[i-1]);
        }
        turnaround = finishVec[i] - procVec[i].AT;
        wait = turnaround - procVec[i].BT;
        
        turnaroundVec.push_back(turnaround);
        waitVec.push_back(wait);
        
        turnaround = 0;
        wait = 0;
        
        totalTime += procVec[i].BT;
    }
    
    // calculate average values
    float aveBT = 0;
    float aveWT = 0;
    float aveTT = 0;
    // need to save value of j for average
    int j = 0;
    for(j = 0; j < finishVec.size(); j++){
        aveBT += procVec[j].BT;
        aveWT += waitVec[j];
        aveTT += turnaroundVec[j];
    }
    
    vector<int> finishVec2;
    for(int k = 0; k < procVec.size(); k++){
        procVec[k].FT = finishVec[k];
    }
    
    aveBT = aveBT/j;
    aveWT = aveWT/j;
    aveTT = aveTT/j;
    
    // print out gantt chart
    cout << endl;
    int procVecCounter = 0;
    for(int i = 0; i < totalTime; i++){
        if(finishVec[procVecCounter] == i){
            procVecCounter++;
            cout << "[[" << i << "]P" << procVecCounter + 1 << "]-";
        }
        else{
                cout << "[[" << i << "]P" << procVecCounter + 1 << "]-";
        }
        
        if(i + 1 == totalTime){
            cout << "[[" << i + 1 << "]P" << procVecCounter + 1 << "]";
        }
    }
    cout << endl;
    
    // print stats
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "|pid |arrival |CPU-burst |finish |waiting time |turn around |No. of Context |" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    
    // loop through and display all results in table
    for(int i = 0; i < procVec.size(); i++){
        cout << "|" << setw(4) << procVec[i].pid << "|" << setw(8) << procVec[i].AT << "|" << setw(10) << procVec[i].BT
        << "|" << setw(7) << finishVec[i] << "|" << setw(13) << waitVec[i] << "|" << setw(12) << turnaroundVec[i] << "|" << setw(15) << "0" << "|" << endl;
        cout << "-----------------------------------------------------------------------------" << endl;
    }
    
    // print averages
    cout << "Average CPU burst time = " << aveBT << " ms" << endl;
    cout << "Average waiting time = " << aveWT << " ms" << endl;
    cout << "Average turn around time = " << aveTT << " ms" << endl;
    // Total No. of context switching will always be 0 for FCFS
    cout << "Total No. of Context Switching Performed = 0" << endl;
    cout << endl;
}

// shortest remaining time first - preemptive
void SRTF(string filename, vector<process> procVec){
    
    cout << endl;
    cout << "************************************************************" << endl;
    cout << "************ Scheduling algorithm : SRTF *******************" << endl;
    cout << "************************************************************" << endl;
    cout << endl;
    
    int totalTime = 0;
    vector<process> arrivedVec;
    
    // Get total time of all processes
    for(int i = 0; i < procVec.size(); i++){
        totalTime += procVec[i].BT;
    }
    
    int i;
    int curProcRunning = 0;
    int lastProcRunning = 0;
    int contextSwitch = 0;
    int lastArrivedPrinted = 0;
    // loop through total time one unit of time at a time
    for(int curTime = 0; curTime < totalTime; curTime++){
        
        // check for all processes that have arrived at time curTime
        for(i = 0; i < procVec.size(); i++){
            if(procVec[i].AT == curTime){
                // add to new arrivedVec
                arrivedVec.push_back(procVec[i]);
                // break to save value of i
                break;
            }
        }
        
        // if nothing has arrived yet
        if(arrivedVec.empty()){
            continue;
        }
        
        // sort all arrived processes by BT
        sort(arrivedVec.begin(), arrivedVec.end(), compareBT);
        
        // check for context switch
        if(curProcRunning != lastProcRunning && curProcRunning != 0 && lastProcRunning != 0){
            procVec[arrivedVec[0].pid - 1].contextSwitch++;
            contextSwitch++;
        }
        
        lastProcRunning = curProcRunning;
        curProcRunning = arrivedVec[0].pid;
    
        curProcRunning = arrivedVec[0].pid;
        
        // print gantt chart as we go
        cout << "[[" << curTime << "]P" << arrivedVec[0].pid << "]-";
        if(curTime + 1 == totalTime){
            cout << "[[" << curTime + 1 << "]P" << arrivedVec[0].pid << "]";
        }
        lastArrivedPrinted = arrivedVec[0].pid;
        
        // decrement burst time
        arrivedVec[0].BT--;
        
        // if BT = 0 then we know a finish time
        if(arrivedVec[0].BT == 0){
            procVec[arrivedVec[0].pid - 1].FT = curTime + 1;
            arrivedVec.erase(arrivedVec.begin());
        }
    }
    cout << endl;
    
    // calculate turnaround and waiting time
    for(int j = 0; j < procVec.size(); j++){
        procVec[j].TT = procVec[j].FT - procVec[j].AT;
        procVec[j].WT = procVec[j].TT - procVec[j].BT;
    }
    
    // calculate average values
    float aveBT = 0;
    float aveWT = 0;
    float aveTT = 0;
    int totalContext = 0;
    // need to save value of j for average
    int j = 0;
    for(j = 0; j < procVec.size(); j++){
        aveBT += procVec[j].BT;
        aveWT += procVec[j].WT;
        aveTT += procVec[j].TT;
        totalContext += procVec[j].contextSwitch;
    }
    aveBT = aveBT/j;
    aveWT = aveWT/j;
    aveTT = aveTT/j;
    
    // print stats
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "|pid |arrival |CPU-burst |finish |waiting time |turn around |No. of Context |" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    
    // loop through and display all results in table
    for(int i = 0; i < procVec.size(); i++){
        cout << "|" << setw(4) << procVec[i].pid << "|" << setw(8) << procVec[i].AT << "|" << setw(10) << procVec[i].BT
        << "|" << setw(7) << procVec[i].FT << "|" << setw(13) << procVec[i].WT << "|" << setw(12) << procVec[i].TT << "|" << setw(15) << procVec[i].contextSwitch << "|" << endl;
        cout << "-----------------------------------------------------------------------------" << endl;
    }
    
    cout << "Average CPU burst time = " << aveBT << " ms" << endl;
    cout << "Average waiting time = " << aveWT << " ms" << endl;
    cout << "Average turn around time = " << aveTT << " ms" << endl;
    cout << "Total No. of Context Switching Performed = " << totalContext << endl;
    cout << endl;
}

// round robin with quantum
void RR(string filename, vector<process> procVec, int quantum){
    
    cout << endl;
    cout << "************************************************************" << endl;
    cout << "************ Scheduling algorithm : RR *********************" << endl;
    cout << "************************************************************" << endl;
    cout << endl;
    
    cout << "Quantum = " << quantum << endl << endl;
    
    
    int totalTime = 0;
    queue<process> readyQueue;
    
    // Get total time of all processes
    for(int i = 0; i < procVec.size(); i++){
        totalTime += procVec[i].BT;
    }
    
    int quantumCounter = 0;
    for(int curTime = 0; curTime < totalTime + 1; curTime++){
        
        // check if any NEW arrivals come in, if so, need to put onto ready queue and set that arrived process as arrived
        for(int i = 0; i < procVec.size(); i++){
            if(procVec[i].AT == curTime && procVec[i].arrived != true){
                readyQueue.push(procVec[i]);
                procVec[i].arrived = true;
            }
        }

        
        // if the queue is not empty then execute this
        if(!readyQueue.empty()){
            
            // if front bt not 0 then must check if it needs to be context switched or not
            // due to quantum counter
            if(readyQueue.front().BT != 0){
                // if quantum counter is no equal to quantum, need to reset quantum counter to 0
                // and context switch sending front of ready queue to the back
                if(quantumCounter == quantum){
                    quantumCounter = 0;
                    for(int j = 0; j < procVec.size(); j++){
                        if(procVec[j].pid == readyQueue.front().pid){
                            if(curTime != 0 && readyQueue.size() > 1){
                                procVec[j].contextSwitch++;
                            }
                        }
                    }
                    readyQueue.push(readyQueue.front());
                    readyQueue.pop();
                }
            }
            
            // if ready queue front process bt = 0 this means it is finished
            if(readyQueue.front().BT == 0){
                // if process has finished then the quantum needs to reset
                quantumCounter = 0;
                // go through all processes
                for(int j = 0; j < procVec.size(); j++){
                    // set finish time of the finished process and pop it from the ready queue, it will no longer be needed
                    if(procVec[j].pid == readyQueue.front().pid){
                        procVec[j].FT = curTime;
                        if(curTime != 0 && readyQueue.size() > 1){
                            procVec[j].contextSwitch++;
                        }
                        //cout << "Process " << procVec[j].pid << " is done at time " << curTime << endl;
                        //cout << "We are popping process " << readyQueue.front().pid << endl;
                        readyQueue.pop();
                        break;
                    }
                }
            }
            
            // decrement bt of first process in ready queue
            readyQueue.front().BT--;
            
            if(curTime < totalTime){
                if(!readyQueue.empty()){
                    cout << "[[" << curTime << "]P" << readyQueue.front().pid << "]-";
                }
                else{
                    cout << "[[" << curTime << "]P" << readyQueue.front().pid << "]-";
                }
                
                if(curTime + 1 == totalTime){
                    cout << "[[" << curTime + 1 << "]P" << readyQueue.front().pid << "]";
                }
            }
        }
        // if quantum counter = quantum then need to reset quantum counter to 0 to start over
        if(quantumCounter == quantum){
            quantumCounter = 0;
        }
        quantumCounter++;
    }
    
    cout << endl;
    
    for(int j = 0; j < procVec.size(); j++){
        procVec[j].TT = procVec[j].FT - procVec[j].AT;
        procVec[j].WT = procVec[j].TT - procVec[j].BT;
    }
    
    // calculate average values
    float aveBT = 0;
    float aveWT = 0;
    float aveTT = 0;
    int totalContext = 0;
    // need to save value of j for average
    int j = 0;
    for(j = 0; j < procVec.size(); j++){
        aveBT += procVec[j].BT;
        aveWT += procVec[j].WT;
        aveTT += procVec[j].TT;
        totalContext += procVec[j].contextSwitch;
    }
    aveBT = aveBT/j;
    aveWT = aveWT/j;
    aveTT = aveTT/j;
    
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "|pid |arrival |CPU-burst |finish |waiting time |turn around |No. of Context |" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    
    // loop through and display all results in table
    for(int i = 0; i < procVec.size(); i++){
        cout << "|" << setw(4) << procVec[i].pid << "|" << setw(8) << procVec[i].AT << "|" << setw(10) << procVec[i].BT
        << "|" << setw(7) << procVec[i].FT << "|" << setw(13) << procVec[i].WT << "|" << setw(12) << procVec[i].TT << "|" << setw(15) << procVec[i].contextSwitch << "|" << endl;
        cout << "-----------------------------------------------------------------------------" << endl;
    }
    cout << "Average CPU burst time = " << aveBT << " ms" << endl;
    cout << "Average waiting time = " << aveWT << " ms" << endl;
    cout << "Average turn around time = " << aveTT << " ms" << endl;
    cout << "Total No. of Context Switching Performed = " << totalContext << endl;
    cout << endl;
}
