#include "C_Compile_Error.h"
#include <iostream>
#include <string>




//ctor by  default
C_Compile_Error::C_Compile_Error()
{
}

//virtual ~ctor
C_Compile_Error::~C_Compile_Error()
{
}

//copy  ctor  from   error  message  class
C_Compile_Error::C_Compile_Error(const C_Compile_Error& Error_object)
	:m_oError_message(Error_object.m_oError_message),m_oError_line(Error_object.m_oError_line)
{
}

//initialize  Error_message
C_Compile_Error::C_Compile_Error(const char * error_message , int line_number)
	:m_oError_message(error_message),m_oError_line(line_number)
{
}


//initialize  Error_message
C_Compile_Error::C_Compile_Error(const char * error_message)
	:m_oError_message(error_message)
{
}

//Show  Error  mesage
void  C_Compile_Error::show_error_message()  const  
{
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl; 
	std::cout << std::endl;

	if (m_oError_line != 0)
		std::cout << m_oError_message << std::to_string(m_oError_line) << std::endl;
	else
		std::cout << m_oError_message << std::endl;

	std::cout << std::endl;
	std::cout << std::endl;
}

// set  Error  message  and  Error
void C_Compile_Error::setError_message(const char * error_message ,  int line_number)
{
	m_oError_line = line_number;
	m_oError_message = error_message;
}

//set  Error  message
void C_Compile_Error::setError_message(const char * error_message)
{
	m_oError_message = error_message;
}