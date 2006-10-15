#ifndef FP_LOAD_XML_TOOL_V1
#define FP_LOAD_XML_TOOL_V1

#include <vector>
#include <string.h>
#include <stdio.h>

struct FPxitem
{
	std::string				m_Name;		// Name in <>
	std::string				m_Data;		// Data
	std::vector<FPxitem>	m_Children;		// Children
};

class FPxml
{
public:
	FPxml();
	~FPxml();

	void loadFile(const char* _filename);
	FPxitem m_base;

private:
	bool loadElment(FPxitem& _item);

	//------------------------------------
	//	Varibels
	//------------------------------------
	char* m_cursor;
	char* m_buffer;
};

#endif