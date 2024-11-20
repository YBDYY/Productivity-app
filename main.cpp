#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>  


using namespace std;


struct Task{
    string name;
    bool IsComplete;
    int priority;
    string deadline;
    string category;
};

void displayASCIIart(){
  
    cout << R"(
     ███      ▄██████▄  ████████▄   ▄██████▄        ▄█        ▄█     ▄████████     ███     
    ▀█████████▄ ███    ███ ███   ▀███ ███    ███      ███       ███    ███    ███ ▀█████████▄ 
       ▀███▀▀██ ███    ███ ███    ███ ███    ███      ███       ███▌   ███    █▀     ▀███▀▀██ 
        ███   ▀ ███    ███ ███    ███ ███    ███      ███       ███▌   ███            ███   ▀ 
        ███     ███    ███ ███    ███ ███    ███      ███       ███▌ ▀███████████     ███     
        ███     ███    ███ ███    ███ ███    ███      ███       ███           ███     ███     
        ███     ███    ███ ███   ▄███ ███    ███      ███▌    ▄ ███     ▄█    ███     ███     
       ▄████▀    ▀██████▀  ████████▀   ▀██████▀       █████▄▄██ █▀    ▄████████▀     ▄████▀   
    )" << endl;
}
void Pause(){
  this_thread::sleep_for(chrono::seconds(2));
}

void SaveTask(const vector<Task>& tasks){
  ofstream outfile("tasks.txt");
 if(outfile.is_open()){
  for(const auto& task : tasks){
    outfile << task.name << ","
            << task.priority << ","
            << task.deadline << ","
            << task.IsComplete << ","
            <<task.category << endl;
  }
   outfile.close();
   cout << "Tasks have been saved. " << endl; 
 } else { 
  cout << "Error opening file " << endl;
 }
  
}

void LoadTasks(vector<Task>& tasks) {
    ifstream inFile("tasks.txt");
    if (inFile.is_open()) {
        Task task;
        string status;
        while (getline(inFile, task.name, ',') &&
               inFile >> task.priority &&
               getline(inFile, task.deadline, ',') &&
               inFile >> task.IsComplete &&
               getline(inFile, task.category)) {
            inFile.ignore(numeric_limits<streamsize>::max(), '\n');  // to ignore any leftover newline
            tasks.push_back(task);
        }
        inFile.close();
    } else {
        cout << "No saved tasks found!" << endl;
    }
}






void AddTask(vector<Task> & tasks){
  Task newtask;
  cout << "Enter task name : ";
  cin.ignore();
  getline(cin,newtask.name);

  cout << "Enter priority (1 to 5)";
  while (!(cin >> newtask.priority)||newtask.priority<1 || newtask.priority > 5){
   cout << "Invalid input! Enter a valid priority between 1 and 5: ";
   cin.clear();
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }

  cin.ignore();


  cout << "Enter deadline : ";
  getline(cin,newtask.deadline);

  cout <<"Enter category (Work,personal,school,etc.)";
  getline(cin,newtask.category);

  newtask.IsComplete = false;

  tasks.push_back(newtask);
}

void RemoveTask(vector<Task> & tasks){
  int taskIndex;
  cout << "Enter the task number to remove: ";
  cin >> taskIndex;
  if (taskIndex>=0 && taskIndex<tasks.size()){
    tasks.erase(tasks.begin()+taskIndex);
    cout << "Task removed " << endl;

  }   else {
    cout << "Invalid task number " << endl;
  }
}
void DisplayTasks(const vector<Task>& tasks){
  if(tasks.empty()){
    cout << "No tasks available " << endl;
  } else {
    for ( int i =0; i<tasks.size(); i++){
      const Task& task = tasks[i];
      cout << "Task " << i + 1 << ": " << task.name << endl;
            cout << "Priority: " << task.priority << endl;
            cout << "Deadline: " << task.deadline << endl;
            cout << "Category: " << task.category << endl;
            cout << "Status: " << (task.IsComplete ? "Completed" : "Not Completed") << endl;
            cout << "----------------------------" << endl;

    }
  }

}
void ToggleTaskStatus(vector<Task>&tasks){
  int taskIndex;
  cout << "Enter the task number to toggle its status: ";
  cin >> taskIndex;

  taskIndex--;


  if (taskIndex >=0 && taskIndex < tasks.size()){
    tasks[taskIndex].IsComplete = !tasks[taskIndex].IsComplete;
    cout << "Task status updated" << endl;
  }else { 
    cout << "Invalid task number" << endl;
  }
}




int main(){
    vector<Task> tasks;   
    int choice;
  displayASCIIart();
  Pause();

   do {
    cout << "To-Do list menu" << endl;
    cout << "1. Add task" << endl;
    cout << "2. Remove task" << endl;
    cout << "3. Display Tasks"<< endl;
    cout << "4. Toggle Task Status"<< endl;
    cout << "5. Exit" << endl; 
    cout << "Enter your choice: "<<endl;
    cin >> choice;

    switch(choice){
    case 1:
      AddTask(tasks);
      break;
    case 2:
      RemoveTask(tasks);
      break;
    case 3:
      DisplayTasks(tasks);
      break;
    case 4:
      ToggleTaskStatus(tasks);
      break;
    case 5:
      cout << "Exiting program..." << endl;
      break;
    default: 
      cout << "Invalid choice. Please try again." << endl;  

    }
   }while (choice!=5); 

  return 0;
}