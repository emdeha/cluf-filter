#include "stdafx.h"
#include "XMLReader.h"

#include "XMLDoc.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <locale>
#include <cctype>
#include <vector>


///////////////////////
// Utility Functions //
///////////////////////
bool IsPostfixOf(const std::string &substr, const std::string &str);

bool IsFileNameValid(const std::string &xmlFileName);
// Removes last part of an xmlPath.
// For example, "root/tooth/quux" will be transformed into "root/tooth".
// "root" will be transformed into "".
void RemoveLastPartOfPath(std::string &path);
// Appends a child to the path.
void AppendPath(std::string &path, const std::string &toAppend);

std::string GetLastPartOfPath(const std::string &path);

void AddTagWithPath(XMLDoc *doc, XMLTag &currentTag, std::string &accPath, 
					std::ifstream &xmlFile, char &ch);


void ReadUntil(int (*pred)(int), std::ifstream &file, char &ch);
std::string ReadUntilAccum(int (*pred)(int), std::ifstream &file, char &ch);

int istagbeg(int ch);
int istagend(int ch);
int isemptytag(int ch);
int iseq(int ch);
int isquot(int ch);
int isspaceORtagend(int ch);


///////////////////////////
// Reader Implementation //
///////////////////////////
bool XMLReader::ReadInto(XMLDoc *doc, const std::string &inputFile)
{
	if ( ! IsFileNameValid(inputFile))
	{
		return false;
	}


	std::ifstream xmlFile(inputFile);
	if (xmlFile.is_open())
	{
		std::string accPath;
		bool result = this->ReadIntoH(doc, accPath, xmlFile);

		xmlFile.close();
		return result;
	}
	else return false;
}

bool XMLReader::ReadIntoH(XMLDoc *doc, std::string &accPath, std::ifstream &xmlFile)
{
	char ch;
	XMLTag currentTag;
	while (xmlFile >> std::noskipws >> ch)
	{
		if (isspace(ch))
		{
			continue;
		}

		// Parse a new tag.
		if (istagbeg(ch))
		{
			xmlFile >> ch;
			if (ch == '?')
			{
				ReadUntil(istagend, xmlFile, ch);
				continue;
			}
			else if (isalnum(ch))
			{
				std::string tagName = "";
				tagName += ch;
				tagName +=
					ReadUntilAccum([](int ch) { return (int)(isspace(ch) || 
															 istagend(ch) ||
															 isemptytag(ch));
											  },
								   xmlFile, ch);
				AppendPath(accPath, tagName);
				while ( ! istagend(ch))
				{
					if (isspace(ch))
					{
						ReadUntil(isalnum, xmlFile, ch);
						std::string key = "";
						key += ch;
						key += ReadUntilAccum(iseq, xmlFile, ch);
						ReadUntil(isquot, xmlFile, ch);
						std::string val = ReadUntilAccum(isquot, xmlFile, ch);
						currentTag.AddAttrib(key, val);
						
						xmlFile >> std::noskipws >> ch;
					}

					if (isemptytag(ch))
					{
						AddTagWithPath(doc, currentTag, accPath, xmlFile, ch);
						return true;
					}
				}
			}
			else 
			{
				std::cerr << "Invalid character in xml doc\n";
				return false;
			}
		}

		ch = xmlFile.peek();

		// Have data.
		if (isalnum(ch))
		{
			xmlFile >> ch;
			std::string data;
			data += ch;
			data += ReadUntilAccum(istagbeg, xmlFile, ch);
			currentTag.AddData(data);

			AddTagWithPath(doc, currentTag, accPath, xmlFile, ch);
			ReadUntil(isspace, xmlFile, ch);
		}
		else if (istagbeg(ch))
		{
			// Begin parsing subtree
			AddTagWithPath(doc, currentTag, accPath, xmlFile, ch);
			return ReadIntoH(doc, accPath, xmlFile);
		}
	}

	return true;
}


const char *xmlExt = ".xml";

bool IsPostfixOf(const std::string &substr, const std::string &str)
{
	size_t substrLen = substr.length();
	if (str.length() > substrLen)
	{
		return str.compare(str.length() - substrLen, substrLen, substr) == 0;
	}
	else return false;
}

///////////////////////////////////////////////////////////////////////////////

bool IsFileNameValid(const std::string &xmlFileName)
{
	return IsPostfixOf(xmlExt, xmlFileName);
}

void RemoveLastPartOfPath(std::string &path)
{
	if (path.empty())
	{
		return;
	}

	size_t lastSlashPos = path.find_last_of('/');
	if (lastSlashPos != std::string::npos)
	{
		path.erase(lastSlashPos);
	}
	else
	{
		// We're at the parentest parent
		path = "";
	}
}

void AppendPath(std::string &path, const std::string &toAppend)
{
	path += "/";
	path += toAppend;
}

std::string GetLastPartOfPath(const std::string &path)
{
	size_t lastSlashPos = path.find_last_of('/');
	if (lastSlashPos != std::string::npos)
	{
		return std::string(path.begin() + lastSlashPos, path.end());
	}
	else
	{
		return "";
	}
}

void AddTagWithPath(XMLDoc *doc, XMLTag &currentTag, std::string &accPath, 
					std::ifstream &xmlFile, char &ch)
{
	doc->AddTag(accPath, currentTag);
	currentTag.Clear();
	RemoveLastPartOfPath(accPath);
	ReadUntil(istagend, xmlFile, ch);
}


void ReadUntil(int (*pred)(int), std::ifstream &file, char &ch)
{
	do
	{
		file >> std::noskipws >> ch;
	}
	while ( ! pred(ch));
}

std::string ReadUntilAccum(int (*pred)(int), std::ifstream &file, char &ch)
{
	std::string res = "";
	file >> std::noskipws >> ch;
	while ( ! pred(ch))
	{
		res += ch;
		file >> std::noskipws >> ch;
	}

	return res;
}


int istagbeg(int ch)
{
	return ch == '<';
}

int istagend(int ch)
{
	return ch == '>';
}

int isemptytag(int ch)
{
	return ch == '/';
}

int iseq(int ch)
{
	return ch == '=';
}

int isquot(int ch)
{
	return ch == '\"';
}

int isspaceORtagend(int ch)
{
	return isspace(ch) || istagend(ch);
}