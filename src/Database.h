//
// Created by mctrivia on 30/01/23.
//
#ifndef SHA256_LENGTH
#define SHA256_LENGTH 32
#endif

#ifndef STRINGIZE
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
#endif




#ifndef DIGIBYTECORE_DATABASECHAIN_H
#define DIGIBYTECORE_DATABASECHAIN_H

#define DIGIBYTECORE_DATABASE_CHAIN_WATCH_MAX 20


#include "BitIO.h"
#include "Blob.h"
#include "DigiAssetRules.h"
#include "DigiAssetTypes.h"
#include "DigiByteCore.h"
#include "IPFS.h"
#include "KYC.h"
#include <future>
#include <iomanip>
#include <mutex>
#include <sqlite3.h>
#include <string>
#include <vector>
#include "Database_Statement.h"
#include "Database_LockedStatement.h"

struct AddressStats {
    unsigned int time;            //time block start
    unsigned int created;         //number of addresses created for the first time
    unsigned int used;            //number of addresses used during time block
    unsigned int withAssets;      //number of addresses with assets
    unsigned int over0;           //address count with any DGB
    unsigned int over1;           //address count with at least 1 DGB
    unsigned int over1k;          //address count with at least 1000 DGB
    unsigned int over1m;          //address count with at least 1000000 DGB
    unsigned int quantumInsecure; //number of addresses a quantum computer could steel from
    unsigned int total;           //total number of addresses that have ever existed up to this point
};

struct AlgoStats {
    unsigned int time;   //time block start
    unsigned int algo;   //algo number
    unsigned int blocks; //number of blocks created with that algo
    double difficultyMin;
    double difficultyMax;
    double difficultyAvg;
};

struct VoteCount {
    std::string address;
    uint64_t count;
};











class Database {
private:
    sqlite3* _db = nullptr;
    Statement _stmtCheckFlag;
    Statement _stmtSetFlag;
    Statement _stmtGetBlockHeight;
    Statement _stmtInsertBlock;
    Statement _stmtGetBlockHash;
    Statement _stmtCreateUTXO;
    Statement _stmtSpendUTXO;
    Statement _stmtIsWatchAddress;
    Statement _stmtAddWatchAddress;
    Statement _stmtGetSpendingAddress;
    Statement _stmtAddExchangeRate;
    Statement _stmtAddKYC;
    Statement _stmtRevokeKYC;
    Statement _stmtPruneUTXOs;
    Statement _stmtExchangeRatesAtHeight;
    Statement _stmtPruneExchangeRate;
    Statement _stmtGetVoteCountAtHeight;
    Statement _stmtPruneVote;
    Statement _stmtAddVote;
    Statement _stmtGetVoteCount;
    Statement _stmtGetAssetUTXO;
    Statement _stmtGetAssetHolders;
    Statement _stmtAddAsset;
    Statement _stmtUpdateAsset;
    Statement _stmtGetAssetIndex;
    Statement _stmtGetAssetIndexOnUTXO;
    Statement _stmtGetHeightAssetCreated;
    Statement _stmtGetAssetRules;
    Statement _stmtGetAsset;
    Statement _stmtGetKYC;
    Statement _stmtGetValidExchangeRate;
    Statement _stmtGetCurrentExchangeRate;
    Statement _stmtGetNextIPFSJob;
    Statement _stmtSetIPFSPauseSync;
    Statement _stmtClearNextIPFSJob_a;
    Statement _stmtClearNextIPFSJob_b;
    Statement _stmtInsertIPFSJob;
    Statement _stmtClearIPFSPause;
    Statement _stmtSetIPFSLockSync;
    Statement _stmtSetIPFSLockJob;
    Statement _stmtSetIPFSPauseJob;
    Statement _stmtGetDomainAssetId;
    Statement _stmtAddDomain;
    Statement _stmtRevokeDomain;
    Statement _stmtSetDomainMasterAssetId_a;
    Statement _stmtSetDomainMasterAssetId_b;
    Statement _stmtGetPermanentPaid;
    Statement _stmtRemoveNonReachable;
    Statement _stmtInsertPermanent;
    Statement _stmtRepinAssets;
    Statement _stmtRepinPermanentSpecific;
    Statement _stmtAddAssetToPool;
    Statement _stmtIsAssetInPool;
    Statement _stmtIsAssetInAPool;
    Statement _stmtPSPFindBadAsset;
    Statement _stmtPSPDeleteBadAsset;
    Statement _stmtDeletePermanent;
    Statement _stmtIsInPermanent;
    Statement _stmtNumberOfIPFSJobs;
    Statement _stmtGetTotalAssetCounta;
    Statement _stmtGetTotalAssetCountb;
    Statement _stmtGetOriginalAssetCounta;
    Statement _stmtGetOriginalAssetCountb;
    Statement _stmtGetAssetTxHistorya;
    Statement _stmtGetAssetTxHistoryb;
    Statement _stmtGetAddressTxHistory;
    Statement _stmtGetAssetCreateByAddress;
    Statement _stmtGetValidUTXO;

public:
    std::string printProfilingInfo() {
        // Header
        std::ostringstream oss;
        oss << std::right << std::setw(30) << "Statement Name"
                  << std::setw(20) << "Total Time (us)"
                  << std::setw(20) << "Time/Transaction (us)"
                  << std::setw(20) << "Transactions" << std::endl;
        std::cout << std::string(90, '-') << std::endl; // Separator
        std::string result=oss.str();

        // Print info for each statement
        result+=printStatementInfo("_stmtCheckFlag", _stmtCheckFlag);
        result+=printStatementInfo("_stmtSetFlag", _stmtSetFlag);
        result+=printStatementInfo("_stmtGetBlockHeight", _stmtGetBlockHeight);
        result+=printStatementInfo("_stmtInsertBlock", _stmtInsertBlock);
        result+=printStatementInfo("_stmtGetBlockHash", _stmtGetBlockHash);
        result+=printStatementInfo("_stmtCreateUTXO", _stmtCreateUTXO);
        result+=printStatementInfo("_stmtSpendUTXO", _stmtSpendUTXO);
        result+=printStatementInfo("_stmtIsWatchAddress", _stmtIsWatchAddress);
        result+=printStatementInfo("_stmtAddWatchAddress", _stmtAddWatchAddress);
        result+=printStatementInfo("_stmtGetSpendingAddress", _stmtGetSpendingAddress);
        result+=printStatementInfo("_stmtAddExchangeRate", _stmtAddExchangeRate);
        result+=printStatementInfo("_stmtAddKYC", _stmtAddKYC);
        result+=printStatementInfo("_stmtRevokeKYC", _stmtRevokeKYC);
        result+=printStatementInfo("_stmtPruneUTXOs", _stmtPruneUTXOs);
        result+=printStatementInfo("_stmtExchangeRatesAtHeight", _stmtExchangeRatesAtHeight);
        result+=printStatementInfo("_stmtPruneExchangeRate", _stmtPruneExchangeRate);
        result+=printStatementInfo("_stmtGetVoteCountAtHeight", _stmtGetVoteCountAtHeight);
        result+=printStatementInfo("_stmtPruneVote", _stmtPruneVote);
        result+=printStatementInfo("_stmtAddVote", _stmtAddVote);
        result+=printStatementInfo("_stmtGetVoteCount", _stmtGetVoteCount);
        result+=printStatementInfo("_stmtGetAssetUTXO", _stmtGetAssetUTXO);
        result+=printStatementInfo("_stmtGetAssetHolders", _stmtGetAssetHolders);
        result+=printStatementInfo("_stmtAddAsset", _stmtAddAsset);
        result+=printStatementInfo("_stmtUpdateAsset", _stmtUpdateAsset);
        result+=printStatementInfo("_stmtGetAssetIndex", _stmtGetAssetIndex);
        result+=printStatementInfo("_stmtGetAssetIndexOnUTXO", _stmtGetAssetIndexOnUTXO);
        result+=printStatementInfo("_stmtGetHeightAssetCreated", _stmtGetHeightAssetCreated);
        result+=printStatementInfo("_stmtGetAssetRules", _stmtGetAssetRules);
        result+=printStatementInfo("_stmtGetAsset", _stmtGetAsset);
        result+=printStatementInfo("_stmtGetKYC", _stmtGetKYC);
        result+=printStatementInfo("_stmtGetValidExchangeRate", _stmtGetValidExchangeRate);
        result+=printStatementInfo("_stmtGetCurrentExchangeRate", _stmtGetCurrentExchangeRate);
        result+=printStatementInfo("_stmtGetNextIPFSJob", _stmtGetNextIPFSJob);
        result+=printStatementInfo("_stmtSetIPFSPauseSync", _stmtSetIPFSPauseSync);
        result+=printStatementInfo("_stmtClearNextIPFSJob_a", _stmtClearNextIPFSJob_a);
        result+=printStatementInfo("_stmtClearNextIPFSJob_b", _stmtClearNextIPFSJob_b);
        result+=printStatementInfo("_stmtInsertIPFSJob", _stmtInsertIPFSJob);
        result+=printStatementInfo("_stmtClearIPFSPause", _stmtClearIPFSPause);
        result+=printStatementInfo("_stmtSetIPFSLockSync", _stmtSetIPFSLockSync);
        result+=printStatementInfo("_stmtSetIPFSLockJob", _stmtSetIPFSLockJob);
        result+=printStatementInfo("_stmtSetIPFSPauseJob", _stmtSetIPFSPauseJob);
        result+=printStatementInfo("_stmtGetDomainAssetId", _stmtGetDomainAssetId);
        result+=printStatementInfo("_stmtAddDomain", _stmtAddDomain);
        result+=printStatementInfo("_stmtRevokeDomain", _stmtRevokeDomain);
        result+=printStatementInfo("_stmtSetDomainMasterAssetId_a", _stmtSetDomainMasterAssetId_a);
        result+=printStatementInfo("_stmtSetDomainMasterAssetId_b", _stmtSetDomainMasterAssetId_b);
        result+=printStatementInfo("_stmtGetPermanentPaid", _stmtGetPermanentPaid);
        result+=printStatementInfo("_stmtRemoveNonReachable", _stmtRemoveNonReachable);
        result+=printStatementInfo("_stmtInsertPermanent", _stmtInsertPermanent);
        result+=printStatementInfo("_stmtRepinAssets", _stmtRepinAssets);
        result+=printStatementInfo("_stmtRepinPermanentSpecific", _stmtRepinPermanentSpecific);
        result+=printStatementInfo("_stmtAddAssetToPool", _stmtAddAssetToPool);
        result+=printStatementInfo("_stmtIsAssetInPool", _stmtIsAssetInPool);
        result+=printStatementInfo("_stmtIsAssetInAPool", _stmtIsAssetInAPool);
        result+=printStatementInfo("_stmtPSPFindBadAsset", _stmtPSPFindBadAsset);
        result+=printStatementInfo("_stmtPSPDeleteBadAsset", _stmtPSPDeleteBadAsset);
        result+=printStatementInfo("_stmtDeletePermanent", _stmtDeletePermanent);
        result+=printStatementInfo("_stmtIsInPermanent", _stmtIsInPermanent);
        result+=printStatementInfo("_stmtNumberOfIPFSJobs", _stmtNumberOfIPFSJobs);
        result+=printStatementInfo("_stmtGetTotalAssetCounta", _stmtGetTotalAssetCounta);
        result+=printStatementInfo("_stmtGetTotalAssetCountb", _stmtGetTotalAssetCountb);
        result+=printStatementInfo("_stmtGetOriginalAssetCounta", _stmtGetOriginalAssetCounta);
        result+=printStatementInfo("_stmtGetOriginalAssetCountb", _stmtGetOriginalAssetCountb);
        result+=printStatementInfo("_stmtGetAssetTxHistorya", _stmtGetAssetTxHistorya);
        result+=printStatementInfo("_stmtGetAssetTxHistoryb", _stmtGetAssetTxHistoryb);
        result+=printStatementInfo("_stmtGetAddressTxHistory", _stmtGetAddressTxHistory);
        result+=printStatementInfo("_stmtGetAssetCreateByAddress", _stmtGetAssetCreateByAddress);
        result+=printStatementInfo("_stmtGetValidUTXO", _stmtGetValidUTXO);
        return result;
    }

    std::string printStatementInfo(const std::string& name, const Statement& stmt) {
        long long totalDuration = stmt.getTotalLockDuration();
        int transactions = stmt.getLockCount();
        long long avgDuration = transactions > 0 ? totalDuration / transactions : 0;

        std::ostringstream oss;
        oss << std::right << std::setw(30) << name
            << std::setw(20) << totalDuration
            << std::setw(20) << avgDuration
            << std::setw(20) << transactions << std::endl;

        return oss.str();
    }
private:


    //locks
    std::mutex _mutexGetNextIPFSJob;
    std::mutex _mutexRemoveIPFSJob;

    void buildTables(unsigned int dbVersionNumber = 0);
    void initializeClassValues();

    //flag table
    int getFlagInt(const std::string& flag);
    void setFlagInt(const std::string& flag, int state);
    std::map<std::string, int> _flagState;

    //exchangeWatch table
    std::vector<std::string> _exchangeWatchAddresses;

    //TestHelpers
    static int defaultCallback(void* NotUsed, int argc, char** argv, char** azColName);

    //helpers
    static int executeSqliteStepWithRetry(sqlite3_stmt* stmt, int maxRetries = 3, int sleepDurationMs = 100);
    void executeSQLStatement(const std::string& query, const std::exception& errorToThrowOnFail);

    //ipfs ram db values
    std::vector<std::pair<std::string, uint64_t>> _ipfsCurrentlyPaused;
    static std::map<std::string, IPFSCallbackFunction> _ipfsCallbacks;

    //DigiBYte Domain ram values
    std::vector<std::string> _masterDomainAssetId = {};

public:
    static std::string _lastErrorMessage;

    struct exchangeRateHistoryValue {
        unsigned int height;
        std::string address;
        unsigned char index;
        double value;
    };

    //constructor
    Database(const std::string& newFileName = "chain.db");
    ~Database();

    //performance related
    void startTransaction();
    void endTransaction();
    void
    disableWriteVerification(); //on power failure not all commands may be written.  If using need to check at startup

    //reset database
    void reset(); //used in case of roll back exceeding pruned history

    //assets table
    uint64_t addAsset(const DigiAsset& asset);
    DigiAsset getAsset(uint64_t assetIndex, uint64_t amount = 0);
    uint64_t getAssetIndex(const std::string& assetId, const std::string& txid = "", unsigned int vout = 0);
    std::vector<uint64_t> getAssetIndexes(const std::string& assetId);

    //assets table not to be used on assets that may have more than one assetIndex
    DigiAssetRules getRules(const std::string& assetId);
    unsigned int getAssetHeightCreated(const std::string& assetId, unsigned int backupHeight, uint64_t& assetIndex);

    //block table
    void insertBlock(uint height, const std::string& hash, unsigned int time, unsigned char algo, double difficulty);
    std::string getBlockHash(uint height);
    uint getBlockHeight();
    void clearBlocksAboveHeight(uint height);

    //exchange table
    void addExchangeRate(const std::string& address, unsigned int index, unsigned int height, double exchangeRate);
    void pruneExchange(unsigned int height);
    double getAcceptedExchangeRate(const ExchangeRate& rate, unsigned int height);
    double getCurrentExchangeRate(const ExchangeRate& rate);
    std::vector<exchangeRateHistoryValue> getExchangeRatesAtHeight(unsigned int height);

    //exchange watch table
    bool isWatchAddress(const std::string& address);
    void addWatchAddress(const std::string& address);

    //flag table
    int getBeenPrunedExchangeHistory(); //-1 = never, above=height which anything below may be pruned
    int getBeenPrunedUTXOHistory();     //-1 = never, above=height which anything below may be pruned
    int getBeenPrunedVoteHistory();     //-1 = never, above=height which anything below may be pruned
    bool getBeenPrunedNonAssetUTXOHistory();
    void setBeenPrunedExchangeHistory(int height); //-1 = never
    void setBeenPrunedUTXOHistory(int height);     //-1 = never
    void setBeenPrunedVoteHistory(int height);     //-1 = never
    void setBeenPrunedNonAssetUTXOHistory(bool state);

    //kyc table
    void
    addKYC(const std::string& address, const std::string& country, const std::string& name, const std::string& hash,
           unsigned int height);
    void revokeKYC(const std::string& address, unsigned int height);
    KYC getAddressKYC(const std::string& address);

    //utxos table
    void createUTXO(const AssetUTXO& value, unsigned int heightCreated, bool assetIssuance);
    void spendUTXO(const std::string& txid, unsigned int vout, unsigned int heightSpent, const std::string& spentTXID);
    std::string getSendingAddress(const std::string& txid, unsigned int vout);
    void pruneUTXO(unsigned int height);

    //utxo table asset related
    AssetUTXO getAssetUTXO(const std::string& txid, unsigned int vout);
    std::vector<AssetHolder> getAssetHolders(uint64_t assetIndex);
    uint64_t getTotalAssetCount(uint64_t assetIndex); //returns total count of specific variant
    uint64_t getTotalAssetCount(const std::string& assetId); //returns total count of specific asset(sum of all variants)
    uint64_t getOriginalAssetCount(uint64_t assetIndex);
    uint64_t getOriginalAssetCount(const std::string& assetId);
    std::vector<std::string> getAssetTxHistory(uint64_t assetIndex);
    std::vector<std::string> getAssetTxHistory(const std::string& assetId);

    //utxo table address related
    std::vector<AssetUTXO> getAddressUTXOs(const std::string& address, unsigned int minConfirms=0, unsigned int maxConfirms=std::numeric_limits<unsigned int>::max());
    std::vector<std::string> getAddressTxList(const std::string& address, unsigned int minHeight=1, unsigned int maxHeight=std::numeric_limits<unsigned int>::max());
    std::vector<uint64_t> getAssetsCreatedByAddress(const std::string& address);

    //vote table
    void addVote(const std::string& address, unsigned int assetIndex, uint64_t count, unsigned int height);
    void pruneVote(unsigned int height);
    std::vector<VoteCount> getVoteCounts(unsigned int assetIndex);

    //IPFS table
    static void registerIPFSCallback(const std::string& callbackSymbol, const IPFSCallbackFunction& callback);
    void getNextIPFSJob(unsigned int& jobIndex, std::string& cid, std::string& sync, std::string& extra,
                        unsigned int& maxSleep, IPFSCallbackFunction& callback);
    void pauseIPFSSync(unsigned int jobIndex, const std::string& sync, unsigned int pauseLengthInSeconds = 3600);
    void removeIPFSJob(unsigned int jobIndex, const std::string& sync);
    unsigned int addIPFSJob(const std::string& cid, const std::string& sync = "pin", const std::string& extra = "",
                            unsigned int maxSleep = 0, const std::string& callbackSymbol = "");
    std::promise<std::string>
    addIPFSJobPromise(const std::string& cid, const std::string& sync = "", unsigned int maxTime = 0);
    IPFSCallbackFunction& getIPFSCallback(const std::string& callbackSymbol);
    unsigned int getIPFSJobCount();

    //DigiByte Domain table(these should only ever be called by DigiByteDomain.cpp
    void revokeDomain(const std::string& domain);
    void addDomain(const std::string& domain, const std::string& assetId);
    std::string getDomainAssetId(const std::string& domain, bool returnErrorIfRevoked = true);
    std::string getDomainAddress(const std::string& domain);
    bool isMasterDomainAssetId(const std::string& assetId) const;
    bool isActiveMasterDomainAssetId(const std::string& assetId) const;
    void setMasterDomainAssetId(const std::string& assetId);
    void setDomainCompromised();
    bool isDomainCompromised() const;

    //Permanent table
    void addToPermanent(unsigned int poolIndex, const std::string& cid);
    void removeFromPermanent(unsigned int poolIndex, const std::string& cid, bool unpin);
    void repinPermanent(unsigned int poolIndex);
    void unpinPermanent(unsigned int poolIndex);
    void addAssetToPool(unsigned int poolIndex, unsigned int assetIndex);
    void removeAssetFromPool(unsigned int poolIndex, const std::string& assetId, bool unpin);
    bool isAssetInPool(unsigned int poolIndex, unsigned int assetIndex);

    //stats table
    //warning a new stats table is created for every timeFrame.  It is not recommended to allow users direct access to this value
    void updateStats(unsigned int timeFrame = 86400);
    bool canGetAlgoStats();
    bool canGetAddressStats();
    std::vector<AlgoStats> getAlgoStats(unsigned int start = 0, unsigned int end = std::numeric_limits<unsigned int>::max(), unsigned int timeFrame = 86400);
    std::vector<AddressStats> getAddressStats(unsigned int start = 0, unsigned int end = std::numeric_limits<unsigned int>::max(), unsigned int timeFrame = 86400);

    /*
    ███████╗██████╗ ██████╗  ██████╗ ██████╗ ███████╗
    ██╔════╝██╔══██╗██╔══██╗██╔═══██╗██╔══██╗██╔════╝
    █████╗  ██████╔╝██████╔╝██║   ██║██████╔╝███████╗
    ██╔══╝  ██╔══██╗██╔══██╗██║   ██║██╔══██╗╚════██║
    ███████╗██║  ██║██║  ██║╚██████╔╝██║  ██║███████║
    ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝
     */

    class exception : public std::exception {
    public:
        char* what() {
            _lastErrorMessage = "Something went wrong with database";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };

    class exceptionFailedToOpen : public exception {
    public:
        char* what() {
            _lastErrorMessage = "Couldn't open or create the database";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };

    class exceptionFailedSQLCommand : public exception {
        char* what() {
            _lastErrorMessage = "Failed to create sql command";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };


    class exceptionFailedToCreateTable : public exceptionFailedSQLCommand {
    public:
        char* what() {
            _lastErrorMessage = "Failed to create table";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };

    class exceptionFailedInsert : public exceptionFailedSQLCommand {
    public:
        char* what() {
            _lastErrorMessage = "Failed to insert into table";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };

    class exceptionFailedSelect : public exceptionFailedSQLCommand {
    public:
        char* what() {
            _lastErrorMessage = "Failed to select from table";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };

    class exceptionFailedUpdate : public exceptionFailedSQLCommand {
    public:
        char* what() {
            _lastErrorMessage = "Failed to update table";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };

    class exceptionFailedDelete : public exceptionFailedSQLCommand {
    public:
        char* what() {
            _lastErrorMessage = "Failed to delete from table";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };

    class exceptionCreatingStatement : public exceptionFailedSQLCommand {
    public:
        char* what() {
            _lastErrorMessage = "Failed to create statement for table";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };

    class exceptionFailedReset : public exception {
    public:
        char* what() {
            _lastErrorMessage = "Failed to reset table";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };

    class exceptionDataPruned : public exception {
    public:
        char* what() {
            _lastErrorMessage = "The requested data has been pruned";
            return const_cast<char*>(_lastErrorMessage.c_str());
        }
    };
};


#endif //DIGIBYTECORE_DATABASECHAIN_H
