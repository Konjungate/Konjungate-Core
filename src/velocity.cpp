// Copyright (c) 2014 The Cryptocoin Revival Foundation
// Copyright (c) 2015-2020 The CryptoCoderz Team / Espers
// Copyright (c) 2018-2020 The Rubix Project
// Copyright (c) 2018-2022 The Konjungate project
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "blockparams.h"
#include "main.h"
#include "txdb.h"
#include "velocity.h"
#include "rpcserver.h"
#include "wallet.h"

bool VELOCITY_FACTOR = false;
uint256 RollingBlock;
int64_t RollingHeight;

/* VelocityI(int nHeight) ? i : -1
   Returns i or -1 if not found */
int VelocityI(int nHeight)
{
    if (FACTOR_TOGGLE(nHeight)) {
        VELOCITY_FACTOR = true;
    }
    return nHeight;
}

/* Velocity(int nHeight) ? true : false
   Returns true if nHeight is higher or equal to VELOCITY_HEIGHT */
bool Velocity_check(int nHeight)
{
    LogPrintf("Checking for Velocity on block %u: ",nHeight);
    if(VelocityI(nHeight) >= VELOCITY_HEIGHT[nHeight])
    {
        LogPrintf("Velocity is currently Enabled\n");
        return true;
    }
    LogPrintf("Velocity is currently disabled\n");
    return false;
}

/* Velocity(CBlockIndex* prevBlock, CBlock* block) ? true : false
   Goes close to the top of CBlock::AcceptBlock
   Returns true if proposed Block matches constrains */
bool Velocity(CBlockIndex* prevBlock, CBlock* block, bool fFactor_tx)
{
    // Define values
    int64_t TXrate = 0;
    int64_t CURvalstamp  = 0;
    int64_t OLDvalstamp  = 0;
    int64_t CURstamp = 0;
    int64_t OLDstamp = 0;
    int64_t TXstampC = 0;
    int64_t TXstampO = 0;
    int64_t SYScrntstamp = 0;
    int64_t SYSbaseStamp = 0;
    int nHeight = prevBlock->nHeight+1;
    int i = VelocityI(nHeight);
    // Set stanard values
    TXrate = block->GetBlockTime() - prevBlock->GetBlockTime();
    TXstampC = block->nTime;
    TXstampO = prevBlock->nTime;
    CURstamp = block->GetBlockTime();
    OLDstamp = prevBlock->GetBlockTime();
    CURvalstamp = prevBlock->GetBlockTime() + VELOCITY_MIN_RATE[i];
    OLDvalstamp = prevBlock->pprev->GetBlockTime() + VELOCITY_MIN_RATE[i];
    SYScrntstamp = GetAdjustedTime() + VELOCITY_MIN_RATE[i];
    SYSbaseStamp = GetTime() + VELOCITY_MIN_RATE[i];

    // Factor in TXs for Velocity constraints
    if(VELOCITY_FACTOR == true && fFactor_tx)
    {
        // Run TX factoring
        if(!tx_Factor(prevBlock, block))
        {
            LogPrintf("DENIED: Velocity denied block: %u\n", nHeight);
            return false;
        }
    }

    // Verify minimum Velocity rate
    if( VELOCITY_RATE[i] > 0 && TXrate >= VELOCITY_MIN_RATE[i] )
    {
        LogPrintf("CHECK_PASSED: block spacing has met Velocity constraints\n");
    }
    // Rates that are too rapid are rejected without exception
    else if( VELOCITY_RATE[i] > 0 && TXrate < VELOCITY_MIN_RATE[i] )
    {
        LogPrintf("DENIED: Minimum block spacing not met for Velocity\n");
        return false;
    }

    // Validate timestamp is logical based on previous block history
    else if(CURstamp < CURvalstamp || OLDstamp < OLDvalstamp || TXstampC < CURvalstamp || TXstampO < OLDvalstamp)
    {
        LogPrintf("DENIED: Block timestamp is not logical\n");
        return false;
    }

    // Validate timestamp is logical based on system time
    if(CURstamp > SYSbaseStamp || CURstamp > SYScrntstamp || TXstampC > SYSbaseStamp || TXstampC > SYScrntstamp)
    {
        LogPrintf("DENIED: Block timestamp is not logical\n");
        return false;
    }

    // Constrain Velocity
    if(VELOCITY_EXPLICIT[i])
    {
        if(VELOCITY_MIN_TX[i] > 0)
            return false;
        if(VELOCITY_MIN_VALUE[i] > 0)
            return false;
        if(VELOCITY_MIN_FEE[i] > 0)
            return false;
    }

    // Velocity constraints met, return block acceptance
    LogPrintf("ACCEPTED: block has met all Velocity constraints\n");
    return true;
}

bool RollingCheckpoints(int nHeight)
{
    // Skip chain start
    if (nHeight < 500) {
        return false;
    }
    // Define values
    CBlockIndex* pindexCurrentBlock = pindexBest;
    CBlockIndex* pindexPastBlock = pindexCurrentBlock;
    // Set count and loop
    int pastBLOCK_1 = (pindexCurrentBlock->nHeight - (BLOCK_TEMP_CHECKPOINT_DEPTH + BLOCK_REORG_THRESHOLD));
    while (pastBLOCK_1 < pindexCurrentBlock->nHeight) {
        // Index backwards
        pindexPastBlock = pindexPastBlock->pprev;
        pastBLOCK_1 ++;
    }
    // Set output values
    RollingBlock = pindexPastBlock->GetBlockHash();
    RollingHeight = pindexPastBlock->nHeight;
    // Success
    return true;
}

// Factor in TXs for Velocity constraints
bool tx_Factor(CBlockIndex* prevBlock, CBlock* block)
{
    // Define Values
    CAmount tx_inputs_values = 0;
    CAmount tx_outputs_values = 0;
    CAmount tx_MapIn_values = 0;
    CAmount tx_MapOut_values = 0;
    CAmount tx_threshold = 0;
    // Negative Stake Refund Values
    bool containsRefundPayment = false;
    int nRefundIndex;

    if(block->IsProofOfStake()) {
        tx_threshold = GetProofOfStakeReward(prevBlock, 0, 0);
    } else {
        tx_threshold = GetProofOfWorkReward(prevBlock->nHeight+1, 0);
    }

    // Enforce security after negative stake refund
    if(prevBlock->nHeight > 700000) {
        // Set factor values
        BOOST_FOREACH(const CTransaction& tx, block->vtx)
        {
            // Load TX inputs
            CTxDB txdb("r");
            MapPrevTx mapInputs;
            map<uint256, CTxIndex> mapUnused;
            bool fInvalid = false;
            // Ensure we can fetch inputs
            if (!tx.FetchInputs(txdb, mapUnused, true, false, mapInputs, fInvalid))
            {
                LogPrintf("DENIED: Invalid TX found during FetchInputs\n");
                return false;
            }
            // Authenticate submitted block's TXs
            tx_MapIn_values = tx.GetValueMapIn(mapInputs);
            tx_MapOut_values = tx.GetValueOut();
            if(tx_inputs_values + tx_MapIn_values >= 0) {
                tx_inputs_values += tx_MapIn_values;
            } else {
                LogPrintf("DENIED: overflow detected tx_inputs_values + tx.GetValueMapIn(mapInputs)\n");
                return false;
            }
            if(tx_outputs_values + tx_MapOut_values >= 0) {
                tx_outputs_values += tx_MapOut_values;
            } else {
                LogPrintf("DENIED: overflow detected tx_outputs_values + tx.GetValueOut()\n");
                return false;
            }
        }
        // Ensure input/output sanity of transactions in the block
        if((tx_inputs_values + tx_threshold) < tx_outputs_values)
        {
            LogPrintf("DENIED: block contains a tx input that is less that output\n");
            return false;
        }
    }

    // Negative stake Refund starts after nPaymentUpdate_4
    if(prevBlock->nHeight < nPaymentUpdate_4) {
        LogPrintf("INFO: skipping negative stake refund, block height too low.\n");
    } else {
        if(prevBlock->nHeight < nEndOfRefund)
        {
            //
            CTransaction cRewardTx = block->IsProofOfStake() ? block->vtx[1] : block->vtx[0];
            //
            if(block->IsProofOfStake())
            {
                int nVoutSize = cRewardTx.vout.size();
        
                if(prevBlock->nHeight < nEndOfRefund)
                {
                    if(nVoutSize < 5) {
                        LogPrintf("DENIED: [Negative Stake Refund] not enough outputs in coinstake tx (%d)", nVoutSize); 
                        return false;
                    }

                    if(cRewardTx.vout[nVoutSize-1].nValue == nBlockStandardRefund){
                        containsRefundPayment = true;
                        nRefundIndex = nVoutSize-1;
                        nVoutSize -= 1;
                    }
                }

                if(nVoutSize < 4) {
                    LogPrintf("DENIED: [Negative Stake Refund] not enough outputs in coinstake tx (%d)", nVoutSize);           
                    return false;
                }
            } else {
                CTxOut txOut;
                for (unsigned int i = 0; i < cRewardTx.vout.size(); i++)
                {
                    txOut = cRewardTx.vout[i];
                    if(txOut.nValue == nBlockStandardRefund){
                        containsRefundPayment = true;
                        nRefundIndex = i;
                    }
                }
            }
            //
            if(!containsRefundPayment) {
                return error("DENIED: [Negative Stake Refund] refund payment missing or incorrect");
            }
        
            if(!IsInitialBlockDownload())
            {
                int nHeightRefund = prevBlock->nHeight+1 - nNbrWrongBlocks;
                CBlock blockRefund;
                CBlockIndex* pBlockIndexRefund;
                uint256 hash;
                
                pBlockIndexRefund = mapBlockIndex[hashBestChain];
            
                while (pBlockIndexRefund->nHeight > nHeightRefund) {
                    pBlockIndexRefund = pBlockIndexRefund->pprev;
                }

                hash = *pBlockIndexRefund->phashBlock;
            
                pBlockIndexRefund = mapBlockIndex[hash];
                blockRefund.ReadFromDisk(pBlockIndexRefund, true);

                if(blockRefund.IsProofOfStake()) {
                    CScript refundpayee = blockRefund.vtx[1].vout[1].scriptPubKey;
                
                    if (cRewardTx.vout[nRefundIndex].scriptPubKey != refundpayee) {
                        LogPrintf("DENIED: [Negative Stake Refund] refund payee is incorrect");
                        return false;
                    }
                }
            }
        } else {
            //
            LogPrintf("INFO: skipping negative stake refund, refund phase now over.\n");
        }
    }

    // Return success if we get here
    LogPrintf("CHECK_PASSED: transaction/input factoring has met Velocity constraints\n");
    return true;
}

bool bIndex_Factor(CBlockIndex* InSplitPoint, CBlockIndex* InSplitEnd, int InFactor)
{
    CAmount tx_threshold = 1 * COIN;
    tx_threshold *= InFactor;

    // Ensure expected coin supply matches actualy coin supply of branch
    if(((InSplitPoint->nMoneySupply + tx_threshold) / COIN) < (InSplitEnd->nMoneySupply / COIN))
    {
        LogPrintf("VELOCITY_FACTOR: Mismatched supply in branch, excpected: %u | found: %u\n", (int64_t)((InSplitPoint->nMoneySupply + tx_threshold) / COIN), (int64_t)(InSplitEnd->nMoneySupply / COIN));
        LogPrintf("DENIED: branch contains invalid coin supply amount\n");
        return false;
    }
    return true;
}
