#include "ChessBoard.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>  // For reading process output
#include <cstring> // For strerror

#ifdef _WIN32
#define _HAS_STD_BYTE 0 // Prevent std::byte conflicts
#include <direct.h>     // Keep for _getcwd if needed, or remove if fully relying on CreateProcess
#endif

using namespace std;

// Helper function to read from pipe
string readFromPipe(HANDLE pipe)
{
    char buffer[4096];
    DWORD bytesRead = 0;
    string output = "";

    // Check if there's data available without blocking
    DWORD bytesAvailable = 0;
    if (!PeekNamedPipe(pipe, NULL, 0, NULL, &bytesAvailable, NULL))
    {
        cerr << "PeekNamedPipe failed. Error: " << GetLastError() << endl;
        return "";
    }

    if (bytesAvailable > 0)
    {
        if (ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
        {
            buffer[bytesRead] = '\0'; // Null-terminate
            output = buffer;
        }
        else
        {
            // Handle potential ReadFile errors or 0 bytes read if pipe closed
            DWORD error = GetLastError();
            if (error != ERROR_BROKEN_PIPE && error != 0)
            {
                cerr << "ReadFile failed. Error: " << error << endl;
            }
        }
    }
    return output;
}

StockfishEngine::StockfishEngine()
    : engineProcessHandle(nullptr),
      hChildStd_IN_Rd(nullptr), hChildStd_IN_Wr(nullptr),
      hChildStd_OUT_Rd(nullptr), hChildStd_OUT_Wr(nullptr),
      initialized(false), skillLevel(10)
{
}

StockfishEngine::~StockfishEngine()
{
    close();
}

bool StockfishEngine::initialize()
{
#ifdef _WIN32
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create pipes for STDOUT
    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0))
    {
        cerr << "StdoutRd CreatePipe failed! Error: " << GetLastError() << endl;
        return false;
    }
    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
        cerr << "Stdout SetHandleInformation failed! Error: " << GetLastError() << endl;
        return false;
    }

    // Create pipes for STDIN
    if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0))
    {
        cerr << "Stdin CreatePipe failed! Error: " << GetLastError() << endl;
        return false;
    }
    if (!SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    {
        cerr << "Stdin SetHandleInformation failed! Error: " << GetLastError() << endl;
        return false;
    }

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE); // Redirect errors to console
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    string commandLine = "stockfish.exe";
    cerr << "Attempting to create process: " << commandLine << endl;

    BOOL bSuccess = CreateProcess(NULL,
                                  const_cast<char *>(commandLine.c_str()), // Command line needs to be mutable char*
                                  NULL,                                    // Process handle not inheritable
                                  NULL,                                    // Thread handle not inheritable
                                  TRUE,                                    // Set handle inheritance to TRUE
                                  CREATE_NO_WINDOW,                        // Don't create console window for stockfish
                                  NULL,                                    // Use parent's environment block
                                  NULL,                                    // Use parent's starting directory
                                  &siStartInfo,                            // Pointer to STARTUPINFO structure
                                  &piProcInfo);                            // Pointer to PROCESS_INFORMATION structure

    if (!bSuccess)
    {
        DWORD error = GetLastError();
        cerr << "CreateProcess failed! Error: " << error << endl;

        // Try absolute path as fallback
        char buffer[1024];
        if (_getcwd(buffer, sizeof(buffer)))
        {
            commandLine = string(buffer) + "\\stockfish.exe";
            cerr << "Retrying with absolute path: " << commandLine << endl;
            bSuccess = CreateProcess(NULL, const_cast<char *>(commandLine.c_str()), NULL, NULL, TRUE,
                                     CREATE_NO_WINDOW, NULL, NULL, &siStartInfo, &piProcInfo);
            if (!bSuccess)
            {
                error = GetLastError();
                cerr << "CreateProcess with absolute path failed! Error: " << error << endl;
                CloseHandle(hChildStd_OUT_Rd);
                CloseHandle(hChildStd_IN_Wr);
                return false;
            }
        }
        else
        {
            CloseHandle(hChildStd_OUT_Rd);
            CloseHandle(hChildStd_IN_Wr);
            return false; // Failed to get current directory
        }
    }

    // Successfully created process
    engineProcessHandle = piProcInfo.hProcess;
    // Close handles we don't need in the parent process
    CloseHandle(piProcInfo.hThread);
    CloseHandle(hChildStd_OUT_Wr); // Parent doesn't write to child stdout
    CloseHandle(hChildStd_IN_Rd);  // Parent doesn't read from child stdin

#else
    // Unix-like system logic (remains the same for now)
    engineProcess = popen("./stockfish", "r+");
    if (!engineProcess)
    {
        cerr << "Failed to start Stockfish engine! Error code: " << errno
             << " (" << strerror(errno) << ")" << endl;
        return false;
    }
#endif

    // Add a short delay to let Stockfish initialize
    this_thread::sleep_for(chrono::milliseconds(1000)); // Increased delay slightly

    // Initialize UCI mode
    string response = sendCommand("uci");
    cerr << "Stockfish initial response check: " << response << endl;

    // Wait up to 3 seconds for uciok
    bool uciOkReceived = false;
    for (int i = 0; i < 30; ++i)
    { // Check 10 times per second for 3 seconds
        if (response.find("uciok") != string::npos)
        {
            uciOkReceived = true;
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
        response += readFromPipe(hChildStd_OUT_Rd); // Append any new output
    }

    if (!uciOkReceived)
    {
        cerr << "Stockfish failed to initialize UCI mode! Final response: " << response << endl;
        close();
        return false;
    }
    cerr << "Stockfish UCI mode confirmed." << endl;

    // Configure the engine
    sendCommand("setoption name UCI_AnalyseMode value true");
    setDifficulty(skillLevel);

    // Send isready and wait for readyok
    response = sendCommand("isready");
    bool readyOkReceived = false;
    for (int i = 0; i < 30; ++i)
    {
        if (response.find("readyok") != string::npos)
        {
            readyOkReceived = true;
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
        response += readFromPipe(hChildStd_OUT_Rd);
    }

    if (!readyOkReceived)
    {
        cerr << "Stockfish failed to respond with readyok! Final response: " << response << endl;
        close();
        return false;
    }
    cerr << "Stockfish is ready." << endl;

    initialized = true;
    return true;
}

void StockfishEngine::setDifficulty(int level)
{
    if (!initialized)
        return;
    skillLevel = level;
    // Clamp skill level
    skillLevel = max(0, min(20, skillLevel));

    // Stockfish skill level setting (no error/probability needed for basic levels)
    stringstream ss;
    ss << "setoption name Skill Level value " << skillLevel;
    sendCommand(ss.str());
}

string StockfishEngine::getBestMove(const string &position, int moveTime)
{
    if (!initialized)
        return "";

    // Set position
    string posCmd = "position startpos";
    if (!position.empty())
    {
        posCmd += " moves " + position;
    }
    sendCommand(posCmd);

    // Calculate best move
    stringstream ss;
    ss << "go movetime " << moveTime;
    string response = sendCommand(ss.str());

    // Wait for bestmove response (up to moveTime + buffer)
    string bestMove = "";
    size_t bestMovePos = string::npos;
    int waitTime = moveTime + 1000; // Wait for movetime + 1 second buffer

    for (int i = 0; i < waitTime / 50; ++i)
    {
        bestMovePos = response.find("bestmove");
        if (bestMovePos != string::npos)
        {
            size_t spacePos = response.find(' ', bestMovePos + 9);
            if (spacePos != string::npos)
            {
                bestMove = response.substr(bestMovePos + 9, spacePos - (bestMovePos + 9));
            }
            else
            {
                bestMove = response.substr(bestMovePos + 9); // If no space after move
                // Trim potential newline/whitespace
                bestMove.erase(bestMove.find_last_not_of(" \n\r\t") + 1);
            }
            // Basic validation: check length (e.g., e2e4 is 4, a7a8q is 5)
            if (bestMove.length() >= 4 && bestMove.length() <= 5)
            {
                break; // Found valid-looking move
            }
            else
            {
                cerr << "Warning: Parsed potentially invalid move format: '" << bestMove << "'" << endl;
                bestMove = "";              // Reset if invalid format
                bestMovePos = string::npos; // Continue searching
            }
        }
        this_thread::sleep_for(chrono::milliseconds(50));
        response += readFromPipe(hChildStd_OUT_Rd);
    }

    if (bestMove.empty())
    {
        cerr << "Engine response did not contain bestmove after waiting. Full response: " << response << endl;
    }

    return bestMove;
}

string StockfishEngine::sendCommand(const string &command)
{
    string response = "";
#ifdef _WIN32
    if (!hChildStd_IN_Wr || !hChildStd_OUT_Rd)
        return "";

    // Send command to engine's stdin
    DWORD bytesWritten;
    string cmdWithNewline = command + "\n";
    if (!WriteFile(hChildStd_IN_Wr, cmdWithNewline.c_str(), cmdWithNewline.length(), &bytesWritten, NULL))
    {
        cerr << "WriteFile to pipe failed. Error: " << GetLastError() << endl;
        return "";
    }

    // Give the engine a little time to process before reading initial response
    this_thread::sleep_for(chrono::milliseconds(50));
    response = readFromPipe(hChildStd_OUT_Rd);

#else
    // Unix logic
    if (!engineProcess)
        return "";
    fprintf(engineProcess, "%s\n", command.c_str());
    fflush(engineProcess);
    this_thread::sleep_for(chrono::milliseconds(50));
    char buffer[4096] = {0};
    while (fgets(buffer, sizeof(buffer), engineProcess))
    {
        response += buffer;
        if (response.find("bestmove") != string::npos ||
            response.find("uciok") != string::npos ||
            response.find("readyok") != string::npos)
        {
            break;
        }
        if (feof(engineProcess) || ferror(engineProcess))
            break;
    }
#endif
    return response;
}

void StockfishEngine::close()
{
#ifdef _WIN32
    if (engineProcessHandle)
    {
        // Gently ask it to quit first
        sendCommand("quit");
        this_thread::sleep_for(chrono::milliseconds(200));

        // Forcefully terminate if still running
        DWORD exitCode;
        if (GetExitCodeProcess(engineProcessHandle, &exitCode) && exitCode == STILL_ACTIVE)
        {
            TerminateProcess(engineProcessHandle, 1);
        }
        CloseHandle(engineProcessHandle);
        engineProcessHandle = nullptr;
    }
    // Close pipe handles
    if (hChildStd_IN_Wr)
        CloseHandle(hChildStd_IN_Wr);
    hChildStd_IN_Wr = nullptr;
    if (hChildStd_OUT_Rd)
        CloseHandle(hChildStd_OUT_Rd);
    hChildStd_OUT_Rd = nullptr;
#else
    // Unix logic
    if (engineProcess)
    {
        sendCommand("quit");
        pclose(engineProcess);
        engineProcess = nullptr;
    }
#endif
    initialized = false;
}