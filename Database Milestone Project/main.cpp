/* ******************************************
Authors: JASON LEE(106186216), IN HO HAN(106053218), MIN GYU BANG(127944213)
Date: April 12, 2022
Title: DBS211 NGG Milestone 3
******************************************** */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <occi.h>
#include <ctime>

using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

struct Employee 
{
   int m_employeeNumber;
   char lastName[50];
   char firstName[50];
   char email[100];
   char phone[50];
   char extension[10];
   char reportsTo[100];
   char jobTitle[50];
   char city[50];

};

int menu(void);
int getID();
int findEmployee(Connection *conn, int employeeNumber, struct Employee* emp);
void displayEmployee(Connection *conn, struct Employee emp);
void displayAllEmployees(Connection *conn);
void insertEmployee(Connection* conn, struct Employee emp);
void updateEmployee(Connection* conn, int employeeNumber , struct Employee emp);
void deleteEmployee(Connection* conn, int employeeNumber, struct Employee emp);

int main(void) {
   // OCCI Variables
   Environment* env = nullptr;
   Connection* conn = nullptr;
 
   // User Variables
   string str;
   string usr = "";
   string pass = "";
   string srv = "";
   time_t now = time(0);
   int empNum = 0;
   char* currentTime = ctime(&now);

   int selection;
   Employee emp{};

   try 
   {
      env = Environment::createEnvironment(Environment::DEFAULT);
      conn = env->createConnection(usr, pass, srv);            //Creates connection through inserting id, password, and server address
      cout << "Connection is Successful" << currentTime;
      do 
      {
         selection = menu();                                   //Everytime it calls function menu until user input 0
         switch (selection)
         {
         case 1:
            cout << endl;
            displayEmployee(conn, emp);                        //Calls displayEmployee if 1 is pressed
            cout << endl;
            break;
         case 2:
            cout << endl;
            displayAllEmployees(conn);                         //Calls displayAllEmployee if 2 is pressed
            cout << endl;
            break;
         case 3:
            cout << endl;                                   
            emp.m_employeeNumber = getID();                    // calls getID() to get user input 
            cout << "Last Name: ";
            cin.get(emp.lastName, 50, '\n');                   // stores user input to emp.lastName until new line is entered
            cin.clear();                                       // clears cin
            cin.ignore(1000, '\n');                            // cin gets ignored up to 1000 characters or it finds new line
            cout << "First Name: ";
            cin.get(emp.firstName, 50, '\n');
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Email: ";
            cin.get(emp.email, 100, '\n');
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Extension: ";
            cin.get(emp.extension, 10, '\n');
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Job Title: ";
            cin.get(emp.jobTitle, 50, '\n');
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "City: ";
            cin.get(emp.city, 50, '\n');
            cin.clear();
            cin.ignore(1000, '\n');
            cout << endl;
            insertEmployee(conn, emp);
            break;
         case 4:
            cout << endl;
            empNum = getID();
            cout << endl;
            updateEmployee(conn, empNum, emp);
            break;
         case 5:
            cout << endl;
            empNum = getID();
            cout << endl;
            deleteEmployee(conn, empNum, emp);
            break;
         }
      } while (selection != 0);
      env->terminateConnection(conn);
      Environment::terminateEnvironment(env);
   }
   catch (SQLException& sqlExcp) 
   {                                //If connection fails... 
      cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();   
   }
   return 0;
}

int menu(void)
{
   int num;
   bool flag = true;
   char next{};
   cout << "********************* HR Menu *********************\n";
   cout << "1) Find Employee\n";
   cout << "2) Employees Report\n";
   cout << "3) Add Employee\n";
   cout << "4) Update Employee\n";
   cout << "5) Remove Employee\n";
   cout << "0) Exit\n";
   cout << "Please select one of the option\n> ";
   while (flag)
   {
      cin >> num;                                                      //Prompts for menu input
      next = cin.peek();                                               //Checks next character to prevent typing more than one digit or invalid value
      if (!cin)
      {
         cin.clear();                                                  //Clears istream state
         cin.ignore(1000, '\n');                                       //istream ignores all characters until newline
         cout << "ERROR: Value must be an integer: ";
      }
      else if (!(num >= 0 && num <= 5))                                //if num is less than 0 or greater than 5...
      {
         cout << "ERROR: Value out of range [0 <= Value <= 5]: ";
      }
      else if (next != '\n')                                            //if num is not one digit number
      {
         cout << "ERROR: Only one digit number is allowed: ";
      }
      else if (num >= 0 && num <= 5)                                     //if valid num is received...
      {
         flag = false;
      }
   }
   return num;
}

int getID()
{
   int findNum = 0;
   bool keepGoing = true;
   cout << "Employee Number: ";
   while (keepGoing) 
   {
      cin >> findNum;                                                      //prompts for employee number
      cin.clear();
      cin.ignore(1000, '\n');
      if (cin) 
      {
         if (findNum > 9999 || findNum < 1000)                             //if user input is not four digit number
         {
            cout << "ERROR: Employee number must be 4 digits: ";
         }
         else                                                               //if user input is valid...
         {
            keepGoing = false;
         }
      }
      else
      {
         cin.clear();                                                        //Clears istream state
         cin.ignore(1000, '\n');                                             //istream ignores all characters until newline
         cout << "ERROR: Value must be an integer: ";
      }
   }
   return findNum;
}

int findEmployee(Connection* conn, int employeeNumber, Employee* emp)
{
   int num = 0;
   int empNum = 0;
   string str1 = "SELECT\n\te.employeeNumber,\n\te.lastName,\n\te.firstName,\n\te.email,\n\to.phone,\n\te.extension,\n\tc.firstname || ' ' || c.lastname AS reportsTo, \n\t\e.jobTitle,\n\to.city\nFROM employees e\n\tLEFT OUTER JOIN employees c ON e.reportsTo = c.employeeNumber\n\tLEFT OUTER JOIN offices o ON e.officeCode = o.officeCode\nWHERE e.employeeNumber = ";                    //sql statement is stored to string and adds employee number string
   string str2 = str1 + to_string(employeeNumber);
   Statement* stmt = conn->createStatement();                                   //sql statement is created
   ResultSet* rs = stmt->executeQuery(str2);                                    //sql statement is executed
   while (rs->next()) {                                                         //each field is stored into variables respectively
      empNum = rs->getInt(1);
      emp->m_employeeNumber = rs->getInt(1);
      strcpy(emp->lastName, rs->getString(2).c_str());
      strcpy(emp->firstName, rs->getString(3).c_str());
      strcpy(emp->email, rs->getString(4).c_str());
      strcpy(emp->phone, rs->getString(5).c_str());
      strcpy(emp->extension, rs->getString(6).c_str());
      strcpy(emp->reportsTo, rs->getString(7).c_str());
      strcpy(emp->jobTitle, rs->getString(8).c_str());
      strcpy(emp->city, rs->getString(9).c_str());
   }
   conn->terminateStatement(stmt);                                               //terminates statement
   if (empNum > 0)                                                //if employee exists num is set to 1
   {
      num = 1;
   }
   return num;
}

void displayEmployee(Connection* conn, Employee emp)
{
   int number = 0;
   int result = 0;
   number = getID();                                                               //getID validates user input for employee number, if its valid stored into number
   result = findEmployee(conn, number, &emp);                                      //call findEmployee to check if employee exists
   if (result == 1)                                                                //if employee exists...
   {
      cout << "\nemployeeNumber = " << emp.m_employeeNumber << "\n";
      cout << "lastName = " << emp.lastName << "\n";
      cout << "firstName = " << emp.firstName << "\n";
      cout << "email = " << emp.email << "\n";
      cout << "phone = " << emp.phone << "\n";
      cout << "extension = " << emp.extension << "\n";
      cout << "reportsTo = " << emp.reportsTo << "\n";
      cout << "jobTitle = " << emp.jobTitle << "\n";
      cout << "city = " << emp.city << "\n";
   }
   else if (result == 0)                                                            //if employee does not exist...
   {
      cout << "\nERROR: Employee " << number << " doesn't exist.\n";
   }
}

void displayAllEmployees(Connection* conn)
{
   string str1 = "SELECT\n\te.employeeNumber,\n\te.lastName,\n\te.firstName,\n\te.email,\n\to.phone,\n\te.extension,\n\tc.firstname || ' ' || c.lastname AS reportsTo\n\nFROM employees e\n\tLEFT OUTER JOIN employees c ON e.reportsTo = c.employeeNumber\n\tLEFT OUTER JOIN offices o ON e.officeCode = o.officeCode\nORDER BY e.officeCode,employeeNumber";                                                     //sql statement is stored to string and adds employee number string
   Statement* stmt = conn->createStatement();                                         //sql statement created
   ResultSet* rs = stmt->executeQuery(str1);                                          //sql statement executed
   cout << "E       Employee Name            Email                              Phone             Ext       Manager" << endl;
   cout << "----------------------------------------------------------------------------------------------------------------" << endl;
   while (rs->next()) {                                                                //each field is stored into variables respectively                            
      int empNum = rs->getInt(1);
      string lName = rs->getString(2);
      string fName = rs->getString(3);
      string email = rs->getString(4);
      string phone = rs->getString(5);
      string extension = rs->getString(6);
      string reportsTo = rs->getString(7);
      cout.setf(ios::left);                                                              //formats output to left justified, begin printing out all fields
      cout.width(8);                                                                     //sets next output width to 8 spaces
      cout << empNum;
      cout.width(25);                                                                    //sets next output width to 25 spaces   
      cout << fName + " " + lName;
      cout.width(35);                                                                    //sets next output width to 35 spaces                                     
      cout << email;
      cout.width(18);                                                                    //sets next output width to 18 spaces
      cout << phone;
      cout.width(10);                                                                    //sets next output width to 10 spaces
      cout << extension;
      cout.width(12);                                                                    //sets next output width to 12 spaces
      cout << reportsTo << "\n";
   }
   conn->terminateStatement(stmt);                                                       //terminate statement
}

void insertEmployee(Connection* conn, Employee emp)
{
   int num = 0;
   int officeCode = 1;
   int reportTo = 1002;
   num = findEmployee(conn, emp.m_employeeNumber, &emp);                                  // calls findEmployee() to check if the employee already exists
   if (num != 0)                                                                          
   {
      cout << "An employee with the same employee number exists.\n\n";
   
   }
   else
   {
      string str1 = "INSERT INTO employees VALUES (" + to_string(emp.m_employeeNumber) + ", '" + emp.lastName + "', '" + emp.firstName + "', '" + emp.extension + "', '" + emp.email + "', " + to_string(officeCode) + ", " + to_string(reportTo) + ", '" + emp.jobTitle + "')";
      Statement* stmt = conn->createStatement();
      ResultSet* rs = stmt->executeQuery(str1);
      conn->terminateStatement(stmt);
      cout << "The new employee is added successfully.\n\n";
   }
}

void updateEmployee(Connection* conn, int employeeNumber, struct Employee emp)
{
   int num;
   char extension[10]{};
   num = findEmployee(conn, employeeNumber, &emp);                                        // calls findEmployee() to check if the employee already exists
   if (num != 0)
   {
      cout << "New Extension: ";
      cin.get(extension, 10, '\n');                                                       // stores user's input in extension variable
      cin.clear();
      string str3 = to_string(emp.m_employeeNumber);                                      // converts employeeNumber to a string 
      string str2 = extension;
      string str1 = "UPDATE employees SET extension = '" + str2 + "' " + "WHERE employeeNumber = " + str3;    
      Statement* stmt = conn->createStatement();                                          // creates satement
      ResultSet* rs = stmt->executeQuery(str1);                                           // executes statment
      conn->terminateStatement(stmt);
      cout << "The employee is updated successfully.\n\n";
   }
}

void deleteEmployee(Connection* conn, int employeeNumber, struct Employee emp)
{
   int num = 0;
   num = findEmployee(conn, employeeNumber, &emp);                                        // calls findEmployee() to check if the employee already exists
   if (num != 0)
   {
      string str1 = "DELETE FROM employees WHERE employeeNumber = " + to_string(emp.m_employeeNumber);
      Statement* stmt = conn->createStatement();                                          // creates statment
      ResultSet* rs = stmt->executeQuery(str1);                                           // executes statment
      conn->terminateStatement(stmt);
      cout << "The employee is deleted.\n\n";
   }
   else
   {
      cout << "The employee does not exist.\n\n";
   }
}
