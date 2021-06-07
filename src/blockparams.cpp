// Copyright (c) 2016-2020 The CryptoCoderz Team / Espers
// Copyright (c) 2018-2020 The CryptoCoderz Team / INSaNe project
// Copyright (c) 2018-2020 The Rubix project
// Copyright (c) 2018-2020 The Konjungate project
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "blockparams.h"
#include "chainparams.h"
#include "checkpoints.h"
#include "db.h"
#include "init.h"
#include "kernel.h"
#include "net.h"
#include "txdb.h"
#include "velocity.h"
#include "main.h"
#include "mnengine.h"
#include "masternodeman.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace std;
using namespace boost;

//////////////////////////////////////////////////////////////////////////////
//
// Standard Global Values
//

//
// Section defines global values for retarget logic
//

double VLF1 = 0;
double VLF2 = 0;
double VLF3 = 0;
double VLF4 = 0;
double VLF5 = 0;
double VLFtmp = 0;
double VRFsm1 = 1;
double VRFdw1 = 0.75;
double VRFdw2 = 0.5;
double VRFup1 = 1.25;
double VRFup2 = 1.5;
double VRFup3 = 2;
double TerminalAverage = 0;
double TerminalFactor = 10000;
double debugTerminalAverage = 0;
CBigNum newBN = 0;
CBigNum oldBN = 0;
int64_t VLrate1 = 0;
int64_t VLrate2 = 0;
int64_t VLrate3 = 0;
int64_t VLrate4 = 0;
int64_t VLrate5 = 0;
int64_t VLRtemp = 0;
int64_t DSrateNRM = BLOCK_SPACING;
int64_t DSrateMAX = BLOCK_SPACING_MAX;
int64_t FRrateDWN = DSrateNRM - 60;
int64_t FRrateFLR = DSrateNRM - 90;
int64_t FRrateCLNG = DSrateMAX + 180;
int64_t difficultyfactor = 0;
int64_t AverageDivisor = 5;
int64_t scanheight = 6;
int64_t scanblocks = 1;
int64_t scantime_1 = 0;
int64_t scantime_2 = 0;
int64_t prevPoW = 0; // hybrid value
int64_t prevPoS = 0; // hybrid value
uint64_t blkTime = 0;
uint64_t cntTime = 0;
uint64_t prvTime = 0;
uint64_t difTime = 0;
uint64_t hourRounds = 0;
uint64_t difCurve = 0;
uint64_t debugHourRounds = 0;
uint64_t debugDifCurve = 0;
bool fDryRun;
bool fCRVreset;
const CBlockIndex* pindexPrev = 0;
const CBlockIndex* BlockVelocityType = 0;
CBigNum bnVelocity = 0;
CBigNum bnOld;
CBigNum bnNew;
std::string difType ("");
unsigned int retarget = DIFF_VRX; // Default with VRX


//////////////////////////////////////////////////////////////////////////////
//
// Debug section
//

//
// Debug log printing
//

void VRXswngdebug()
{
    // Print for debugging
    LogPrintf("Previously discovered %s block: %u: \n",difType.c_str(),prvTime);
    LogPrintf("Current block-time: %u: \n",cntTime);
    LogPrintf("Time since last %s block: %u: \n",difType.c_str(),difTime);
    // Handle updated versions as well as legacy
    if(GetTime() > nPaymentUpdate_2) {
        debugHourRounds = hourRounds;
        debugTerminalAverage = TerminalAverage;
        debugDifCurve = difCurve;
        while(difTime > (debugHourRounds * 60 * 60)) {
            debugTerminalAverage /= debugDifCurve;
            LogPrintf("diffTime%s is greater than %u Hours: %u \n",difType.c_str(),debugHourRounds,cntTime);
            LogPrintf("Difficulty will be multiplied by: %d \n",debugTerminalAverage);
            // Break loop after 5 hours, otherwise time threshold will auto-break loop
            if (debugHourRounds > 5){
                break;
            }
            debugDifCurve ++;
            debugHourRounds ++;
        }
    } else {
        if(difTime > (hourRounds+0) * 60 * 60) {LogPrintf("diffTime%s is greater than 1 Hours: %u \n",difType.c_str(),cntTime);}
        if(difTime > (hourRounds+1) * 60 * 60) {LogPrintf("diffTime%s is greater than 2 Hours: %u \n",difType.c_str(),cntTime);}
        if(difTime > (hourRounds+2) * 60 * 60) {LogPrintf("diffTime%s is greater than 3 Hours: %u \n",difType.c_str(),cntTime);}
        if(difTime > (hourRounds+3) * 60 * 60) {LogPrintf("diffTime%s is greater than 4 Hours: %u \n",difType.c_str(),cntTime);}
    }

    return;
}

void VRXdebug()
{
    // Print for debugging
    LogPrintf("Terminal-Velocity 1st spacing: %u: \n",VLrate1);
    LogPrintf("Terminal-Velocity 2nd spacing: %u: \n",VLrate2);
    LogPrintf("Terminal-Velocity 3rd spacing: %u: \n",VLrate3);
    LogPrintf("Terminal-Velocity 4th spacing: %u: \n",VLrate4);
    LogPrintf("Terminal-Velocity 5th spacing: %u: \n",VLrate5);
    LogPrintf("Desired normal spacing: %u: \n",DSrateNRM);
    LogPrintf("Desired maximum spacing: %u: \n",DSrateMAX);
    LogPrintf("Terminal-Velocity 1st multiplier set to: %f: \n",VLF1);
    LogPrintf("Terminal-Velocity 2nd multiplier set to: %f: \n",VLF2);
    LogPrintf("Terminal-Velocity 3rd multiplier set to: %f: \n",VLF3);
    LogPrintf("Terminal-Velocity 4th multiplier set to: %f: \n",VLF4);
    LogPrintf("Terminal-Velocity 5th multiplier set to: %f: \n",VLF5);
    LogPrintf("Terminal-Velocity averaged a final multiplier of: %f: \n",TerminalAverage);
    LogPrintf("Prior Terminal-Velocity: %u\n", oldBN);
    LogPrintf("New Terminal-Velocity:  %u\n", newBN);
    return;
}

void GNTdebug()
{
    // Print for debugging
    // Retarget ignoring invalid selection
    if (retarget != DIFF_VRX)
    {
        // debug info for testing
        LogPrintf("GetNextTargetRequired() : Invalid retarget selection, using default \n");
        return;
    }

    // Retarget using Terminal-Velocity
    // debug info for testing
    LogPrintf("Terminal-Velocity retarget selected \n");
    LogPrintf("Espers retargetted using: Terminal-Velocity difficulty curve \n");
    return;
}


//////////////////////////////////////////////////////////////////////////////
//
// Difficulty retarget (current section)
//

//
// This is VRX (v3.5) revised implementation
//
// Terminal-Velocity-RateX, v10-Beta-R9, written by Jonathan Dan Zaretsky - cryptocoderz@gmail.com
void VRX_BaseEngine(const CBlockIndex* pindexLast, bool fProofOfStake)
{
       // Set base values
       VLF1 = 0;
       VLF2 = 0;
       VLF3 = 0;
       VLF4 = 0;
       VLF5 = 0;
       VLFtmp = 0;
       TerminalAverage = 0;
       TerminalFactor = 10000;
       VLrate1 = 0;
       VLrate2 = 0;
       VLrate3 = 0;
       VLrate4 = 0;
       VLrate5 = 0;
       VLRtemp = 0;
       difficultyfactor = 0;
       scanblocks = 1;
       scantime_1 = 0;
       scantime_2 = pindexLast->GetBlockTime();
       prevPoW = 0; // hybrid value
       prevPoS = 0; // hybrid value
       // Set prev blocks...
       pindexPrev = pindexLast;
       // ...and deduce spacing
       while(scanblocks < scanheight)
       {
           scantime_1 = scantime_2;
           pindexPrev = pindexPrev->pprev;
           scantime_2 = pindexPrev->GetBlockTime();
           // Set standard values
           if(scanblocks > 0){
               if     (scanblocks < scanheight-4){ VLrate1 = (scantime_1 - scantime_2); VLRtemp = VLrate1; }
               else if(scanblocks < scanheight-3){ VLrate2 = (scantime_1 - scantime_2); VLRtemp = VLrate2; }
               else if(scanblocks < scanheight-2){ VLrate3 = (scantime_1 - scantime_2); VLRtemp = VLrate3; }
               else if(scanblocks < scanheight-1){ VLrate4 = (scantime_1 - scantime_2); VLRtemp = VLrate4; }
               else if(scanblocks < scanheight-0){ VLrate5 = (scantime_1 - scantime_2); VLRtemp = VLrate5; }
           }
           // Round factoring
           if(VLRtemp >= DSrateNRM){ VLFtmp = VRFsm1;
               if(VLRtemp > DSrateMAX){ VLFtmp = VRFdw1;
                   if(VLRtemp > FRrateCLNG){ VLFtmp = VRFdw2; }
               }
           }
           else if(VLRtemp < DSrateNRM){ VLFtmp = VRFup1;
               if(VLRtemp < FRrateDWN){ VLFtmp = VRFup2;
                   if(VLRtemp < FRrateFLR){ VLFtmp = VRFup3; }
               }
           }
           // Record factoring
           if      (scanblocks < scanheight-4) VLF1 = VLFtmp;
           else if (scanblocks < scanheight-3) VLF2 = VLFtmp;
           else if (scanblocks < scanheight-2) VLF3 = VLFtmp;
           else if (scanblocks < scanheight-1) VLF4 = VLFtmp;
           else if (scanblocks < scanheight-0) VLF5 = VLFtmp;
           // Log hybrid block type
           //
           // v1.0
           if(pindexBest->GetBlockTime() < 1520198278) // ON Sunday, March 4, 2018 9:17:58 PM
           {
                if     (fProofOfStake) prevPoS ++;
                else if(!fProofOfStake) prevPoW ++;
           }
           // v1.1
           if(pindexBest->GetBlockTime() > 1520198278) // ON Sunday, March 4, 2018 9:17:58 PM
           {
               if(pindexPrev->IsProofOfStake()) { prevPoS ++; }
               else if(pindexPrev->IsProofOfWork()) { prevPoW ++; }
           }

           // move up per scan round
           scanblocks ++;
       }
       // Final mathematics
       TerminalAverage = (VLF1 + VLF2 + VLF3 + VLF4 + VLF5) / AverageDivisor;
       return;
}

void VRX_Simulate_Retarget()
{
    // Perform retarget simulation
    TerminalFactor *= TerminalAverage;
    difficultyfactor = TerminalFactor;
    bnOld.SetCompact(BlockVelocityType->nBits);
    bnNew = bnOld / difficultyfactor;
    bnNew *= 10000;
    // Reset TerminalFactor for actual retarget
    TerminalFactor = 10000;
    return;
}

void VRX_ThreadCurve(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    // Run VRX engine
    VRX_BaseEngine(pindexLast, fProofOfStake);

    //
    // Skew for less selected block type
    //

    // Version 1.0
    //
    int64_t nNow = nBestHeight; int64_t nThen = 10; // Toggle skew system fork - Mon, 01 May 2017 00:00:00 GMT
    if(nNow > nThen){if(prevPoW < prevPoS && !fProofOfStake){if((prevPoS-prevPoW) > 3) TerminalAverage /= 3;}
    else if(prevPoW > prevPoS && fProofOfStake){if((prevPoW-prevPoS) > 3) TerminalAverage /= 3;}
    if(TerminalAverage < 0.5) TerminalAverage = 0.5;} // limit skew to halving

    // Version 1.1 curve-patch
    //
    if(1 == 1) // ON Sunday, March 4, 2018 9:17:58 PM
    {
        // Define time values
        blkTime = pindexLast->GetBlockTime();
        cntTime = BlockVelocityType->GetBlockTime();
        prvTime = BlockVelocityType->pprev->GetBlockTime();
        difTime = cntTime - prvTime;
        hourRounds = 1;
        difCurve = 2;
        fCRVreset = false;

        // Debug print toggle
        if(fProofOfStake) {
            difType = "PoS";
        } else {
            difType = "PoW";
        }
        if(fDebug) VRXswngdebug();

        // Version 1.2 Extended Curve Run Upgrade
        if(pindexLast->GetBlockTime() > nPaymentUpdate_2) {// ON Tuesday, Jul 02, 2019 12:00:00 PM PDT
            // Set unbiased comparison
            difTime = blkTime - cntTime;
            // Run Curve
            while(difTime > (hourRounds * 60 * 60)) {
                // Break loop after 5 hours, otherwise time threshold will auto-break loop
                if (hourRounds > 5){
                    fCRVreset = true;
                    break;
                }
                // Drop difficulty per round
                TerminalAverage /= difCurve;
                // Simulate retarget for sanity
                VRX_Simulate_Retarget();
                // Increase Curve per round
                difCurve ++;
                // Move up an hour per round
                hourRounds ++;
            }
        } else {// Version 1.1 Standard Curve Run
            if(difTime > (hourRounds+0) * 60 * 60) { TerminalAverage /= difCurve; }
            if(difTime > (hourRounds+1) * 60 * 60) { TerminalAverage /= difCurve; }
            if(difTime > (hourRounds+2) * 60 * 60) { TerminalAverage /= difCurve; }
            if(difTime > (hourRounds+3) * 60 * 60) { TerminalAverage /= difCurve; }
        }
    }
    return;
}

void VRX_Dry_Run(const CBlockIndex* pindexLast)
{
    // Check for blocks to index | Allowing for initial chain start
    if (pindexLast->nHeight < scanheight+124) {
        fDryRun = true;
        return; // can't index prevblock
    }

    // Test Fork
    if (nLiveForkToggle != 0) {
        // Do nothing
    }// TODO setup next testing fork

    // Standard, non-Dry Run
    fDryRun = false;
    return;
}

unsigned int VRX_Retarget(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    // Set base values
    bnVelocity = fProofOfStake ? Params().ProofOfStakeLimit() : Params().ProofOfWorkLimit();

    // Differentiate PoW/PoS prev block
    BlockVelocityType = GetLastBlockIndex(pindexLast, fProofOfStake);

    // Check for a dry run
    VRX_Dry_Run(pindexLast);
    if(fDryRun) { return bnVelocity.GetCompact(); }

    // Run VRX threadcurve
    VRX_ThreadCurve(pindexLast, fProofOfStake);
    if (fCRVreset) { return bnVelocity.GetCompact(); }

    // Retarget using simulation
    VRX_Simulate_Retarget();

    // Limit
    if (bnNew > bnVelocity) { bnNew = bnVelocity; }

    // Final log
    oldBN = bnOld.GetCompact();
    newBN = bnNew.GetCompact();

    // Debug print toggle
    if(fDebug) VRXdebug();

    // Return difficulty
    return bnNew.GetCompact();
}

//////////////////////////////////////////////////////////////////////////////
//
// Difficulty retarget (function)
//

unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    // Default with VRX
    unsigned int retarget = DIFF_VRX;

    // Check selection
    if (retarget != DIFF_VRX)
    {
        // debug info for testing
        if(fDebug) GNTdebug();
        return VRX_Retarget(pindexLast, fProofOfStake);
    }

    // Retarget using Terminal-Velocity
    // debug info for testing
    if(fDebug) GNTdebug();
    return VRX_Retarget(pindexLast, fProofOfStake);
}

//////////////////////////////////////////////////////////////////////////////
//
// Coin base subsidy
//
//
// Reward calculations for 25-years of KONJ emissions
// ----------------------------------
// Block numbers based on 2-minute blocktime average
// (Not including initial 250 starting blocks)
// Blocks per day       :     720
// Blocks per month     :  21,600
// Blocks per year      : 262,800
// ----------------------------------
// 100% for Calculations: 720 blocks per day
// Payout per block     : 300 KONJ
// Payout per day       : 300 * ((1 * 60 * 60) / (2 * 60) * 24)                   =       216,000 KONJ
// Payout per month     : 300 * (((1 * 60 * 60) / (2 * 60) * 24) * 30)            =     6,480,000 KONJ
// Payout per year      : 300 * (((1 * 60 * 60) / (2 * 60) * 24) * 365)           =    78,840,000 KONJ
// Mineout              : 300 * (25.36 * (((1 * 60 * 60) / (2 * 60) * 24) * 365)) = 2,750,000,000 KONJ
// ----------------------------------
// (Network Allocation) (BLOCKS | 25-Years of minting)
// Singular Payout      : 150-->50 KONJ
// Maternode Payout     : 100-->200 KONJ
// DevOps Payout        : 50 KONJ
// ----------------------------------
// (PLEASE NOTE)
// Masternode Payout is calculated based on the assumption of starting payout date of Masternode Payout and
// DevOps payout matching in terms of start date. 
// DevOps may or may not start at at the same time as the Masternode Payouts at which point the numbers
// will be skewed off slightly in either direction.
// This is the same for DevOps payout, it is assumed for its calculations that it starts with Masternode
// Payouts.
// ----------------------------------
// (Masternode | Network) SeeSaw
// Increment step       : 20% step
// Interval             : 30 blocks
// Step per Interval    : 1 (20% step per interval)
// Steps per swing      : 5 Steps up or down 
// Epoch (SeeSaw finish): 15 Intervals
// Upswing Duration     : 5 Intervals
// Downswing Duration   : 5 Intervals
// Idle Duration        : 5 Intervals (no adjustment)
//

//
//
// PoW coin base reward
//
int64_t GetProofOfWorkReward(int nHeight, int64_t nFees){    
    int64_t nSubsidy = nBlockStandardReward;// Reward amount
    int64_t nMasterNodeAdjustment = 113.75 * COIN;// Masternode extra generation
    
    if(pindexBest->nHeight > 488888){
        nMasterNodeAdjustment = 153.75 * COIN;// 17% Step up payment adjustment base
    }
    
    if(pindexBest->nHeight < nPaymentUpdate_4){ // POS FIX
        if(pindexBest->nHeight > 526000){// Fork toggle (Has to be the first loop or else height - fork height = negative....)    
            int64_t nDownSubsidy = nSubsidy / 10;// 10% step down value
            int64_t i = ((pindexBest->nHeight - 488888) / 526000);// CURRENT_HEIGHT - FORK_HEIGHT(desired) / 6 Months = possible loops
            int64_t i2 = 0;// Base value for loop logic
    
            while(i2 <= i){// Loop for as many times as possible    
                if(i2 <= 9){// Only drop subsidy up to 9 times, regardless of loop count
                    nSubsidy -= nDownSubsidy;// 10% Step down payment adjustment
                } else if(i2 <= 19) {
                    int64_t nMNdownSubsidy = nMasterNodeAdjustment / 17;// 17% step up value
                    nSubsidy += nMasterNodeAdjustment + nMNdownSubsidy;// 17% Step up payment adjustment
                } else {
                    break;// Limit looping to max loop rounds
                }
                i2++;// Move up in loop round
            }
        } else {
            nSubsidy += nMasterNodeAdjustment;
        }
    } else {
        if(pindexBest->nHeight > 526000){// Fork toggle (Has to be the first loop or else height - fork height = negative....)
            int64_t i = ((pindexBest->nHeight - 488888) / 526000);// CURRENT_HEIGHT - FORK_HEIGHT(desired) / 6 Months = possible loops
            int64_t i2 = 0;// Base value for loop logic
        
            while(i2 <= i){// Loop for as many times as possible
                if(nSubsidy > 30 * COIN){
                    nSubsidy -= (nSubsidy * 10) / 100;// 10% Step down payment adjustment
                } else {
                    nSubsidy = 30 * COIN;
                }
                if(i2 > 0){
                    if(nMasterNodeAdjustment < 210 * COIN){
                        nMasterNodeAdjustment += (nMasterNodeAdjustment * 17) / 100;// 17% Step up payment adjustment
                    } else {
                        nMasterNodeAdjustment = 210 * COIN;
                    }
                }
                if(nSubsidy == 30 * COIN && nMasterNodeAdjustment == 210 * COIN)
                    break;
                i2++;// Move up in loop round
            }
            nSubsidy += nMasterNodeAdjustment;
        }
    }
        
    //premine function
    if(nHeight > nReservePhaseStart) {
        if(pindexBest->nMoneySupply < (nBlockRewardReserve * 100)) {
            nSubsidy = nBlockRewardReserve;
        }
    }
   
    // 30.21 = PoW Payments for regular miners
    // hardCap v2.1
    if(pindexBest->nMoneySupply > MAX_SINGLE_TX){
        LogPrint("MINEOUT", "GetProofOfWorkReward(): create=%s nFees=%d\n", FormatMoney(nFees), nFees);
        return nFees;
    }
   
    return nSubsidy + nFees;
}

//
// PoS coin base reward
//
int64_t GetProofOfStakeReward(const CBlockIndex* pindexPrev, int64_t nCoinAge, int64_t nFees)
{
    int64_t nSubsidy = 48.75 * COIN;// Reward amount
    int64_t nMasterNodeAdjustment = 113.75 * COIN;// Masternode extra generation
    
    if(pindexBest->nHeight > 488888){
        nSubsidy = 68.75 * COIN;// 15% Step down payment adjustment baser
        nMasterNodeAdjustment = 153.75 * COIN;// 17% Step up payment adjustment base
    }
    
    if(pindexBest->nHeight < nPaymentUpdate_4 ){ // POS FIX
        if(pindexBest->nHeight > 526000){// Fork toggle (Has to be the first loop or else height - fork height = negative....)    
            int64_t i = ((pindexBest->nHeight - 488888) / 526000);// CURRENT_HEIGHT - FORK_HEIGHT(desired) / 6 Months = possible loops
            int64_t i2 = 0;// Base value for loop logic
    
            while(i2 <= i){// Loop for as many times as possible    
                if(i2 <= 3){// Only drop subsidy up to 9 times, regardless of loop count
                int64_t nDownSubsidy = nSubsidy / 15;// 15% step down value
                nSubsidy -= nDownSubsidy;// 10% Step down payment adjustment
                } else if(i2 <= 19){
                    int64_t nMNdownSubsidy = nMasterNodeAdjustment / 17;// 17% step up value
                    nSubsidy += nMasterNodeAdjustment + nMNdownSubsidy;// 17% Step up payment adjustment
                } else {
                    break;// Limit looping to max loop rounds
                }
    
                i2++;// Move up in loop round
            }
        } else {
            nSubsidy += nMasterNodeAdjustment;
        }
        
        //PoS block reward increase means it pays DevOps && meets proper rewards
        /*if(pindexBest->nHeight > nPaymentUpdate_4){ //nBlockForkHeight0 ==> nPaymentUpdate_4 can be found in Mining.h
            nSubsidy += 25 * COIN;
        }*/
    } else { // NEW REWARDS TAKE PLACE
        if(pindexBest->nHeight > 526000){// Fork toggle (Has to be the first loop or else height - fork height = negative....)
            
            int64_t i = ((pindexBest->nHeight - 488888) / 526000) - 1;// CURRENT_HEIGHT - FORK_HEIGHT(desired) / 6 Months = possible loops
            int64_t i2 = 0;// Base value for loop logic
            
            while(i2 <= i){// Loop for as many times as possible                
                if(nSubsidy > 10 * COIN){
                    nSubsidy -= (nSubsidy * 15) / 100;// 10% Step down payment adjustment
                } else {
                    nSubsidy = 10 * COIN;
                }
                if(nMasterNodeAdjustment < 210 * COIN){
                    nMasterNodeAdjustment += (nMasterNodeAdjustment * 17) / 100;// 17% Step up payment adjustment
                } else {
                    nMasterNodeAdjustment = 210 * COIN;
                }
                if(nSubsidy == 10 * COIN && nMasterNodeAdjustment == 210 * COIN)
                    break;                
                i2++;// Move up in loop round
            }
            nSubsidy += nMasterNodeAdjustment;
            //PoS block reward increase means it pays DevOps && meets proper rewards
            if(pindexBest->nHeight > nPaymentUpdate_4){ //nBlockForkHeight0 can be found in Mining.h permits staking fix for DevOps
                nSubsidy += 25 * COIN;
            }
        }
    }

    //premine function
    if(pindexBest->nHeight > nReservePhaseStart) {
        if(pindexBest->nMoneySupply < (nBlockRewardReserve * 100)) {
            nSubsidy = nBlockRewardReserve;
        }
    }

    // hardCap v2.1
    if(pindexBest->nMoneySupply > MAX_SINGLE_TX)
    {
        LogPrint("MINEOUT", "GetProofOfStakeReward(): create=%s nFees=%d\n", FormatMoney(nFees), nFees);
        return nFees;
    }

    LogPrint("creation", "GetProofOfStakeReward(): create=%s nCoinAge=%d\n", FormatMoney(nSubsidy), nCoinAge);
    return nSubsidy + nFees;
}

//
// Masternode coin base reward
//
int64_t GetMasternodePayment(int nHeight, int64_t blockValue)
{
    int64_t nSubsidy = 0;// Reward amount
    int64_t nSubAdjust = 113.75 * COIN;
    
    if(pindexBest->nHeight > 488888){
        nSubAdjust = 153.75 * COIN;
    }    
    nSubsidy = nSubAdjust;    
    if(pindexBest->nHeight < nPaymentUpdate_4){        
        if(pindexBest->nHeight > 526000){// Fork toggle (Has to be the first loop or else height - fork height = negative....)            
            int64_t i = ((pindexBest->nHeight - 488888) / 526000);// CURRENT_HEIGHT - FORK_HEIGHT(desired) / 6 Months = possible loops
            int64_t i2 = 0;// Base value for loop logic            
            while(i2 <= i){// Loop for as many times as possible                
                if(i2 <= 19){// Only go up subsidy up to 19 times, regardless of loop count
                    int64_t nUpSubsidy = nSubAdjust / 17;// 17% step up value
                    nSubsidy += nUpSubsidy + nSubAdjust;// 17% Step up payment adjustment
                } else {
                    break;// Limit looping to max loop rounds
                }                
                i2++;// Move up in loop round
            }
        }
    } else { // NEW REWARDS TAKE PLACE        
        if(pindexBest->nHeight > 526000){// Fork toggle (Has to be the first loop or else height - fork height = negative....)            
            int64_t i = ((pindexBest->nHeight - 488888) / 526000) - 1;// CURRENT_HEIGHT - FORK_HEIGHT(desired) / 6 Months = possible loops
            int64_t i2 = 0;// Base value for loop logic            
            while(i2 <= i){// Loop for as many times as possible                
                nSubsidy += (nSubsidy * 17) / 100;// 17% Step up payment adjustment                
                if(nSubsidy > 210 * COIN){
                    nSubsidy = 210 * COIN;
                    break;
                }
                i2++;// Move up in loop round
            }
        }
    }
    int64_t ret = nSubsidy;
    return ret;
}

//
// DevOps coin base reward
//
int64_t GetDevOpsPayment(int nHeight, int64_t blockValue)
{
    int64_t ret2 = 0;
    ret2 = 25 * COIN; // 25 KONJ per block = 10% of blocks.

    return ret2;
}
