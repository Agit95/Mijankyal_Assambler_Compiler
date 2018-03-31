#define _CRT_SECURE_NO_WARNINGS
#include "Parser.h"
#include "Operators_and_bracket.h"
#include <cstring>



//ctor  by  default
C_Parser::C_Parser()
{
}


// virtual ~ctor  
C_Parser::~C_Parser()
{
}

//copy  ctor
C_Parser::C_Parser(const C_Parser& object)
{
}

//returned   variable  naem  is  register  name  or  not
bool  C_Parser::is_register_name(const std::string& variable_name) const _NOEXCEPT
{
	std::string obj;
	if (variable_name.length() == 3)
	{
		obj += variable_name[1];
		obj += variable_name[2];
	}
	else if (variable_name.length() == 2)
		obj += variable_name[1];

	if (variable_name.length() == 3 && (variable_name[0] == 'R' || variable_name[0] == 'A') &&
		((is_number(obj) && (is_number(obj)))))
		return true;
	else if ((variable_name.length() == 2 && (variable_name[0] == 'R' || variable_name[0] == 'A') &&
		((is_number(obj)))))
		return  true;
	else if ((variable_name.length() == 1 && (variable_name[0] == 'R' || variable_name[0] == 'A')))
		return true;
	return false;
}


//returned   symbole  is  number  or not
bool  C_Parser::is_number(std::string& symbole)  const  _NOEXCEPT
{
	int i = 0;
	do
	{
		if (symbole[i] != '0' && symbole[i] != '1' && symbole[i] != '2' && symbole[i] != '3' && symbole[i] != '4' &&
			symbole[i] != '5' && symbole[i] != '6' && symbole[i] != '7' && symbole[i] != '8' && symbole[i] != '9')
			return false;
		++i;
	} while (i < symbole.size() - 1);
	return true;
}


//get  the  execuited  variable  and  register  type 
EType C_Parser::get_type(int& number) const  _NOEXCEPT
{
	if (number > -128 && number < 127)
		return EType::BYTE;
	if (number > -8388608 && number < 8388607)
		return EType::WORD;
	if (number > -2147483647 && number < 2147483647)
		return EType::DWORD;
	if (number > -4294967296 && number < 429496729)
		return EType::QWORD;
	return EType::DWORD;
}


void  C_Parser::tok_lexing_string(std::string lexer_string, int lexer_line_number)
{
	const char* first_even = strchr(lexer_string.c_str(), '=');
	const char* last_even = strrchr(lexer_string.c_str(), '=');


	if (first_even != last_even)
		throw C_Compile_Error("WRONG ERROR ';' , PUT ';'  IN  LINE  ", lexer_line_number);


	char*  lexer_char_string = const_cast<char*>(lexer_string.c_str());

	char* temp_str = std::strtok(lexer_char_string, " ");
	//get  postfix  tokens
	while (temp_str != NULL)
	{
		std::string  obj;
		obj += temp_str;
		m_oVariable_Operators_and_Operands.push(obj);
		temp_str = strtok(NULL, " ");
	}
}


void  C_Parser::parse_code_line(int code_line_number)
{
	std::stringstream        temp_code_stream;
	std::stringstream        temp_data_stream;
	std::stack<std::string>  temp_operator_stack;
	std::stack<E_GPR>        temp_GPR_stack;

	while (!m_oVariable_Operators_and_Operands.empty())
	{
		//validate   if  variabl  name  is  register  name  then throw  exeption
		if (is_register_name(m_oVariable_Operators_and_Operands.top()))
			throw C_Compile_Error("INVALID  VARIABLE IN LINE ", code_line_number);

		if (is_operator(m_oVariable_Operators_and_Operands.top()))
		{
			temp_operator_stack.push(m_oVariable_Operators_and_Operands.top());
			m_oVariable_Operators_and_Operands.pop();
		}
		else if (is_number(m_oVariable_Operators_and_Operands.top()))
		{
			if (is_operator(temp_operator_stack.top()))
			{
				temp_operator_stack.push(m_oVariable_Operators_and_Operands.top());
				m_oVariable_Operators_and_Operands.pop();
			}
			else  if (is_number(temp_operator_stack.top()))
			{
				std::string temp_1 = m_oVariable_Operators_and_Operands.top();
				std::string temp_2 = temp_operator_stack.top();
				m_oVariable_Operators_and_Operands.pop();
				temp_operator_stack.pop();
				/*-------------------------------------------------------------*/
				//cast  string to  int and  write  number in register
				int i = std::atoi(temp_1.c_str());
				E_GPR reg = find_empty_GPR_in_input_type(get_type(i));
				temp_code_stream << "ASSIGN " << get_typename(get_type(i)) << " R" << reg << " , " << i << std::endl;
				change_GPR_state(reg);
				/*--------------------------------------------------------------*/
				//cast  string  to  int  and  write  number  in register
				i = std::atoi(temp_2.c_str());
				E_GPR reg_2 = find_empty_GPR_in_input_type(get_type(i));
				temp_code_stream << "ASSIGN " << get_typename(get_type(i)) << " R" << reg_2 << " , " << i << std::endl;
				change_GPR_state(reg_2);
				/*---------------------------------------------------------------*/
				//call  code  generator
				generate_code(reg, reg_2, temp_operator_stack.top(), code_line_number,
					temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
			}
			else  if (is_function(temp_operator_stack.top()))
			{
				std::string temp_1 = m_oVariable_Operators_and_Operands.top();
				std::string temp_2 = temp_operator_stack.top();
				m_oVariable_Operators_and_Operands.pop();
				temp_operator_stack.pop();
				/*-------------------------------------------------------------*/
				//cast  string to int and  write  number  in register
				int i = std::atoi(temp_1.c_str());
				E_GPR reg = find_empty_GPR_in_input_type(get_type(i));
				temp_code_stream << "ASSIGN " << get_typename(get_type(i)) << " R" << reg << " , " << i << std::endl;
				change_GPR_state(reg);

				//call function call  par code generator
				generate_function_call_part_code(temp_2, temp_code_stream);

				/*--------------------------------------------------------------*/
				// call  code generator
				generate_code(reg, E_GPR::R0, temp_operator_stack.top(), code_line_number,
					temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
				m_oGPR_name_and_state.find(E_GPR::R0)->second = true;
			}
			else  if (is_variable(temp_operator_stack.top()))
			{
				std::string temp_1 = m_oVariable_Operators_and_Operands.top();
				std::string temp_2 = temp_operator_stack.top();
				m_oVariable_Operators_and_Operands.pop();
				temp_operator_stack.pop();
				/*-------------------------------------------------------------*/
				//cast  string  to int 
				int i = std::atoi(temp_1.c_str());
				//find  empty  GPR  from work
				E_GPR reg = find_empty_GPR_in_input_type(get_type(i));
				//write  number  in register
				temp_code_stream << "ASSIGN " << get_typename(get_type(i)) << " R" << reg << " , " << i << std::endl;
				//reserve  register
				change_GPR_state(reg);
				/*--------------------------------------------------------------*/
				std::map<std::string, EType>::iterator iter = m_oVariable_name_and_type.find(temp_2);
				//find  and  reserve  GPR 
				E_GPR reg_2 = find_empty_GPR_in_input_type(iter->second);
				change_GPR_state(reg_2);
				//read  variable in  memory
				temp_code_stream << "ASSIGN  A" << find_empty_AR() << " , " << temp_2 << std::endl;
				temp_code_stream << "LOAD  R" << reg_2 << " , A" << find_empty_AR() << std::endl;
				//reserving  ADDRESS register
				change_AR_state(find_empty_AR());
				//generat  working  code
				generate_code(reg, reg_2, temp_operator_stack.top(), code_line_number,
					temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
			}
			else
				throw C_Compile_Error("UNDEFINDE VARIABLE OR  FUNKTION  IN LINE  ", code_line_number);
		}
		else if (is_function(m_oVariable_Operators_and_Operands.top()))
		{
			if (is_operator(temp_operator_stack.top()))
			{
				temp_operator_stack.push(m_oVariable_Operators_and_Operands.top());
				m_oVariable_Operators_and_Operands.pop();
			}
			else  if (is_number(temp_operator_stack.top()))
			{
				std::map<std::string, EType>::iterator iter = m_oVariable_name_and_type.find(m_oVariable_Operators_and_Operands.top());
				std::string temp_2 = m_oVariable_Operators_and_Operands.top();
				m_oVariable_Operators_and_Operands.pop();
				/*-----------------------------------------------------------------------------------------*/
				if (iter != m_oVariable_name_and_type.end())
				{
					//generate  function call code  part
					generate_function_call_part_code(temp_2, temp_code_stream);

					/*-----------------------------------------------------------------------------------------*/
					//cast  string  to  nt
					int i = std::atoi(temp_operator_stack.top().c_str());
					//find  empty  GPR  from work
					E_GPR reg = find_empty_GPR_in_input_type(get_type(i));
					temp_operator_stack.pop();
					//write  number  in rgister
					temp_code_stream << "ASSIGN " << get_typename(get_type(i)) << " R" << reg << " , " << i << std::endl;
					//reserve  register
					change_GPR_state(reg);
					//genrate  working  code
					generate_code(E_GPR::R0, reg, temp_operator_stack.top(), code_line_number,
						temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
					//reserve R0 register
					m_oGPR_name_and_state.find(E_GPR::R0)->second = true;
				}
				else  if (m_oVariable_Operators_and_Operands.size() == 0 && temp_operator_stack.size() == 2)
				{
					//get  function name 
					int i = 0;
					std::string temp_str;
					while (temp_2[i] != '(')
					{
						temp_str += temp_2[i];
						++i;
					}
					/*-----------------------------------------------------------------------------------------*/
					//cast  string  to  int
					i = std::atoi(temp_operator_stack.top().c_str());
					m_oGPR_name_and_state.find(E_GPR::R0)->second = true;
					E_GPR reg = find_empty_GPR_in_input_type(get_type(i));
					//insert  new  variable  in variable  map
					m_oVariable_name_and_type.insert(std::make_pair(temp_2, get_type(i)));

					temp_operator_stack.pop();
					temp_code_stream << "ASSIGN " << get_typename(get_type(i)) << " R" << reg << " , " << i << std::endl;

					change_GPR_state(reg);
					//generate function body
					std::string temp_str_1 = temp_code_stream.str();
					temp_code_stream.str(std::string());
					temp_code_stream << temp_str << ":" << std::endl;
					temp_code_stream << "PUSH A1 " << std::endl;
					temp_code_stream << "MOVE  A0 , A1" << std::endl;
					temp_code_stream << temp_str_1 ;
					temp_code_stream << "MOVE R0 , R" << reg << std::endl;
					temp_code_stream << "POP A1" << std::endl;
					temp_code_stream << "RET" << std::endl;

					//write  in function  code  stream function  body
					m_oTemp_function_stream << temp_code_stream.str() << std::endl;
					//set function call  code  part  in  function strar
					generate_function_call_part_code(temp_2, m_oTemp_variable_stream);
				}
			}
			else if (is_function(temp_operator_stack.top()))
			{
				std::map<std::string, EType>::iterator iter = m_oVariable_name_and_type.find(m_oVariable_Operators_and_Operands.top());
				std::string temp_2 = m_oVariable_Operators_and_Operands.top();
				m_oVariable_Operators_and_Operands.pop();
				//if  we  has'nt  such  function  throw  exeption
				if (iter == m_oVariable_name_and_type.end() && m_oVariable_Operators_and_Operands.size() != 0)
					throw C_Compile_Error("UNDEFINDE FUNCTION IN  LINE ", code_line_number);
				//generate  function call  code  part 
				generate_function_call_part_code(temp_2, temp_code_stream);

				/*-----------------------------------------------------------------------------------------*/
				std::map<std::string, EType>::iterator iter_1 = m_oVariable_name_and_type.find(temp_operator_stack.top());
				if (iter_1 == m_oVariable_name_and_type.end())
					throw C_Compile_Error("UNDEFINDE FUNCTION  IN LINE ", code_line_number);

				temp_2 = temp_operator_stack.top();
				temp_operator_stack.pop();
				// genarate  function call  code 
				generate_function_call_part_code(temp_2, temp_code_stream);
				//find  and  reserve  GPR from work
				E_GPR reg = find_empty_GPR_in_input_type(iter_1->second);
				change_GPR_state(reg);
				/*-----------------------------------------------------------------------------------------*/
				//generate  code  from two  operands
				generate_code(E_GPR::R0, reg, temp_operator_stack.top(), code_line_number,
					temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
				//reserve R0  register
				m_oGPR_name_and_state.find(E_GPR::R0)->second = true;
			}
			else if (is_variable(temp_operator_stack.top()))
			{
				std::map<std::string, EType>::iterator iter = m_oVariable_name_and_type.find(m_oVariable_Operators_and_Operands.top());
				std::string temp_2 = m_oVariable_Operators_and_Operands.top();
				// if  we  has'nt  this  function throw  exeption
				if (iter == m_oVariable_name_and_type.end())
					throw C_Compile_Error("UNDEFINDE FUNCTION IN LINE ", code_line_number);

				m_oVariable_Operators_and_Operands.pop();
				// generate  function call  code  part
				generate_function_call_part_code(temp_2, temp_code_stream);

				/*-----------------------------------------------------------------------------------------*/
				std::map<std::string, EType>::iterator iter_1 = m_oVariable_name_and_type.find(temp_operator_stack.top());
				temp_operator_stack.pop();
				//find  and  reserve  GPR
				E_GPR reg = find_empty_GPR_in_input_type(iter_1->second);
				change_GPR_state(reg);
				//generate  code  part  from two operands
				generate_code(E_GPR::R0, reg , temp_operator_stack.top(), code_line_number,
					temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
				//reserve R0 register
				m_oGPR_name_and_state.find(E_GPR::R0)->second = true;
			}
			else
				throw C_Compile_Error("UNDEFINDE  VARIABLE  OR  FUNCTION IN  LINE  ", code_line_number);
		}
		else  if (is_variable(m_oVariable_Operators_and_Operands.top()))
		{
			if (is_operator(temp_operator_stack.top()))
			{
				temp_operator_stack.push(m_oVariable_Operators_and_Operands.top());
				m_oVariable_Operators_and_Operands.pop();
			}
			else  if (is_number(temp_operator_stack.top()))
			{
				std::map<std::string, EType>::iterator iter = m_oVariable_name_and_type.find(m_oVariable_Operators_and_Operands.top());
				m_oVariable_Operators_and_Operands.pop();
				//cast  string  to  int
				int i = std::atoi(temp_operator_stack.top().c_str());
				//find  and  rezerve   GPR 1
				E_GPR reg_1 = find_empty_GPR_in_input_type(get_type(i));
				change_GPR_state(reg_1);
				//find  and  rezerve  GPR 2
				E_GPR reg_2 = find_empty_GPR_in_input_type(iter->second);
				temp_operator_stack.pop();
				change_GPR_state(reg_2);
				/*--------------------------------------------------------------------------------*/
				//generate  from variable  work
				temp_code_stream << "ASSIGN " << get_typename(get_type(i)) << " R" << reg_1 << " , " << i << std::endl;
				temp_code_stream << "ASSIGN  A" << find_empty_AR() << " , " << iter->first << std::endl;
				temp_code_stream << "LOAD R" << reg_2 << " , A" << find_empty_AR() << std::endl;
				//generate  working  code  part
				generate_code(reg_2, reg_1, temp_operator_stack.top(), code_line_number,
					temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
			}
			else  if (is_function(temp_operator_stack.top()))
			{
				std::map<std::string, EType>::iterator iter_1 = m_oVariable_name_and_type.find(m_oVariable_Operators_and_Operands.top());
				m_oVariable_Operators_and_Operands.pop();
				std::map<std::string, EType>::iterator iter_2 = m_oVariable_name_and_type.find(temp_operator_stack.top());
				std::string temp_2 = temp_operator_stack.top();
				temp_operator_stack.pop();
				//if this  function undefinde  throw  exeption
				if (iter_2 == m_oVariable_name_and_type.end())
					throw C_Compile_Error("UNDEFINDE  FUNCTION IN LINE ", code_line_number);
				//generate  function  caal  code  
				generate_function_call_part_code(temp_2, temp_code_stream);
				//find  and  change  empty  GPR
				E_GPR reg = find_empty_GPR_in_input_type(iter_1->second);
				change_GPR_state(reg);
				/*------------------------------------------------------------------------------------------*/
				//generate  working  code
				generate_code(reg, E_GPR::R0, temp_operator_stack.top(), code_line_number,
					temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
			}
			else  if (is_variable(temp_operator_stack.top()))
			{
				//get  iterators  to  variables
				std::map<std::string, EType>::iterator iter_1 = m_oVariable_name_and_type.find(m_oVariable_Operators_and_Operands.top());
				m_oVariable_Operators_and_Operands.pop();
				std::map<std::string, EType>::iterator iter_2 = m_oVariable_name_and_type.find(temp_operator_stack.top());
				temp_operator_stack.pop();
				//reserving   registers  from  work
				E_GPR reg = find_empty_GPR_in_input_type(iter_1->second);
				change_GPR_state(reg);
				E_GPR reg_2 = find_empty_GPR_in_input_type(iter_2->second);
				change_GPR_state(reg_2);
				/*----------------------------------------------------------------------------------------*/
				//generate  code   working  code  from  two  variables
				generate_code(reg, reg_2, temp_operator_stack.top(), code_line_number,
					temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
			}
			else
				throw C_Compile_Error("UNDEFINDE VARIABLE OR  FUNKTION  IN LINE  ", code_line_number);
		}
		else  if (m_oVariable_Operators_and_Operands.size() == 1 && temp_operator_stack.size() == 2 && temp_GPR_stack.empty())
		{
			if (is_number(temp_operator_stack.top()))
			{
				//cast  string from  number  
				int i = std::atoi(temp_operator_stack.top().c_str());
				//reserving  register  from  work
				E_GPR reg = find_empty_GPR_in_input_type(get_type(i));
				//set new  variable  in  variables  map
				m_oVariable_name_and_type.insert(std::make_pair(m_oVariable_Operators_and_Operands.top(), get_type(i)));
				// get  numbers  type  
				get_data_type(i, temp_data_stream);
				temp_data_stream << m_oVariable_Operators_and_Operands.top() << std::endl;
				temp_operator_stack.pop();
				//generate   working  code  
				temp_code_stream << "ASSIGN " << get_typename(get_type(i)) << " R" << reg << " , " << i << std::endl;
				temp_code_stream << "ASSIGN  A" << find_empty_AR() << " , " << m_oVariable_Operators_and_Operands.top() << std::endl;
				temp_code_stream << "STORE  A" << find_empty_AR() << " , R" << reg << std::endl;
				//write  temp  code  stream code  in  variable  stream  
				m_oTemp_variable_stream << temp_code_stream.str() << std::endl;
				m_oVariable_Operators_and_Operands.pop();
				//change  ADDRESS  and  GP registers  state  
				change_AR_state(find_empty_AR());
				change_GPR_state(reg);
			}
			else if (is_function(temp_operator_stack.top()))
			{
				std::map<std::string, EType>::iterator iter = m_oVariable_name_and_type.find(temp_operator_stack.top());
				
				//if  we  has'nt  such  function  throw  execption
				if (iter == m_oVariable_name_and_type.end())
					throw C_Compile_Error("UNDEFINDE FUNCTION  IN  LINE  ", code_line_number);

				E_GPR reg = find_empty_GPR_in_input_type(iter->second);
				// set  new  variable   in variable map
				m_oVariable_name_and_type.insert(std::make_pair(m_oVariable_Operators_and_Operands.top(), iter->second));
				//get  variable  data  type 
				get_data_type(iter->second, temp_code_stream);
				temp_data_stream << m_oVariable_Operators_and_Operands.top() << std::endl;
				// generate  function  call  code  part
				generate_function_call_part_code(temp_operator_stack.top(), temp_code_stream);
				//read  variable address  and  store  value
				temp_code_stream << "ASSIGN  A" << find_empty_AR() << " , " << m_oVariable_Operators_and_Operands.top() << std::endl;
				temp_code_stream << "STORE  A" << find_empty_AR() << " , R0" << reg << std::endl;
				//release  the  working  stack's
				m_oVariable_Operators_and_Operands.pop();
				temp_operator_stack.pop();
				//change  ADDRESS  and  GP registers  state
				change_AR_state(find_empty_AR());
				change_GPR_state(reg);
			}
			else
			{
				std::map<std::string, EType>::iterator iter = m_oVariable_name_and_type.find(temp_operator_stack.top());
				//if  we  has'nt such variable  throw  exeption
				if (iter == m_oVariable_name_and_type.end())
					throw C_Compile_Error("UNDEFINDE VARIABLE IN LINE ", code_line_number);
				//get  empty  register  from work
				E_GPR reg = find_empty_GPR_in_input_type(iter->second);

				//read  variable  value  in  memory
				temp_code_stream << "ASSIGN  A" << find_empty_AR() << " , " << temp_operator_stack.top() << std::endl;
				temp_code_stream << "LOAD  R" << reg << " , A" << find_empty_AR() << std::endl;
				//change  ADDRESS  and  GP  registers  state  from work
				change_AR_state(find_empty_AR());
				change_GPR_state(reg);

				//end  task
			}
		}
		else
		{
			//if  in  temp_operators_stream  we  have  a  objects  then  we worked  of  this  variable
			while (temp_operator_stack.size() != 0)
			{
				// read  temp register in GPR_stack and read  object in temp_operator_stack
				E_GPR reg = temp_GPR_stack.top();
				std::string obj = temp_operator_stack.top();
				//change  GPR  state  from work
				change_GPR_state(reg);
				//release thate  objects
				temp_GPR_stack.pop();
				temp_operator_stack.pop();
				
				// get  operator  from working to  the  objects
				std::string oper = temp_operator_stack.top();
				// get  value second  object
				int i = std::atoi(obj.c_str());
				E_GPR reg_2 = find_empty_GPR_in_input_type(get_type(i));
				change_GPR_state(reg_2);
				//generate  part  working  code
				temp_code_stream << "ASSIGN " << get_typename(get_type(i)) << " R" << reg_2 << " , " << i << std::endl;
				//generate   working  code 
				generate_code(reg, reg_2, oper, code_line_number,
					temp_code_stream, temp_data_stream, temp_GPR_stack, temp_operator_stack);
			}
			//base case  
			//end  task
		}
	}

	//write  in   Data section stream   
	m_oData_section << temp_data_stream.str() ;
	//GPR  and  AR  state = false
	update_GPA_and_AR();

	// clear  streams
	temp_code_stream.clear();
	temp_data_stream.clear();
}


//returned   the  operand  is  funktion or  not
bool  C_Parser::is_function(std::string& operand) const _NOEXCEPT
{
	if (operand.length() >= 3)
	{
		if (operand[operand.length() - 1] == ')' && operand[operand.length() - 2] == '(')
			return  true;
	}
	return false;
}


// generate .code  section code 
void  C_Parser::generate_code_section_code()
{
	// jump  to  main 
	m_oCode_section << "JUMP  START " << std::endl;
	m_oCode_section << "EXIT" << std::endl;
	// write  function part
	m_oCode_section << m_oTemp_function_stream.str() << std::endl;

	m_oCode_section << "START:" << std::endl;
	m_oCode_section << "PUSH A1 " << std::endl << "MOVE A0 , A1 " << std::endl;
	m_oCode_section << m_oTemp_variable_stream.str() << std::endl;
	m_oCode_section << "POP A1" << std::endl << "RET" << std::endl;
}

//changet   the  GPR  state 
void  C_Parser::change_GPR_state(E_GPR GPR_name)
{
	std::map<E_GPR, bool>::iterator iter = m_oGPR_name_and_state.find(GPR_name);
	iter->second = !iter->second;
}


//change  the  Address   register  state
void  C_Parser::change_AR_state(E_AR reg) _NOEXCEPT
{
	std::map<E_AR,bool>::iterator iter = m_oAddress_register_and_state.find(reg);
	iter->second = !iter->second;
}

//returned  the  operand  is  variable  or  not
bool  C_Parser::is_variable(const std::string& code_part) const
{
	std::map<std::string, EType>::const_iterator iter = m_oVariable_name_and_type.find(code_part);
	if (iter == m_oVariable_name_and_type.end())
		return false;
	return true;
}


//returnde   the  object  is operator  or  not
bool  C_Parser::is_operator(std::string& object) const _NOEXCEPT
{
	if (*(object.c_str()) == E_Operators::plus)
		return true;
	else  if (*(object.c_str()) == E_Operators::minus)
		return true;
	else  if (*(object.c_str()) == E_Operators::division)
		return true;
	else  if (*(object.c_str()) == E_Operators::multiplication)
		return true;
	else  if (*(object.c_str()) == E_Operators::equal)
		return true;
	return  false;
}



E_GPR C_Parser::find_empty_GPR_in_input_type(EType type) const _NOEXCEPT
{
	int variable_type;
	// get the number of bytes  the variable type
	switch (type)
	{
	case EType::BYTE:  variable_type = 1; break;
	case EType::WORD:  variable_type = 2; break;
	case EType::DWORD: variable_type = 4; break;
	case EType::QWORD: variable_type = 8; break;
	default:           variable_type = 4; break;
	}
	//itrators  begin and  end
	std::map<E_GPR, bool>::const_iterator iter_begin = m_oGPR_name_and_state.begin();
	std::map<E_GPR, bool>::const_iterator iter_end = m_oGPR_name_and_state.end();

	int n_r = variable_type;

	//get  first   empty  GPR in  type  
	for (; iter_begin != iter_end;)
	{
		if (iter_begin->first % variable_type == 0 && iter_begin->second == false)
		{
			return iter_begin->first;
		}
		//forward   iterator  to  n gives  
		while (n_r != 0)
		{
			++iter_begin;
			--n_r;
		}
		n_r = variable_type;
	}

	// if  dont have  this  type  GPR  return undefinde
	return E_GPR::undefinde;
}


//returnde  type  name  in  string 
const std::string  C_Parser::get_typename(EType type) const _NOEXCEPT
{
	switch (type)
	{
	case EType::BYTE:  return " B ";
	case EType::WORD:  return " W ";
	case EType::DWORD: return " DW ";
	case EType::QWORD: return " QW ";
	default: return " DW ";
	}
}


void  C_Parser::initialize_Data_and_Code_sections()
{
	m_oData_section << ".data" << std::endl;
	m_oCode_section << ".code" << std::endl;
}


//get   register  type  
EType C_Parser::get_reg_type(int reg_number) const _NOEXCEPT
{
	if (reg_number % 8 == 0)
		return EType::QWORD;
	if (reg_number % 2 == 0 && reg_number % 4 != 0)
		return EType::WORD;
	if (reg_number % 4)
		return EType::DWORD;
	return EType::BYTE;
}

//generate  output  code  in  stream
void  C_Parser::generat_code_output()
{
	generate_code_section_code();
	IO_Device_Class::sm_oWrite_output_file_straem << m_oData_section.str() << std::endl << m_oCode_section.str() << std::endl;
}


// get  data  type  and  write  to  data  stream
void  C_Parser::get_data_type(int argument, std::stringstream& temp_data_stream)
{
	switch (get_type(argument))
	{
	case EType::BYTE:  temp_data_stream << "BYTE  ";  break;
	case EType::WORD:  temp_data_stream << "WORD  ";  break;
	case EType::DWORD: temp_data_stream << "DWORD  "; break;
	case EType::QWORD: temp_data_stream << "QWORD  "; break;
	default:           temp_data_stream << "DWORD  "; break;
	}
}

//find  empty  Address  register
E_AR  C_Parser::find_empty_AR() const _NOEXCEPT
{
	std::map<E_AR, bool>::const_iterator iter_begin = m_oAddress_register_and_state.begin();
	std::map<E_AR, bool>::const_iterator iter_end = m_oAddress_register_and_state.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		if (iter_begin->second == false)
			return iter_begin->first;
	}
}


// update  GPR  and  Address  register   state
void  C_Parser::update_GPA_and_AR() _NOEXCEPT
{
	//update  AR state  in false
	for(auto&  element:m_oAddress_register_and_state)
	{
		 element.second = false;
	}
	//rezering  BASE ptr  and  STACK ptr  in  ADDRESS  registers
	m_oAddress_register_and_state.find(E_AR::SF)->second = true;
	m_oAddress_register_and_state.find(E_AR::SP)->second = true;

	//update GPR state  in false 
	for (auto& element : m_oGPR_name_and_state)
	{
		element.second = false;
	}
	//reseving R0 register  from  return value  in  functions
	m_oGPR_name_and_state.find(E_GPR::R0)->second = true;
}


// generate  assambler   code   
void  C_Parser::generate_code(E_GPR left, E_GPR right, std::string& operator_1, int line_number,
	std::stringstream& temp_code_stream, std::stringstream& temp_data_stream, std::stack<E_GPR>& temp_GPR_stack,
	std::stack<std::string>&temp_operator_stack)
{
	E_GPR reg_3 = left > right ? left : right;
	E_GPR reg_4 = left < right ? left : right;
	//generating  code  from  2  operands  and  hes  operators 
	if (operator_1 == "+")
		temp_code_stream << "ADD " << get_typename(get_reg_type(reg_3)) << " R" << left << " , R" << right << " , R" << reg_3 << std::endl;
	else if (operator_1 == "-")
		temp_code_stream << "SUB " << get_typename(get_reg_type(reg_3)) << " R" << left << " , R" << right << " , R" << reg_3 << std::endl;
	else if (operator_1 == "*")
		temp_code_stream << "MUL " << get_typename(get_reg_type(reg_3)) << " R" << left << " , R" << right << " , R" << reg_3 << std::endl;
	else if (operator_1 == "/")
		temp_code_stream << "DIV " << get_typename(get_reg_type(reg_3)) << " R" << left << " , R" << right << " , R" << reg_3 << std::endl;
	// end code  generations 
	//change  GPR  state  from new work
	change_GPR_state(reg_4);
	temp_operator_stack.pop();
	temp_GPR_stack.push(reg_3);

	//genetting   code  vrom  2  temporary  variable  
	if (temp_GPR_stack.size() == 2)
	{
		//get   register  wich have  temp  values 
		E_GPR reg_2 = temp_GPR_stack.top();
		temp_GPR_stack.pop();
		E_GPR reg_1 = temp_GPR_stack.top();
		temp_GPR_stack.pop();
		//call   code  generators  function 
		generate_code(reg_2, reg_1, temp_operator_stack.top(), line_number, temp_code_stream, temp_data_stream,
			temp_GPR_stack, temp_operator_stack);
	}	//base  case  from  parsing  code
	else if (m_oVariable_Operators_and_Operands.size() == 1 && temp_GPR_stack.size() == 1 && temp_operator_stack.size() == 1)
	{	
		//generate  new  funcion  or  variable
		if (!is_variable(m_oVariable_Operators_and_Operands.top()) && is_function(m_oVariable_Operators_and_Operands.top()))
		{
			E_GPR reg = temp_GPR_stack.top();
			//set  new  function in  variable  map
			m_oVariable_name_and_type.insert(std::make_pair(m_oVariable_Operators_and_Operands.top(), get_reg_type(reg)));
			/*----------------------------------------------------------------------------------------------------------------*/
			//change  General pourpous register  state
			change_GPR_state(reg);

			std::string temp_str;
			std::string temp_s = temp_code_stream.str();		// copy code  in  new  string 
			temp_code_stream.str(std::string());				// clear  code stream in "ERROR  STATE"

			// get  function  name in  new  string
			int i = 0;
			while (m_oVariable_Operators_and_Operands.top()[i] != '(')
			{
				temp_str += m_oVariable_Operators_and_Operands.top()[i];
				++i;
			}
			//generate  function  body
			temp_code_stream << temp_str << ":" << std::endl;
			temp_code_stream << "PUSH A1 " << std::endl;
			temp_code_stream << "MOVE A0 , A1 " << std::endl;
			temp_code_stream << temp_s ;
			temp_code_stream << "MOVE R0 , R" << reg << std::endl;
			temp_code_stream << "POP A1" << std::endl;
			temp_code_stream << "RET" << std::endl;
			// end  function body  

			//set  function body  to  function temp  stream 
			m_oTemp_function_stream << temp_code_stream.str() << std::endl;
			//set function call  code  part  in  function strar
			generate_function_call_part_code(m_oVariable_Operators_and_Operands.top(), m_oTemp_variable_stream);

			m_oVariable_Operators_and_Operands.pop();
			change_GPR_state(reg);
			temp_operator_stack.pop();
			temp_GPR_stack.pop();
			//end  task
		}
		else  if (!is_function(m_oVariable_Operators_and_Operands.top()) && !is_variable(m_oVariable_Operators_and_Operands.top()))
		{
			E_GPR reg = temp_GPR_stack.top();
			
			// generate  code   from type in  ths  data 
			get_data_type(get_reg_type(reg), temp_data_stream);
			//set  new  variable  in   variable  map
			m_oVariable_name_and_type.insert(std::make_pair(m_oVariable_Operators_and_Operands.top(), get_reg_type(reg)));
			/*----------------------------------------------------------------------------------------------------------------*/
			//generate   new  variable  code  
			temp_data_stream << m_oVariable_Operators_and_Operands.top() << std::endl;
			temp_code_stream << "ASSIGN  A" << find_empty_AR() << " , " << m_oVariable_Operators_and_Operands.top() << std::endl;
			temp_code_stream << "STORE  A" << find_empty_AR() << " , R" << reg << std::endl;
			// end   variable  code  

			//set  variable  to  variable  stream  
			m_oTemp_variable_stream << temp_code_stream.str() << std::endl;

			m_oVariable_Operators_and_Operands.pop();
			//change  ADDREDS  and  GP  registers  state  from new  work
			change_AR_state(find_empty_AR());
			change_GPR_state(reg);
			//
			temp_operator_stack.pop();
			temp_GPR_stack.pop();

			//end  task
		}
		else
			throw C_Compile_Error("UNDEFINDE  VARIABLE  OR  FUNCTION  IN  LINE ", line_number);
	}
}

// get  call  code  part   
void C_Parser::generate_function_call_part_code(std::string& temp, std::stringstream& temp_code_stream) _NOEXCEPT
{
	int i = 0;
	std::string temp_str;
	//get  funtion  name  
	while (temp[i] != '(')
	{
		temp_str += temp[i];
		++i;
	}
	//generate  function  call  code  
	temp_code_stream << "ASSIGN  A" << find_empty_AR() << " , " << temp_str << std::endl;
	temp_code_stream << "CALL  A" << find_empty_AR() << std::endl;
	//change   state ADDRESS  REGISTER
	change_AR_state(find_empty_AR());
	//Reserved  R0  GPR  from  return value 
	m_oGPR_name_and_state.find(E_GPR::R0)->second = true;
}
