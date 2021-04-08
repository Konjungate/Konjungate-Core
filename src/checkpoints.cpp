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
        (175000, uint256("0x180b0cfd6557b5e6eb38aad79165ebe360faf7d40cb8ca319e76acaf0fd5048d"))
        (200000, uint256("0x0000000000fac9848205d60548055519732faf219137a6260bd3a003381b34e5"))
        (250000, uint256("0x0000000000bf74f747a060bef3b5b9e483832c66843bd58ba93978657bb6a7c0"))
        (350000, uint256("0x28968f7124079e435c00df7468db85f366956a8b28500a6bdff108f2bfd7040b"))
        (400000, uint256("0xc98e9128138c5ac38f4529cea666a3fdcfa22ea14b691f49bb711d66bf19b687"))
        (425000, uint256("0x0000000000136c280a3bfd9f7cd64b939ab5c5958f774dfa7da53ac3e832a245"))
        (450000, uint256("0x00000000008fe0fe49683a2909a247dd4d96be9db2c9347f3dd6ceaeed55ad9f"))
        (480000, uint256("0x0000000000484a7d61299ded479ba89f67afe543e6ca413edeb537281f3cf0b3"))
        (481500, uint256("0xb213f365005069398f6e0b55064eca40ec2d3856921104cc74b90627493b2509"))
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
