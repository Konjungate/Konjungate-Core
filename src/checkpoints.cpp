// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (0,     Params().HashGenesisBlock() )
        (1,      uint256("0x0001a0666d542fc4a8652005d1cedc58e79c854fe2180009e5f765b9132af0d0"))
        (5,      uint256("0x0001245936cdf742cff5101e518f0fca18175171ecbfae2095723f1a1682bffd"))
        (10,     uint256("0x00004ab2b9ca9c44b4cd391084ac636668901d2ddc7a41562b0e9af25c6fb523"))
        (25,     uint256("0x00000fbca3e6d02a157d0d741f8704358e1cbcd9fb45f51ea0c0c2b1168cbe21"))
        (50,     uint256("0x0002bb79fc15bed841200211a506f690ee2e689460267afb8a6db3ea81ac3375"))
        (100,    uint256("0xdab561a9f373deb23375f3996773561b80e85f005e02269ee682c37251df42a8"))
        (125,    uint256("0x2773ba43fa984d53108ffb9cf762b874c547c190043fb307eb16b45bd224556d"))
        (178,    uint256("0x6e475f58326c289bf92f3fea30ebd552767ce85a2e3a42c77c5094cd00e7d1fb"))
        (250,    uint256("0xda9805cac3cc5134c3c113264bfe247b306753823959328be4204699cf165a71"))
        (386,    uint256("0x400c76fabe852292176214c5c8c9f6ae100356d5c40919288acf50d4aa376862"))
        (80000,  uint256("0x766f30ad50c5b48680e9f5f0d3be2a7594349945ea4ddf3160f2c4aa2f1017cc"))
        (100000, uint256("0xf6a13abde1e012185f5c46b4616368ad1eb64d31d54ba4f8b6d769aff942c9be"))
        (104293, uint256("0x000000000157c91b96608aeed733f5f3b8124c5795f1822faed2b7728c49b0cc"))
        (150000, uint256("0x000000000061edd95f733cb94de3b493f4b3f343d62240d6d0e164442d6be315"))
        (200000, uint256("0x0000000000fac9848205d60548055519732faf219137a6260bd3a003381b34e5"))
        (250000, uint256("0x0000000000bf74f747a060bef3b5b9e483832c66843bd58ba93978657bb6a7c0"))
        (300000, uint256("0x0000000000fc33954fb98cfdff5f72f3f334cd232d1e0b99c575f7c25df2c057"))
        (350000, uint256("0x28968f7124079e435c00df7468db85f366956a8b28500a6bdff108f2bfd7040b"))
        (400000, uint256("0xc98e9128138c5ac38f4529cea666a3fdcfa22ea14b691f49bb711d66bf19b687"))
        (450000, uint256("0x00000000008fe0fe49683a2909a247dd4d96be9db2c9347f3dd6ceaeed55ad9f"))
        (500000, uint256("0x83f3249bb781f3be2f3bdb4782295130eabe02b4aa8a184870843d32e3ba9796"))
        (550000, uint256("0x5680dcb37fd20160c3b6f9b737657ccada8d5ed7bcfe861fa0c4e6face67dec6"))
        (600000, uint256("0x276a8d53f8e19a341299034829d98bb996f060ebdaa959a41aa31828fbff2aaf"))
        (602073, uint256("0x0000000003e461bb4107459397a3561cee787eefb3dae398024dca0502c0816b"))
        (625000, uint256("0x0000000002cf42960483dcf438e74a6c11410eb2687e3fa63ae975c4235d1477"))
        (650000, uint256("0x301a0ce40a777d14ca2a40c885036e4fb21e281d8576bfa567be690d26c75afd"))
        (675000, uint256("0x0000000013be01ded5961bb66d90f07aece18be4de200b9a40a9d8a3b6586b20"))
        (700000, uint256("0x000000000135d5196d23f0dd055c55060ec4da98b233e8faf0dddcb264b166a7"))
        (725000, uint256("0x92e4fc040c008b91f8278a2f930e5d9f3b59f88628198540cdde7589a28776b9"))
        (750000, uint256("0x6c85d6bcb7ace94c67746941dccd239cd00aa12ff373c7608767c8b1817ebeac"))
        (775000, uint256("0x2d1ed218811a007b1ddb6db0bfc1375c62b90568406440e31824ab2100533c32"))
        (800000, uint256("0x855a8c9f8fc5eda57fae6e33a65bd7f0c21e80e58116629a0de6bcd1f663fc60"))
        (807390, uint256("0x4622497276183e6a406a74d29fb4a857c88f05ae517436d9ce70963d6cb95ef6"))
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
