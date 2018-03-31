#pragma once
#include "C_Compile_Error.h"
#include <fstream>
#include <sstream>
#include <queue>


class IO_Device_Class
{
private:

	std::string				m_oOutput_file_path_name;
	std::string				m_oInput_file_path_name;
	std::stringstream		m_oInput_file_stream;


public:
	IO_Device_Class();
	IO_Device_Class(const IO_Device_Class&);
	virtual ~IO_Device_Class();

	static 	std::stringstream	sm_oWrite_output_file_straem;

	void  Read_from_file();
	void  Write_to_out_file();
	void  initializer_input_output_file(const std::string&, const std::string&);
	std::string get_parsed_code() const;
};

