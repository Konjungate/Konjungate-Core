// Copyright (c) 2021 The Espers Project/CryptoCoderz Team
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "demimodule.h"
#include "../util.h"

#include <boost/filesystem/fstream.hpp>


bool fDemiFound = false;

boost::filesystem::path GetDemiConfigFile()
{
    boost::filesystem::path pathConfigFile(GetArg("-conf", "Demi.conf"));
    if (!pathConfigFile.is_complete()) pathConfigFile = GetDataDir(false) / pathConfigFile;

    return pathConfigFile;
}

void ReadDemiConfigFile(std::string peerReadAddr)
{
    fDemiFound = false;
    boost::filesystem::ifstream streamConfig(GetDemiConfigFile());
    if (!streamConfig.good())
    {
        boost::filesystem::path ConfPath;
               ConfPath = GetDataDir() / "Demi.conf";
               FILE* ConfFile = fopen(ConfPath.string().c_str(), "w");
               fprintf(ConfFile, "170.187.136.118\n");
               fprintf(ConfFile, "170.187.136.118:19417\n");
               fprintf(ConfFile, "170.187.136.134\n");
               fprintf(ConfFile, "170.187.136.134:19417\n");
               fprintf(ConfFile, "170.187.136.177\n");
               fprintf(ConfFile, "170.187.136.177:19417\n");
               fprintf(ConfFile, "155.138.148.198\n");
               fprintf(ConfFile, "139.99.239.62:19417\n");
               fprintf(ConfFile, "51.195.42.49\n");
               fprintf(ConfFile, "51.195.42.49:19417\n");
               fprintf(ConfFile, "51.68.175.13\n");
               fprintf(ConfFile, "51.68.175.13:19417\n");

               fclose(ConfFile);
    }

    // Open requested config file
    LogPrintf("ReadDemiConfigFile - INFO - Loading Demi-nodes from: %s \n", GetDemiConfigFile().string().c_str());
    std::ifstream file;
    file.open(GetDemiConfigFile().string().c_str());
    if(!file.is_open()) {
        // Print for debugging
        LogPrintf("ReadDemiConfigFile - ERROR 00 - Cannot open file!\n");
        return;
    } else {
        // Print for debugging
        LogPrintf("ReadDemiConfigFile - INFO - File successfully opened!\n");
    }

    // Read data
    //
    // Print for debugging
    LogPrintf("ReadDemiConfigFile - INFO - Reading file...\n");
    std::string line;
    while(file.good()) {
        // Loop through lines
        std::getline(file, line);
        // Print for debugging
        LogPrintf("ReadDemiConfigFile - INFO - Got line data...\n");
        if (!line.empty()) {
            if (line[0] != '#') {
                // Print for debugging
                LogPrintf("ReadDemiConfigFile - INFO - Read data success!\n");
                // Combine input string
                if(peerReadAddr == line) {
                    // Print for debugging
                    LogPrintf("ReadDemiConfigFile - INFO - Set data success!\n");
                    fDemiFound = true;
                    break;
                }
            } else {
                // Print for debugging
                LogPrintf("ReadDemiConfigFile - WARNING - Comment detected! \n");
            }
        } else {
            // Print for debugging
            LogPrintf("ReadDemiConfigFile - WARNING - Empty line detected! \n");
        }
    }

    file.close();
}
