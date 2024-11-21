#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <atomic>
#include <termios.h>
#include <unistd.h>  


using namespace std;

atomic<bool> stopTimer(false);


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
// Function to configure terminal to allow immediate keypress detection
void setInputMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);           
    t.c_lflag &= ~(ICANON | ECHO);         
    t.c_cc[VMIN] = 1;                      
    t.c_cc[VTIME] = 0;                     //time out in tenths of seconds..
    tcsetattr(STDIN_FILENO, TCSANOW, &t);  
}
// Function to restore terminal settings
void resetInputMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO);          // Enable canonical mode and echo back
    tcsetattr(STDIN_FILENO, TCSANOW, &t);  // Apply the changes
}
// Function to read a single character from the user without needing Enter key
char getChar() {
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
}





void StartPomodoroTimer(){
  int WorkTime, BreakTime;

  //choices for worktime...
  cout << "Choose work time: " << endl;
  cout << "1. 25 minutes " << endl;
  cout << "2. 50 minutes " << endl;
  cout << "3. 90 minutes " << endl;
  cout << "4. custom time(set your own)" << endl;
  int choice;
  cin >> choice;

  switch (choice){
    case 1:
      WorkTime = 25;
      break;
    case 2: 
      WorkTime = 50;
      break;
    case 3: 
      WorkTime = 90;
      break;
    case 4:
      cout << "Enter custom work time in minutes: " << endl;
      cin >> WorkTime;
      break;
    default: 
      cout <<"Invalid choice, using default 25 minutes." << endl;
      WorkTime = 25;
  }

  //choices for breaktime...
  cout << "Choose break time:" << endl;
  cout << "1. 5 minutes" << endl;
  cout << "2. 10 minutes" << endl;
  cout << "3. 15 minutes" << endl;
  cout << "4. Custom break time (set your own)" << endl;
  cin >> choice;

  switch(choice){
    case 1:
      BreakTime = 5;
      break;
    case 2: 
      BreakTime = 10;
      break;
    case 3:
      BreakTime = 15;
      break;
    case 4:
      cout << "Enter custom break time in minutes: " << endl;
      cin >> BreakTime;
      break;
    default:
    cout <<"Invalid choice, using default 5 minutes." << endl;
    BreakTime = 5;
  }

  // Start work session
  cout << "Pomodoro started! Work for " << WorkTime << " minutes." << endl;
  stopTimer = false;  // Reset the stop flag

  // Start work timer thread
  thread timerThread([WorkTime]() {
      for (int i = WorkTime * 60; i > 0; --i) {
          if (stopTimer) {
              cout << "\nPomodoro session stopped early!" << endl;
              return;  // Stop the timer early
          }
          this_thread::sleep_for(chrono::seconds(1));
      }
      cout << "Work session over! Take a break." << endl;
  });

  setInputMode();

  char UserInput;
  while (timerThread.joinable()){
    UserInput = getchar();
    if(UserInput == 'q' || UserInput == 'Q'){
      stopTimer = true;
      timerThread.join();
      break;
    }
  }

  resetInputMode();

  // Start break session only if the work session was not stopped early
  if (!stopTimer) {
    cout << "Break time. Take a rest for " << BreakTime << " minutes." << endl;
    for (int i = BreakTime * 60; i > 0; --i) {
      if (stopTimer) {
        cout << "\nBreak session stopped early." << endl;
        return;  // Stop the break session early
      }
      this_thread::sleep_for(chrono::seconds(1));
    }
    cout << "Break over. Ready for the next session." << endl;
  }
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
    cout << "6. Start Pomodoro Timer"<<endl;
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
    case  6:
      StartPomodoroTimer();
    default: 
      cout << "Invalid choice. Please try again." << endl;  

    }
   }while (choice!=5); 

  return 0; 
}