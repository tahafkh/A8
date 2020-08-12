#include<string>
#include<sstream>
#include<algorithm>
#include<iostream>
#include<vector>
#include<map>
#include<functional>
#define MINUS '-'
#define PLUS '+'
#define MULTIPLIES '*'
#define DIVIDES '/'
#define WHITE_SPACE ' '
#define INPUT_CHARACTER '?'
#define OUTPUT_CHARACTER '!'
#define VALUE_CHARACTER "$"
#define ASSIGN_CHARACTER '='
#define INITIAL_VALUE 0
#define MINIMUM_STEPS "Minimum Steps: "
using namespace std;
class Variable{
public:
    Variable(string variable_name);
    void set_value(int new_value);
    int get_value(){ return value;};
    string get_name(){ return name;};
private:
    int value;
    string name;
};
Variable::Variable(string variable_name){
    name = variable_name;
}
void Variable::set_value(int new_value){
    value = new_value;
}
class Interpreter{
public:
    Interpreter();
    ~Interpreter();
    void start_interpreting();
    void check_minimum_steps();
    void add_command(string new_command);
private:
    void determine_inputs();
    void assign_input(stringstream& command);
    void assign_variable(stringstream& command);
    void show_expression(stringstream& expression);
    bool check_dependency(string& first_command,string& second_command);
    void check_step(string& new_command);
    void determine_expression_operands(stringstream& expression,vector<string>& operands);
    int calculate_expression(stringstream& expression);
    bool is_operator(char expression_character);
    bool is_variable(string expression_operand);
    void remove_whitespaces(string& command);
    vector<vector<string>> parallel_commands;
    vector<string> commands;
    vector<Variable*> variables;
    vector<int> inputs_values;
    map<char,function<int(int,int)>> operators;
};
Interpreter::Interpreter(){
    operators[PLUS] = plus<int>();
    operators[MINUS] = minus<int>();
    operators[MULTIPLIES] = multiplies<int>();
    operators[DIVIDES] = divides<int>();
}
Interpreter::~Interpreter(){
    for(auto variable : variables){
        delete variable;
    }
}
void Interpreter::check_step(string& new_command){
    for(int i = parallel_commands.size() - 1;i>=0;i--){
        int j;
        for(j=0;j<parallel_commands[i].size();j++){
            if(!check_dependency(new_command,parallel_commands[i][j])){
                break;
            }
        }
        if(j==parallel_commands[i].size() - 1){
            parallel_commands[i].push_back(new_command);
            return;
        }
    }
    parallel_commands.push_back({new_command});
}
bool Interpreter::check_dependency(string& first,string& second){
    char first_type = first[0],second_type = second[0];
    if((first_type == INPUT_CHARACTER && second_type == INPUT_CHARACTER) || (first_type == OUTPUT_CHARACTER && second_type == OUTPUT_CHARACTER)){
        return false;
    }
    if((first_type == INPUT_CHARACTER && second_type == OUTPUT_CHARACTER) || (second_type == INPUT_CHARACTER && first_type == OUTPUT_CHARACTER)){
        stringstream first_command(first);
        stringstream second_command(second);
    }
}
void Interpreter::determine_expression_operands(stringstream& expression,vector<string>& operands){
    char expression_character;
    string operand;
    expression_character = expression.get();
    while(!is_operator(expression_character) && !expression.eof()){
        operand.push_back(expression_character);
        expression_character = expression.get();
    }
    if(is_variable(operand) && !operand.empty()){
        operands.push_back(operand);
    }
    if(operand.empty()){
        return;
    }
    determine_expression_operands(expression,operands);
    return;
}
bool Interpreter::is_operator(char expression_character){
    return expression_character == PLUS || expression_character == MINUS || expression_character == DIVIDES || expression_character == MULTIPLIES;
}
bool Interpreter::is_variable(string expression_operand){
    try{
        int operand = stoi(expression_operand);
        return false;
    }
    catch(...){
        return true;
    }
}
int Interpreter::calculate_expression(stringstream& expression){
    char expression_character;
    string operand;
    int operand_value;
    expression_character = expression.get();
    while(!is_operator(expression_character) && !expression.eof()){
        operand.push_back(expression_character);
        expression_character = expression.get();
    }
    if(is_variable(operand)){
        auto variable_iterator = find_if(variables.begin(),variables.end(),[=](Variable* variable){ return variable->get_name() == operand;});
        operand_value = (variable_iterator != variables.end()) ? (*variable_iterator)->get_value() : INITIAL_VALUE;
    }
    else{
        operand_value = stoi(operand);
    }
    if(is_operator(expression_character)){
        return operators[expression_character](operand_value,calculate_expression(expression));
    }
    return operand_value;
}
void Interpreter::remove_whitespaces(string& command){
    command.erase(remove(command.begin(),command.end(),WHITE_SPACE),command.end());
}
void Interpreter::add_command(string new_command){
    commands.push_back(new_command);
}
void Interpreter::determine_inputs(){
    stringstream inputs_value(commands.back());
    string command_type;
    getline(inputs_value,command_type,WHITE_SPACE);
    if(command_type == VALUE_CHARACTER){
        string value;
        while(getline(inputs_value,value,WHITE_SPACE)){
            inputs_values.push_back(stoi(value));
        }
        commands.erase(commands.begin() + commands.size() - 1);
    }
}
void Interpreter::start_interpreting(){
    determine_inputs();
    for(int i = 0 ; i < commands.size();i++){
        remove_whitespaces(commands[i]);
        stringstream command(commands[i]);
        char command_type;
        auto position = command.tellg();
        command >> command_type;
        if(command_type == INPUT_CHARACTER){
            assign_input(command);
        }
        else if(command_type == OUTPUT_CHARACTER){
            show_expression(command);
        }
        else{
            command.seekg(position);
            assign_variable(command);
        }
    }
}
void Interpreter::show_expression(stringstream& command){
    cout<<calculate_expression(command)<<endl;
}
void Interpreter::assign_input(stringstream& command){
    string input_name;
    getline(command,input_name);
    variables.push_back(new Variable(input_name));
    variables.back()->set_value(inputs_values.front());
    inputs_values.erase(inputs_values.begin());
}
void Interpreter::assign_variable(stringstream& command){
    string variable_name;
    getline(command,variable_name,ASSIGN_CHARACTER);
    auto variable_iterator = find_if(variables.begin(),variables.end(),[=](Variable* variable){ return variable->get_name() == variable_name;});
    if(variable_iterator == variables.end()){
        variables.push_back(new Variable(variable_name));
        variables.back()->set_value(calculate_expression(command));
    }
    else{
        (*variable_iterator)->set_value(calculate_expression(command));
    }
}
void Interpreter::check_minimum_steps(){
    cout<<MINIMUM_STEPS<<parallel_commands.size()<<endl;
}
int main(){
    Interpreter zebarsad_interpreter;
    string command;
    while(getline(cin,command)){
        zebarsad_interpreter.add_command(command);
    }
    zebarsad_interpreter.start_interpreting();
    return 0;
}