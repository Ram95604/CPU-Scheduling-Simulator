#include<iostream>
#include<clocale>
#include<iomanip>
#include<string>
#include<fstream>
#include<sstream>
using namespace std;

class PCB;

template<typename T>
class DList{
    struct node{
        T data;
        node *next, *prev;
        explicit node(T data_):data(data_){
            next = prev = NULL;
        }
    };
    node *head, *tail;
    int _size;
public:
    DList(){
        head = tail = NULL;
        _size = 0;
    }
    ~DList(){
        clear();
    }
    void add_start(T p){
        auto temp = new DList::node(p);
        if(_size > 0) head->prev = temp;
        else tail = temp;
        temp->next = head;
        temp->prev = NULL;
        head = temp;
        _size++;
    }
    void add_end(T p){
        auto temp = new node(p);
        if(_size > 0) tail->next = temp;
        else head = temp;
        temp->next = NULL;
        temp->prev = tail;
        tail = temp;
        _size++;
    }
    T* gethead(){
        if(_size){
            return &(head->data);
        }
        return NULL;
    }
    T* gettail(){
        if(_size) return &(tail->data);
    }
    T* getindex(int index){
        if(index < 0 || index >= _size) return NULL;
        node* curr = head;
        while(curr != NULL && index > 0){
            curr = curr->next;
            index--;
        }
        if(curr != NULL) return &(curr->data);
        return NULL;
    }
    T remove_head(){
        T temp;
        if(_size == 1) {
            temp = head->data;
            clear();
        }
        else if(_size > 1) {
            temp = head->data;
            head = head->next;
            delete head->prev;
            head->prev = NULL;
            _size--;

        }
        return temp;
    }
    T remove_tail(){
        T temp;
        if(_size == 1){
            temp = tail->data;
            clear();
        }
        else if(_size > 1) {
            temp = tail->data;
            tail = tail->prev;
            delete tail->next;
            tail->next = NULL;
            _size--;
        }
        return temp;

    }
    T removeindex(int index){
        T temp;
        if(index == 0) temp=remove_head();
        else if(index == _size-1) temp= remove_tail();
        else {
            node *curr = head;
            while (curr != NULL && index > 0) {
                curr = curr->next;
                index--;
            }
            if(curr != NULL) {
                curr->prev->next = curr->next;
                curr->next->prev = curr->prev;

                PCB temp(curr->data);
                delete curr;
                _size--;
            }
        }
        return temp;
    }
    int size(){
        return _size;
    }
    void clear(){
        node *next = head;
        while(head != NULL){
            next = head->next;
            delete head;
            head = next;
        }
        tail = NULL;
        _size = 0;
    }
    void print(){
        cout << "----Back----" << endl;
        node *curr = head;
        while(curr != NULL){
            curr->data.print();
            curr = curr->next;
        }
        cout << "----Front----" <<endl;
    };
};

class Clock{
    private:
        float time;
    public:
        Clock(){
            time=0;
        }
        void reset(){
            time=0;
        }
        float gettime(){
            return time;
        }
        void step(){
            time+=0.5;
        }
};

struct PCB{
    int pid, arrival, burst, priority, num_context;
    float time_left, resp_time, wait_time, finish_time;
    bool started;

    PCB(){pid = arrival = burst = time_left = priority = resp_time = wait_time = num_context = finish_time = started = 0;}
    PCB(int id, int arr, int time, int prio) : pid(id), arrival(arr), burst(time), time_left(time), priority(prio){
        resp_time = wait_time = num_context = finish_time = started = 0;
    }
    void print(){
        cout << pid << " " << arrival << " " << time_left << " " << priority << endl;
    }
};


class PCBGenerator{
private:
    ifstream infile;
    DList<PCB> *ready_queue;
    Clock *clock;
    PCB nextPCB;
    bool _finished;
    int last_arr;
    bool *arrivals;
    bool *pids;
    int arr_size;
public:
    PCBGenerator(string filename, DList<PCB> *lst, Clock *c){
        clock = c;
        ready_queue = lst;
        _finished = false;
        last_arr = 0;
        arr_size = 25;
        arrivals = new bool[arr_size];
        pids = new bool[arr_size];
        for(int i = 0; i < arr_size; ++i) {
            arrivals[i] = false;
            pids[i] = false;
        }
        infile.open(filename);
        readnext();
    }
    ~PCBGenerator(){
        delete arrivals;
        delete pids;
    }

    void generate(){
        if(!_finished && clock->gettime() >= nextPCB.arrival){
            ready_queue->add_end(nextPCB);
            readnext();
        }
    }

    void readnext(){
        bool error = false;
        if(!infile.eof()){
            stringstream ss;
            string line;
            float vals[5];

            while(!infile.fail()){
                getline(infile, line);
                if(line.length() <= 2) continue;
                break;
            }
            if(infile.eof()){
                _finished = true;
                return;
            }

            ss << line;
            int count = 0;
            while(count < 4 && ss >> vals[count]){
                count++;
            };
            while(vals[0] >= arr_size || vals[1] >= arr_size) doublearrays();

            if(ss.fail() && !error) if(error = true) cout << "Missing data for process in file. Exiting Now." <<    endl;
            if(ss >> vals[5] && !error) if(error = true) cout << "Too many values for a process in file. Exiting now." <<    endl;
            if(vals[1] < 0 && !error) if(error = true) cout << "Arrival time can't be less than zero. Exiting now." <<    endl;
            if(vals[2] <= 0 && !error) if(error = true) cout << "CPU Burst time must be greater than 0. Exiting now." <<    endl;
            if(vals[1] < last_arr && !error) if(error = true) cout << "File needs to be sorted by arrival time. Exiting now." <<    endl;
            if(pids[int(vals[0])]) if(error = true) cout << "Can't have duplicate PIDs. Exiting now." <<    endl;
            if(arrivals[int(vals[1])]) if(error = true) cout << "Can't have duplicate arrival times. Exiting now." <<    endl;

            if(error) return(throw 1);

            arrivals[int(vals[1])] = true;
            pids[int(vals[0])] = true;
            nextPCB = PCB(vals[0], vals[1], vals[2], vals[3]);
        }
        else _finished = true;
    }

    bool finished(){
        return _finished;
    }
    void doublearrays(){
        arr_size *= 2;
        auto temp_arrs = new bool[arr_size];
        auto temp_pids = new bool[arr_size];
        for(int i = 0; i < arr_size; ++i) {
            if(i < arr_size/2){
                temp_arrs[i] = arrivals[i];
                temp_pids[i] = pids[i];
            }
            else {
                temp_arrs[i] = false;
                temp_pids[i] = false;
            }
        }
        delete arrivals;
        delete pids;
        arrivals = temp_arrs;
        pids = temp_pids;
    }
};

class StatUpdater{
private:
    DList<PCB> *ready_queue;
    DList<PCB> *finished_queue;
    Clock *clock;
    int algorithm, num_tasks, timeq;
    float last_update;
       string filename;
public:
    StatUpdater(DList<PCB> *rq, DList<PCB> *fq, Clock *cl, int alg, string fn, int tq){
        ready_queue = rq;
        finished_queue = fq;
        clock = cl;
        algorithm = alg;
        timeq = tq;
        filename = fn;
        last_update = 0;   
    }

    void execute(){
        float increment = clock->gettime() - last_update;
        last_update = clock->gettime();
        for(int index = 0; index < ready_queue->size(); ++index){
            PCB* temp = ready_queue->getindex(index);
            temp->wait_time += increment;
        }
    }
    void print(){
        num_tasks = finished_queue->size();
        string alg;
        int colwidth = 11;
        float tot_burst, tot_turn, tot_wait, tot_resp;
        int contexts;
        tot_burst = tot_turn = tot_wait = tot_resp = contexts = 0;

        ofstream outfile(filename);


        switch(algorithm){
            case 0:
                alg = "FCFS";
                break;
            case 1:
                alg = "SRTF";
                break;
            case 2:
                alg = "Round Robin";
                break;
            case 3:
                alg = "Preemptive Priority";
                break;
        }

        outfile << "*******************************************************************" << endl;
        outfile << "Scheduling Algorithm: " << alg <<    endl;
        if(timeq != -1) outfile << "(No. Of Tasks = " << finished_queue->size() << " Quantum = " << timeq << ")" <<    endl;
        outfile << "*******************************************************************" <<    endl;

        outfile << "----------------------------------------------------------------------------------------------------------------------" <<    endl;
        outfile << "| " <<    left <<    setw(colwidth) << "PID" << "| " <<    left <<    setw(colwidth) << "Arrival"
                << "| " <<    left <<    setw(colwidth) << "CPU-Burst" << "| " <<    left <<    setw(colwidth) << "Priority"
                << "| " <<    left <<    setw(colwidth) << "Finish" << "| " <<    left <<    setw(colwidth) << "Waiting"
                << "| " <<    left <<    setw(colwidth) << "Turnaround" << "| " <<    left <<    setw(colwidth) << "Response"
                << "| " <<    left <<    setw(colwidth) << "C. Switches" << "| " <<    endl
                << "----------------------------------------------------------------------------------------------------------------------" <<    endl;

        for(int id = 1; id < num_tasks+1; ++id){
            for(int index = 0; index < finished_queue->size(); ++index){
                if(finished_queue->getindex(index)->pid == id){
                    PCB temp = finished_queue->removeindex(index);
                    float turnaround = temp.finish_time - temp.arrival;
                    tot_burst += temp.burst;
                    tot_turn += turnaround;
                    tot_wait += temp.wait_time;
                    tot_resp += temp.resp_time;
                    contexts += temp.num_context;

                    outfile << "| " <<    left <<    setw(colwidth) << temp.pid << "| " <<    left <<    setw(colwidth)
                            << temp.arrival << "| " <<    left <<    setw(colwidth) << temp.burst << "| " <<    left
                            <<    setw(colwidth) << temp.priority << "| " <<    left <<    setw(colwidth) << temp.finish_time
                            << "| " <<    left <<    setw(colwidth) << temp.wait_time << "| " <<    left <<    setw(colwidth)
                            << turnaround << "| " <<    left <<    setw(colwidth) << temp.resp_time << "| " <<    left <<    setw(colwidth)
                            << temp.num_context << "|" <<    endl;
                    outfile << "----------------------------------------------------------------------------------------------------------------------" <<    endl;
                }
            }
        }
        outfile <<    endl;
        outfile << "Average CPU Burst Time: " << tot_burst/num_tasks << " ms\t\tAverage Waiting Time: " << tot_wait/num_tasks << " ms" <<    endl
                << "Average Turnaround Time: " << tot_turn/num_tasks << " ms\t\tAverage Response Time: " << tot_resp/num_tasks << " ms" <<    endl
                << "Total No. of Context Switching Performed: " << contexts <<    endl;
        }
};


class CPU{
private:
    PCB *pcb;
    bool idle;
    Clock *clock;
    DList<PCB> *finished_queue; 
public:

    CPU(DList<PCB> *fq, Clock *cl){
        pcb = NULL;
        idle = true;
        finished_queue = fq;
        clock = cl;
    }
    
    PCB* getpcb(){
        return pcb;
    }

    void setpcb(PCB *new_pcb) {
        pcb = new_pcb;
    }

    bool isidle(){
        return idle;
    }
    void execute(){
        if(pcb != NULL){
            idle = false;
            if(!pcb->started){ 
                pcb->started = true;
                pcb->resp_time = clock->gettime() - pcb->arrival;
            }
            pcb->time_left -= .5; 
            if(pcb->time_left <= 0) { 
                terminate();
                idle = true;
            }

        }
    }
    void terminate(){
        pcb->finish_time = clock->gettime()+.5;
        finished_queue->add_end(*pcb);
        delete pcb;
        pcb = NULL;
    }
};
class Dispatcher;
class Scheduler{
    private:
    int next_pcb_index;
    DList<PCB> *ready_queue;
    CPU *cpu;
    Dispatcher *dispatcher;
    int algorithm;
    float timeq, timer;
public:
    Scheduler();
    Scheduler(DList<PCB> *rq, CPU *cp, int alg);
    Scheduler(DList<PCB> *rq, CPU *cp, int alg, int tq);
    void setdispatcher(Dispatcher *disp);
    int getnext();
    void execute();
    void fcfs();
    void srtf();
    void rr();
    void pp();
};
class Dispatcher{
private:
    CPU *cpu;
    Scheduler *scheduler;
    DList<PCB> *ready_queue;
    Clock *clock;
    bool _interrupt;
public:
    Dispatcher(){
        cpu = NULL;
        scheduler = NULL;
        ready_queue = NULL;
        clock = NULL;
        _interrupt = false;

    }
    Dispatcher(CPU *cp, Scheduler *sch, DList<PCB> *rq, Clock *cl){
        cpu = cp;
        scheduler = sch;
        ready_queue = rq;
        clock = cl;
        _interrupt = false;
    }
    PCB* switchcontext(int index){
        int nextIndex=scheduler->getnext();
        PCB* old_pcb = cpu->getpcb();
        PCB* new_pcb = new PCB(ready_queue->removeindex(nextIndex));
        cpu->setpcb(new_pcb);
        return old_pcb;
    }
    void execute(){
        if(_interrupt) {
            PCB* old_pcb = switchcontext(scheduler->getnext());
            if(old_pcb != NULL){
                old_pcb->num_context++;
                PCB* cpu_pcb = cpu->getpcb();
                cpu->getpcb()->wait_time += .5;
                clock->step();
                ready_queue->add_end(*old_pcb);
                delete old_pcb;
            }
            _interrupt = false;
        }
    }
    void setscheduler(Scheduler *sch) {
        scheduler = sch;
    }
    void interrupt(){
        _interrupt = true;
    }
};
    Scheduler::Scheduler(){
        next_pcb_index = -1;
        ready_queue = NULL;
    }
    Scheduler::Scheduler(DList<PCB> *rq, CPU *cp, int alg){
        ready_queue = rq;
        cpu = cp;
        dispatcher = NULL;
        next_pcb_index = -1;
        algorithm = alg;
    }
    Scheduler::Scheduler(DList<PCB> *rq, CPU *cp, int alg, int tq){
        ready_queue = rq;
        cpu = cp;
        dispatcher = NULL;
        next_pcb_index = -1;
        algorithm = alg;
        timeq = timer = tq;
    }
    void Scheduler::setdispatcher(Dispatcher *disp){
        dispatcher = disp;

    }
    int Scheduler::getnext(){
        return next_pcb_index;

    }
    void Scheduler:: execute(){
        if(timer > 0) timer -= .5;
        if(ready_queue->size()) {
            switch (algorithm) {
                case 0:
                    fcfs();
                    break;
                case 1:
                    srtf();
                    break;
                case 2:
                    rr();
                    break;
                case 3:
                    pp();
                    break;
                default:
                    break;
            }
        }
    }
    void Scheduler::fcfs(){
        next_pcb_index = 0;
        if(cpu->isidle()){
            dispatcher->interrupt();
        }

    }
    void Scheduler::srtf(){
        float short_time;
        int short_index = -1;

        if(!cpu->isidle()) short_time = cpu->getpcb()->time_left;
        else {
            short_time = ready_queue->gethead()->time_left;
            short_index = 0;
        }

        for(int index = 0; index < ready_queue->size(); ++index){
            if(ready_queue->getindex(index)->time_left < short_time){ 
                short_index = index;
                short_time = ready_queue->getindex(index)->time_left;
            }
        }

        if(short_index >= 0) {
            next_pcb_index = short_index;
            dispatcher->interrupt();
        }
    }
    void Scheduler::rr(){
        if(cpu->isidle() || timer <= 0){
            timer = timeq;
            next_pcb_index = 0;
            dispatcher->interrupt();
        }
    }
    void Scheduler::pp(){
        int low_prio;
        int low_index = -1;

        if(!cpu->isidle()) low_prio = cpu->getpcb()->priority;
        else{
            low_prio = ready_queue->gethead()->priority;
            low_index = 0;
        }

        for(int index = 0; index < ready_queue->size(); ++index){
            int temp_prio = ready_queue->getindex(index)->priority;
            if(temp_prio < low_prio){ 
                low_prio = temp_prio;
                low_index = index;
            }
        }

        if(low_index >= 0){
            next_pcb_index = low_index;
            dispatcher->interrupt();
        }
    }




int main(int argc, char* argv[]) {
    if(argc < 4){
        cout << "Not enough arguments sent to main." << endl;
        cout << "Format should be: ./lab2 inputfile outputfile algorithm timequantum(if algorithm == 2)" << endl;
        return EXIT_FAILURE;
    }
    if(atoi(argv[3]) == 2 && argc == 4){
        cout << "Need to provide time quantum when using Round Robin algorithm" << endl;
        return EXIT_FAILURE;
    }
    int algorithm = atoi(argv[3]);
    int timeq = -1;
    if(algorithm == 2) timeq = atoi(argv[4]);

    auto ready_queue = new DList<PCB>();
    auto finished_queue = new DList<PCB>();

    try {
        Clock clock;
        PCBGenerator pgen(argv[1], ready_queue, &clock);
        StatUpdater stats(ready_queue, finished_queue, &clock, algorithm, argv[2], timeq);
        CPU cpu(finished_queue, &clock);
        Scheduler scheduler(ready_queue, &cpu, algorithm, timeq);
        Dispatcher dispatcher(&cpu, &scheduler, ready_queue, &clock);
        scheduler.setdispatcher(&dispatcher);
        


        while (!pgen.finished() || ready_queue->size() || !cpu.isidle()) {
            pgen.generate();
            scheduler.execute();
            dispatcher.execute();
            cpu.execute();
            stats.execute();
            clock.step();
        }

        stats.print();

    }
    catch(int){
        delete ready_queue;
        delete finished_queue;
        return EXIT_FAILURE;
    }

    return 0;
}