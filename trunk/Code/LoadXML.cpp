#include "LoadXML.h"

#define FPSTRCHR(str,token) \
	str = strchr(m_cursor,token); \
	if(str == 0) return false;
//=================================================================
//	Create blank
//=================================================================
FPxml::FPxml()
{
	m_buffer = 0;
	m_cursor = 0;
}
//=================================================================
//	Delete Data
//=================================================================
FPxml::~FPxml()
{
	if(m_buffer)
		delete m_buffer;
}

//=================================================================
//	Load File
//=================================================================
void FPxml::loadFile(const char* _filename)
{
	FILE* _file = fopen(_filename,"rt");
	if (_file!=NULL)
	{
		//--------------------------
		//	Measure File
		//--------------------------
		fseek(_file,0, SEEK_END);
		long _fSize = ftell(_file);
		rewind(_file);

		//--------------------------
		//	Allocate and Copy
		//--------------------------
		m_buffer = new char[_fSize];
		memset(m_buffer,0,(_fSize)*sizeof(char));
		m_cursor = m_buffer;

		//--------------------------
		//	Copy Loop
		//	Sanitizes string
		//--------------------------
		while(m_cursor[0] != EOF)
		{
			m_cursor[0] = fgetc(_file);
			if((m_cursor[0] > 31) && (m_cursor[0] < 127))
				++m_cursor;
		}
		m_cursor[0] = 0;
		
		//--------------------------
		//	Load Data
		//--------------------------
		m_cursor = m_buffer;
		loadElment(m_base);

		//--------------------------
		//	Terminate
		//--------------------------		
		fclose(_file);
		delete[] m_buffer;
		m_buffer = 0;
	}
}
//=================================================================
//	Recursive Element Loader
//=================================================================
bool FPxml::loadElment(FPxitem& _item)
{
	char* _end = 0;

	//-------------------------------
	//	Extract Name <..>
	//-------------------------------
	m_cursor = strchr(m_cursor,'<') + 1;	if(m_cursor == 0) return false;
	FPSTRCHR(_end,'>')
	if(m_cursor[0] == '/')
	{
		m_cursor = strchr(m_cursor,'>') + 1;
		return true;
	}
	_end[0] = 0;

	_item.m_Name = m_cursor;

	m_cursor = _end + 1;
	//-------------------------------
	//	Extract Data >..<
	//-------------------------------
	FPSTRCHR(_end,'<')
	if(m_cursor != _end)
	{
		_end[0] = 0;
        _item.m_Data = m_cursor;
		_end[0] = '<';
	}

	m_cursor = _end; 
	//-------------------------------
	//	Check for </
	//-------------------------------
	if(m_cursor[1] == '/')
	{
		m_cursor = strchr(m_cursor,'>') + 1;
		return true;
	}

	while(true)
	{
		_item.m_Children.push_back(FPxitem());
		if(loadElment(_item.m_Children.back()))
		{
			FPSTRCHR(_end,'<')
			_end[0] = 0;
			_item.m_Data += m_cursor;
			_end[0] = '<';
			m_cursor = _end; 
			//-------------------------------
			//	Check for </
			//-------------------------------
			if(m_cursor[1] == '/')
			{
				m_cursor = strchr(m_cursor,'>') + 1;
				return true;
			}
		}
		else return false;
	}
}