#define _CRT_SECURE_NO_WARNINGS
#include "IO_Device_Class.h"
#include <iostream>
#include <cstring>


IO_Device_Class::IO_Device_Class()      //ctor by  default
{
}

IO_Device_Class::~IO_Device_Class()		//~ctor  by  default
{
}

IO_Device_Class::IO_Device_Class(const IO_Device_Class&)    //copy  ctor
{
}

//input  compiling  file  path  
void IO_Device_Class::initializer_input_output_file(const std::string& input_file_path, const std::string& out_file_path)
{
	m_oInput_file_path_name = input_file_path;
	m_oOutput_file_path_name = out_file_path;
}

//read  input file  to  stringstream
void IO_Device_Class::Read_from_file()
{
	std::string t_oRead_line;
	std::fstream t_oInput_file;
	t_oInput_file.open(m_oInput_file_path_name, std::ios::in);

	//if  this  file is  not  open  or  invalid returned false
	if (!t_oInput_file.is_open())
		throw C_Compile_Error(" CAN  NOT  OPEN INPUT FILE ");

	//read  file
	while (!t_oInput_file.eof())
	{
		std::getline(t_oInput_file, t_oRead_line);      
		m_oInput_file_stream << t_oRead_line;
	}
	//read  from  file  is  completed

	t_oInput_file.close();  //close  input  file         
}



//wiret  assambly  code  in  to  output file
void IO_Device_Class::Write_to_out_file()																						
{
	std::fstream temp_out_file;
	std::string temp_str = sm_oWrite_output_file_straem.str();
	temp_out_file.open(m_oOutput_file_path_name, std::ios::out);

	//if this  file  in not  open returned false
	if (!temp_out_file.is_open())
		throw C_Compile_Error(" OUTPUT FILE CAN  NOT  OPEN ");

	//write to  out file
	temp_out_file << temp_str;
	temp_out_file.close();
	std::cout << "COMPILE  IS  COMPLETED , HAS  ALL  REDY  OUTPUT  FILE  " << m_oOutput_file_path_name << std::endl;
	//write  in  file  is  completed  
}


//returnd   parsed  line 
std::string IO_Device_Class::get_parsed_code()  const
{
	return   m_oInput_file_stream.str();
}

std::stringstream IO_Device_Class::sm_oWrite_output_file_straem;