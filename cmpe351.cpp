/* This is code developed by <Hafeez Ahmed> */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

// Details of Process
struct Process
{
    int id;
    int burst_time;
    int arrival_time;
    int priority;
    int waiting_time;
    int turnaround_time;
    int remaining_time;

    Process *next;
};

// Function that will parse arguments from command lne

void ParseArguments(int argc, char *argv[], int &time_quantum, string &input_File, string &output_File)
{
    int opt;
    while ((opt = getopt(argc,argv, "t:f:o:")) != -1)
    {
       switch (opt)
       {
       case 't':
        time_quantum = atoi(optarg);
        break;

       case 'f':
        input_File = optarg;
        break;

       case 'o':
        output_File = optarg;
        break;  
       
       default:
        cerr<<"usage: "<<argv[0]<<"-t time_quantum -f input_file -o output_file"<<endl;
        exit(EXIT_FAILURE);
       }
        
    }
    
}

//For loading data from input file
    void Read_File(const string &input_File, Process *&head)
    {
        ifstream insideFile(input_File);
        if(!insideFile)
        {
            cerr<<"Error Opening input File"<<endl;
            exit(EXIT_FAILURE);
        }

        int burst_time, arrival_time, priority;
        int id = 0;
        Process *current = nullptr;
        string line;

        while (getline(insideFile,line))
        {
            stringstream ss(line);
            char colon;
            if(ss>>burst_time>>colon>>arrival_time>>colon>>priority)
            {
                Process *newProcess = new Process();
                newProcess ->id = id++;
                newProcess ->burst_time = burst_time;
                newProcess->remaining_time = burst_time;
                newProcess ->arrival_time = arrival_time;
                newProcess ->priority = priority;
                newProcess ->waiting_time = 0;
                newProcess ->turnaround_time = 0;
                newProcess ->next = nullptr;

            if(head == nullptr)
            {
                head = newProcess;
                current = head;
            }
            else
            {
                current->next = newProcess;
                current = newProcess;

            }

            }
            else
            {
                cerr<<"the format is wrong in input file make sure to seperate it with :"<<line<<endl;
            }
            
        }
        insideFile.close();
        

    }
    //To Clean memory of linked lists
    void memoryCleanup(Process *&head)
    {
        while(head != nullptr)
        {
            Process *temp = head;
            head = head->next;
            delete temp;
        }
    }


    void FCFS(Process *head, ofstream &outputfile)
    {
        Process *fcfs = head;
        int currentTime = 0;
        int processCounter = 0;
        int total_waitingtime = 0;

        outputfile<<"1";
        cout<<"1";

        while(fcfs != nullptr)
        {
            fcfs->waiting_time = currentTime - fcfs->arrival_time;
            currentTime = currentTime+fcfs->burst_time;
            fcfs->turnaround_time = fcfs->waiting_time+fcfs->burst_time;

            outputfile<<":"<<fcfs->waiting_time;
            cout<<":"<<fcfs->waiting_time;
            total_waitingtime = total_waitingtime+fcfs->waiting_time;

            processCounter++;
            fcfs=fcfs->next;
        }

        double averageWaiting_time = (double)total_waitingtime/processCounter;
        averageWaiting_time = (int)(averageWaiting_time *10 + 0.5)/10.0; // Round to 1 decimal place

        outputfile<<":"<<averageWaiting_time<<endl;
        cout<<":"<<averageWaiting_time<<endl;
    }

    

    void ShortestJobFirst_NonPreemptive(Process *head, ofstream &outputFile)
{
    Process *SJF_Current = head;
    Process *SJF_Selected = nullptr;

    int current_Time = 0;
    int completed = 0;
    int total_waitingTime = 0;
    int processCounter = 0;

    //Reading the Process numbers
    while (SJF_Current != nullptr)
    {
        processCounter++;
        SJF_Current = SJF_Current->next;
    }
    outputFile << "2";
    cout<<"2";

    while (completed < processCounter)
    {
        SJF_Current = head;
        SJF_Selected = nullptr;

        
        while (SJF_Current != nullptr)
        {
            if (SJF_Current->arrival_time <= current_Time && SJF_Current->burst_time > 0)
            {
                if (SJF_Selected == nullptr || SJF_Current->burst_time < SJF_Selected->burst_time)
                {
                    SJF_Selected = SJF_Current;
                }
            }
            SJF_Current = SJF_Current->next;
        }

        if (SJF_Selected == nullptr)
        {
            current_Time++;
            continue;
        }

        
        SJF_Selected->waiting_time = current_Time - SJF_Selected->arrival_time;
        current_Time = current_Time + SJF_Selected->burst_time;
        SJF_Selected->turnaround_time = SJF_Selected->waiting_time + SJF_Selected->burst_time;
        total_waitingTime += SJF_Selected->waiting_time;
        SJF_Selected->burst_time = 0;
        completed++;
    }

    // Output in order
    SJF_Current = head;
    while (SJF_Current != nullptr)
    {
        outputFile << ":" << SJF_Current->waiting_time;
        cout<<":"<<SJF_Current->waiting_time;
        SJF_Current = SJF_Current->next;
    }

    
    double average_WaitingTime = (double)total_waitingTime / processCounter;
    average_WaitingTime = (int)(average_WaitingTime *10 + 0.5)/10.0; // Round to 1 decimal place

    outputFile << ":" << average_WaitingTime << endl;
    cout<<":"<<average_WaitingTime<<endl;
}

void ShortestJobFirst_Preemptive(Process *head, ofstream &outputFile)
{
    int currentTime = 0;
    int total_waitingTime = 0;
    int processCounter = 0;
    int completed = 0;

    //Reading the Process numbers
    Process *current = head;
    while (current != nullptr)
    {
        
        processCounter++;
        current->remaining_time = current->burst_time;
        current = current->next;
    }
    while(completed<processCounter)
    {
         Process *shortest = nullptr;
        current = head;
        
        
        while (current != nullptr)
        {
            if (current->arrival_time <= currentTime && current->remaining_time > 0)
            {
                if (shortest == nullptr || current->remaining_time < shortest->remaining_time)
                {
                    shortest = current;
                }
            }
            current = current->next;
        }

        if (shortest == nullptr)
        {
            
            currentTime++;
            continue;
        }

        shortest->remaining_time--;
        currentTime++;

        if(shortest->remaining_time == 0)
        {
            completed++;
            shortest->turnaround_time = currentTime - shortest->arrival_time;
            shortest->waiting_time = shortest->turnaround_time - shortest->burst_time;
            total_waitingTime += shortest->waiting_time;
        }

    }

    outputFile << "3";
    cout << "3";

    current = head;
     while(current!=nullptr)
     {
        cout<<":"<<current->waiting_time;
        outputFile<<":"<<current->waiting_time;
        current = current->next;
     }
    double averageWaitingTime = (double)total_waitingTime / processCounter;
    averageWaitingTime = (int)(averageWaitingTime *10 + 0.5)/10.0;

    outputFile << ":" << averageWaitingTime << endl;
    cout<<":"<<averageWaitingTime<<endl;
}

    void PriorityScheduling_NonPreemp(Process *head, ofstream &outputFile)
    {
        Process *priorityCurrent = head;
        Process *prioritySelected = nullptr;

        int currentTime = 0;
        int completed = 0;
        int totalWaitingTime = 0;
        int processCounter = 0;

        //Reading the Process numbers
        while (priorityCurrent != nullptr)
        {
            processCounter++;
            priorityCurrent = priorityCurrent->next;
        }

        outputFile << "4";
        cout << "4";

        while (completed < processCounter)
        {
            priorityCurrent = head;
            prioritySelected = nullptr;

            while (priorityCurrent != nullptr)
            {
                if (priorityCurrent->arrival_time <= currentTime && priorityCurrent->burst_time > 0)
                {
                    if (prioritySelected == nullptr || priorityCurrent->priority < prioritySelected->priority)
                    {
                        prioritySelected = priorityCurrent;
                    }
                }
                priorityCurrent = priorityCurrent->next;
            }

            if (prioritySelected == nullptr)
            {
                currentTime++;
                continue;
            }
            prioritySelected->waiting_time = currentTime - prioritySelected->arrival_time;
            currentTime += prioritySelected->burst_time;
            prioritySelected->turnaround_time = prioritySelected->waiting_time + prioritySelected->burst_time;
            totalWaitingTime += prioritySelected->waiting_time;

            prioritySelected->burst_time = 0;
            completed++;

        }

        priorityCurrent = head;
        while (priorityCurrent != nullptr)
        {
            outputFile << ":" << priorityCurrent->waiting_time;
            cout << ":" << priorityCurrent->waiting_time;
            priorityCurrent = priorityCurrent->next;
        }

        double averageWaitingTime = (double)totalWaitingTime / processCounter;
        averageWaitingTime = (int)(averageWaitingTime *10 + 0.5)/10.0; // Round to 1 decimal place

        outputFile << ":" << averageWaitingTime << endl;
        cout << ":" << averageWaitingTime << endl;

    }

    void PriorityScheduling_Preemptive(Process *head, ofstream &outputFile)
{
    int currentTime = 0;
    int totalWaitingTime = 0;
    int processCounter = 0;
    int completed = 0;

    //Reading the Process numbers
    Process *current = head;
    while (current != nullptr)
    {
        processCounter++;
        current->remaining_time = current->burst_time;
        current = current->next;
    }

    
    outputFile << "5";
    cout << "5";

    while (completed < processCounter)
    {
        Process *prioritySelected = nullptr;
        current = head;

        
        while (current != nullptr)
        {
            if (current->arrival_time <= currentTime && current->remaining_time > 0)
            {
                if (prioritySelected == nullptr || current->priority < prioritySelected->priority)
                {
                    prioritySelected = current;
                }
            }
            current = current->next;
        }

        if (prioritySelected == nullptr)
        {
            currentTime++;
            continue;
        }

        
        prioritySelected->remaining_time--;
        currentTime++;

        
        if (prioritySelected->remaining_time == 0)
        {
            completed++;
            prioritySelected->turnaround_time = currentTime - prioritySelected->arrival_time;
            prioritySelected->waiting_time = prioritySelected->turnaround_time - prioritySelected->burst_time;
            totalWaitingTime += prioritySelected->waiting_time;
        }
    }

    
    current = head;
    while (current != nullptr)
    {
        outputFile << ":" << current->waiting_time;
        cout << ":" << current->waiting_time;
        current = current->next;
    }

    
    double averageWaitingTime = (double)totalWaitingTime / processCounter;
    averageWaitingTime = (int)(averageWaitingTime *10 + 0.5)/10.0; // Round to 1 decimal place

    outputFile << ":" << averageWaitingTime << endl;
    cout << ":" << averageWaitingTime << endl;
}

void RoundRobin(Process *head, int time_quantum, ofstream &outputFile)
{
    Process *rr_current = head;
    int currentTime = 0;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    int processCounter = 0;
    int completed = 0;

    //Reading the Process numbers
    Process *temp = head;
    while (temp != nullptr)
    {
        processCounter++;
        temp = temp->next;
    }

    outputFile << "6"; 
    cout << "6";

    
    while (completed < processCounter)
    {
        rr_current = head; 

        while (rr_current != nullptr)
        {
            if (rr_current->arrival_time <= currentTime && rr_current->remaining_time > 0)
            {
                int time_to_run = (rr_current->remaining_time <= time_quantum) ? rr_current->remaining_time : time_quantum;
                rr_current->remaining_time -= time_to_run;
                currentTime += time_to_run;

                if (rr_current->remaining_time == 0)
                {
                    
                    rr_current->turnaround_time = currentTime - rr_current->arrival_time;
                    rr_current->waiting_time = rr_current->turnaround_time - rr_current->burst_time;
                    totalWaitingTime += rr_current->waiting_time;
                    totalTurnaroundTime += rr_current->turnaround_time;
                    completed++;
                }
            }

            rr_current = rr_current->next;
        }
    }

    
    rr_current = head;
    while (rr_current != nullptr)
    {
        outputFile << ":" << rr_current->waiting_time;
        cout << ":" << rr_current->waiting_time;
        rr_current = rr_current->next;
    }

    double averageWaitingTime = (double)totalWaitingTime / processCounter;
    averageWaitingTime = (int)(averageWaitingTime * 10 + 0.5) / 10.0; // Round to 1 decimal place

    outputFile << ":" << averageWaitingTime << endl;
    cout << ":" << averageWaitingTime << endl;
}




int main(int argc, char *argv[])
{
    int time_quantum = 0;
    string input_file, output_file;

    ParseArguments(argc, argv, time_quantum, input_file, output_file);

    Process *head = nullptr;
    Read_File(input_file,head);

    
    //Opening output file here
    ofstream The_outputFile(output_file);
    if(!The_outputFile)
    {
        cerr<<"there is an error occured while opening output file please check"<<endl;
        exit(EXIT_FAILURE);
    }
    // Functions CALL

    FCFS(head,The_outputFile);
    memoryCleanup(head);

    Process *head2 = nullptr;
    Read_File(input_file,head2);
    ShortestJobFirst_NonPreemptive(head2,The_outputFile);
    memoryCleanup(head2);

    Process *head3 = nullptr;
    Read_File(input_file,head3);
    ShortestJobFirst_Preemptive(head3,The_outputFile);
    memoryCleanup(head3);

    Process *head4 = nullptr;
    Read_File(input_file,head4);
    PriorityScheduling_NonPreemp(head4,The_outputFile);
    memoryCleanup(head4);

    Process *head5 = nullptr;
    Read_File(input_file,head5);
    PriorityScheduling_Preemptive(head5,The_outputFile);
    memoryCleanup(head5);

    Process *head6 = nullptr;
    Read_File(input_file,head6);
    RoundRobin(head6,time_quantum,The_outputFile);
    memoryCleanup(head6);

    //Closing the file here
    The_outputFile.close();

   


    return 0;
}
