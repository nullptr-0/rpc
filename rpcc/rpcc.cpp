#include <iostream>
#include <regex>
#include <fstream>

using ArgInf = std::pair<std::string, std::string>;

struct FuncInf
{
    std::string funcName;
    std::string retType;
    std::vector<ArgInf> argList;
};

int main(int argc, char* argv[])
{
    std::cout << "rpcc: generate code for rpc from given functions" << std::endl;
    std::string inputPath;
    if (argc == 1)
    {
        std::cout << "input path to source file:" << std::endl;
        std::cin >> inputPath;
        std::cin.get();
    }
    else if (argc == 2)
    {
        inputPath = argv[1];
    }
    else if (argc > 2)
    {
        std::cerr << "rpcc: too many parameters" << std::endl;
        return 2;
    }
    std::fstream inputCode(inputPath, std::ios::in);
    if (!inputCode.is_open())
    {
        std::cerr << "rpcc: unable to open " << inputPath << std::endl;
        return 3;
    }
    std::string userCode;
    std::vector<FuncInf> funcList;
    const std::regex funcDefRegEx{ "(.*?)(((?:(?:const|volatile|mutable|static|extern|inline|virtual|explicit|friend)\\s+)*(?:\\w+::)*(?:\\w+<.*?>|\\w+)\\s*(?:\\*|\\&|\\&\\&|\\[\\d*\\])*)\\s+([A-Za-z_]\\w*)\\s*\\(((?:(?:(?:(?:const|volatile|mutable|static|extern|inline|virtual|explicit|friend)\\s+)*(?:\\w+::)*(?:\\w+<.*?>|\\w+)\\s*(?:\\*|\\&|\\&\\&|\\[\\d*\\])*)\\s+(?:[A-Za-z_]\\w*)(?:,\\s*)?)*)\\))\\s*(?:const)?\\s*(.*)\\r?$" };
    std::smatch m;
    size_t currentLine = 0;
    std::cout << "parsing functions ..." << std::endl;
    std::string includeCommand;
    while (std::getline(inputCode, userCode))
    {
        ++currentLine;
        if (std::regex_match(userCode, std::regex("\\s*#define\\s+RPC_FUNC_HEADER\\s*")))
        {
            while (std::getline(inputCode, userCode))
            {
                ++currentLine;
                if (std::regex_match(userCode, std::regex("\\s*#undef\\s+RPC_FUNC_HEADER\\s*")))
                {
                    break;
                }
                if (std::regex_match(userCode, std::regex("\\s*#include\\s+[<\"][^>\"]+[>\"]\\s*")))
                {
                    includeCommand += userCode + "\n";
                }
            }
        }
        while (true)
        {
            bool match = std::regex_match(userCode, m, funcDefRegEx);
            if (match && m.size() == 7) {
                FuncInf funcInf;
                std::string function = m[2].str();
                std::cout << "found function: \"" << function << "\"" << std::endl;
                std::cout << "at position: line " << currentLine << ", column " << m.position(2) << std::endl;

                funcInf.funcName = m[4].str();
                funcInf.retType = m[3].str();
                std::string argList = m[5].str();
                userCode = m[6].str();
                // info in m lost because userCode has changed
                size_t argCount = 0;
                const std::regex agrListRegEx{ "(?:((?:(?:const|volatile|mutable|static|extern|inline|virtual|explicit|friend)\\s+)*(?:\\w+::)*(?:\\w+<.*?>|\\w+)\\s*(?:\\*|\\&|\\&\\&|\\[\\d*\\])*)\\s+([A-Za-z_]\\w*)(?:,\\s*)?)?(.*)$" };
                do {
                    match = std::regex_match(argList, m, agrListRegEx);
                    if (match && m.size() == 4) {
                        ArgInf argInf;
                        std::cout << "found valid argument \"" << m[1].str() << " " << m[2].str() << "\"" << std::endl;

                        argInf.first = m[1].str();
                        argInf.second = m[2].str();
                        funcInf.argList.push_back(argInf);

                        ++argCount;
                        argList = m[3].str();
                    }
                    else {
                        std::cerr << "rpcc: cannot parse argument list" << std::endl;
                        break;
                    }
                } while (argList != "");

                if (argList == "")
                {
                    funcList.push_back(funcInf);
                }

                std::cout << "argument count: " << argCount << std::endl;
            }
            else {
                break;
            }
        }
    }
    inputCode.close();

    if (!funcList.size())
    {
        std::cerr << "rpcc: skip code generating since no valid functions found in " << inputPath << std::endl;
        return 1;
    }

    std::cout << "generating code ..." << std::endl;
    std::fstream codeOutput;
    codeOutput.open("rpc-client-impl.cpp", std::ios::out);
    if (!codeOutput.is_open())
    {
        std::cerr << "rpcc: unable to open rpc-client-impl.cpp" << std::endl;
        return 3;
    }
    codeOutput << "// generated code for rpc\n\n" << includeCommand << "#include \"rpc-client.hpp\"\n\nnamespace rpc {\n";
    for (size_t i = 0; i < funcList.size(); ++i)
    {
        codeOutput << "\t" << funcList[i].retType << " " << funcList[i].funcName << "(";
        for (size_t j = 0; j < funcList[i].argList.size(); ++j)
        {
            if (j > 0)
            {
                codeOutput << ", ";
            }
            codeOutput << funcList[i].argList[j].first << " " << funcList[i].argList[j].second;
        }
        codeOutput << ") {\n\t\treturn\n\t\t\tRPC_REQ_FUNC(\n\t\t\t\t\"" << funcList[i].funcName << "\",\n";
        for (size_t j = 0; j < funcList[i].argList.size(); ++j)
        {
            if (j > 0)
            {
                codeOutput << "\n";
            }
            codeOutput << "\t\t\t\tRPC_SET_ARGS(" << j + 1 << ", " << funcList[i].argList[j].second << ");";
        }
        codeOutput << " ,\n\t\t\t\t" << funcList[i].retType << ",\n\t\t\t\t";
        for (size_t j = 0; j < funcList[i].argList.size(); ++j)
        {
            if (j > 0)
            {
                codeOutput << ", ";
            }
            codeOutput << funcList[i].argList[j].second;
        }
        codeOutput << "\n\t\t\t);\n\t}\n";
    }
    codeOutput << "}";
    codeOutput.flush();
    codeOutput.close();

    codeOutput.open("rpc-function.hpp", std::ios::out);
    if (!codeOutput.is_open())
    {
        std::cerr << "rpcc: unable to open rpc-function.hpp" << std::endl;
        return 3;
    }
    codeOutput << "// generated code for rpc\n\n#pragma once\n\n" << includeCommand << "#include <string>\nnamespace rpc {\n\tvoid SetServer(std::string rHost, int rPort);\n";
    for (size_t i = 0; i < funcList.size(); ++i)
    {
        codeOutput << "\t" << funcList[i].retType << " " << funcList[i].funcName << "(";
        for (size_t j = 0; j < funcList[i].argList.size(); ++j)
        {
            if (j > 0)
            {
                codeOutput << ", ";
            }
            codeOutput << funcList[i].argList[j].first << " " << funcList[i].argList[j].second;
        }
        codeOutput << ");\n";
    }
    codeOutput << "}";
    codeOutput.flush();
    codeOutput.close();

    codeOutput.open("rpc-server-impl.cpp", std::ios::out);
    if (!codeOutput.is_open())
    {
        std::cerr << "rpcc: unable to open rpc-server-impl.cpp" << std::endl;
        return 3;
    }
    codeOutput << "// generated code for rpc\n\n" << includeCommand << "#include \"rpc-server.hpp\"\n\n";
    for (size_t i = 0; i < funcList.size(); ++i)
    {
        codeOutput << funcList[i].retType << " " << funcList[i].funcName << "(";
        for (size_t j = 0; j < funcList[i].argList.size(); ++j)
        {
            if (j > 0)
            {
                codeOutput << ", ";
            }
            codeOutput << funcList[i].argList[j].first << " " << funcList[i].argList[j].second;
        }
        codeOutput << ");\n";
    }
    codeOutput << "\nvoid RegFuncs() {\n";
    for (size_t i = 0; i < funcList.size(); ++i)
    {
        codeOutput << "\tRPC_REG_FUNC(\n\t\t" << funcList[i].funcName << ",\n\t\t" << funcList[i].argList.size() << ",\n";
        for (size_t j = 0; j < funcList[i].argList.size(); ++j)
        {
            if (j > 0)
            {
                codeOutput << ",\n";
            }
            codeOutput << "\t\tRPC_REG_FUNC_ARG(" << j + 1 << ", " << funcList[i].argList[j].first << ")";
        }
        codeOutput << "\n\t);\n";
    }
    codeOutput << "\treturn;\n}\n\nvoid ExecFunc(const std::string funcName, const std::vector<Argument>& args, json& retVal) {\n";
    for (size_t i = 0; i < funcList.size(); ++i)
    {
        codeOutput << (i > 0 ? "\telse " : "\t") << "if (funcName == \"" << funcList[i].funcName << "\") {\n\t\tretVal = RPC_CALL_FUNC(\"" << funcList[i].funcName << "\", " << funcList[i].retType << ", args);\n\t}\n";
    }
    codeOutput << "\telse {\n\t\tthrow std::runtime_error(\"Function not registered\");\n\t}\n\treturn;\n}";
    codeOutput.flush();
    codeOutput.close();
    std::cout << "rpcc: success" << std::endl;
    if (argc == 1)
    {
        std::cin.get();
    }
    return 0;
}
