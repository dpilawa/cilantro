#include "graphics/ShaderProcessor.h"
#include "system/LogMessage.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace cilantro {

ShaderProcessor::ShaderProcessor (const std::unordered_map<std::string, std::string>& globals)
    : m_globals(globals)
{
}

ShaderProcessor::~ShaderProcessor ()
{
}


std::string ShaderProcessor::ProcessShader (const std::string& filename) 
{
    std::ifstream file (filename, std::ios::binary);
    std::ostringstream output;
    char ch;

    if (m_includedFiles.find (filename) != m_includedFiles.end ()) 
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Circular include for file:" << filename;
    }

    m_includedFiles.insert (filename);

    if (!file.is_open ()) 
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Cannot open:" << filename;
    }

    while (file.get (ch)) 
    {
        if (ch == '%' && file.peek () == '%') // processor directive
        {
            file.get(ch); 

            char c1, c2;
            bool closed = false;
            std::string directive;
            std::string keyword;

            while (file.get(c1)) 
            {
                if (c1 == '%' && file.peek () == '%') 
                {
                    file.get (c2); // consume second '%'
                    closed = true;
                    break;
                }
                directive += c1;
            }

            if (!closed) {
                LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unterminated directive (missing closing %% ) in file:" << filename;
            }

            Trim (directive);
            std::istringstream ds (directive);
            ds >> keyword;

            if (keyword == "include") 
            {
                std::string includeFile;
                if (!(ds >> includeFile)) 
                {
                    LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Expected filename after include keyword in file:" << filename;
                }
                output << ProcessShader (includeFile);
            } 
            else 
            {
                auto it = m_globals.find (keyword);
                if (it != m_globals.end()) 
                {
                    output << it->second;
                } 
                else 
                {
                    LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unknown global" << keyword << "in file:" << filename;
                }
            }
        } 
        else // simply copy the character
        {
            output << ch;
        }
    }

    return output.str();
}

void ShaderProcessor::Reset () 
{
    m_includedFiles.clear ();
}

void ShaderProcessor::Trim (std::string& str) 
{
    size_t first = str.find_first_not_of (" \t\r\n");
    size_t last = str.find_last_not_of (" \t\r\n");
    if (first == std::string::npos || last == std::string::npos) 
    {
        str.clear ();
    } 
    else 
    {
        str = str.substr (first, last - first + 1);
    }
}

}
